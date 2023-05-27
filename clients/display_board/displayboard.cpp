#include "displayboard.hpp"
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "./ui_displayboard.h"

DisplayBoard::DisplayBoard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DisplayBoard)
    , sock(new QTcpSocket(this))
    , timer(new QTimer(this))
{
    this->ui->setupUi(this);

    this->timer->setInterval(500);
    this->connect(this->timer, SIGNAL(timeout()), this, SLOT(update()));
    this->timer->start();
}

DisplayBoard::~DisplayBoard()
{
    delete ui;
}

void DisplayBoard::update() {
    this->sock->connectToHost(this->server_ip, this->port.toInt());
    if(this->sock->waitForConnected(5000)) {
        QJsonObject json_obj, auth, operation, arguments;
        auth["account"] = this->account;
        auth["password"] = this->password;
        arguments["floor"] = this->floor;
        operation["name"] = "get_available_spots";
        operation["arguments"] = arguments;
        json_obj["auth"] = auth;
        json_obj["operation"] = operation;
        QJsonDocument doc_send(json_obj);
        QString json = doc_send.toJson(QJsonDocument::Compact).toStdString().c_str();
        qDebug("get_available_spots json %s", json.toStdString().c_str());
        this->sock->write(json.toStdString().c_str());
        qDebug() << "connect succ";
        this->sock->waitForReadyRead();
        QByteArray data = this->sock->readAll();
        qDebug("receiving data %s", data.toStdString().c_str());
        this->sock->close();
        QJsonDocument doc_recv = QJsonDocument::fromJson(data);
        QJsonObject root = doc_recv.object();
        QJsonObject state = root["state"].toObject();
        if (state["succ"].toInt() == 1) {
            qDebug() << "server succ";
            QJsonObject return_val = root["return_val"].toObject();
            this->ui->label_car_availble->setText(QString(std::to_string(return_val["car_available"].toInt()).c_str()));
            this->ui->label_motorcycle_available->setText(QString(std::to_string(return_val["motorcycle_available"].toInt()).c_str()));
            this->ui->label_van_available->setText(QString(std::to_string(return_val["van_available"].toInt()).c_str()));
            this->ui->label_trunk_available->setText(QString(std::to_string(return_val["trunk_available"].toInt()).c_str()));
            this->ui->label_floor->setText("FLOOR " + this->floor);
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
            this->close();
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
        this->close();
    }
}
