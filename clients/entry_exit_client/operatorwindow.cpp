#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "operatorwindow.hpp"
#include "./ui_operatorwindow.h"

OperatorWindow::OperatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OperatorWindow)
    , sock(new QTcpSocket(this))
    , timer(new QTimer(this))
{
    this->ui->setupUi(this);

    this->connect(this->ui->pushButton_confirm, SIGNAL(clicked()), this, SLOT(confirm()));
    this->connect(this->ui->radioButton_car, SIGNAL(clicked()), this, SLOT(select_car()));
    this->connect(this->ui->radioButton_motorcycle, SIGNAL(clicked()), this, SLOT(select_motorcycle()));
    this->connect(this->ui->radioButton_trunk, SIGNAL(clicked()), this, SLOT(select_trunk()));
    this->connect(this->ui->radioButton_van, SIGNAL(clicked()), this, SLOT(select_van()));
    this->connect(this->timer, SIGNAL(timeout()), this, SLOT(update()));
}

OperatorWindow::~OperatorWindow()
{
    delete this->ui;
    delete this->sock;
}

void OperatorWindow::confirm() {
    this->sock->connectToHost(this->server_ip, this->port.toInt());

    QJsonObject json_obj, auth, operation, arguments;
    auth["account"] = this->account;
    auth["password"] = this->password;
    if (this->client_type == "entry") {
        operation["name"] = "vehicle_enter";
    } else if (this->client_type == "exit") {
        operation["name"] = "vehicle_exit";
    }
    arguments["plate_number"] = this->ui->lineEdit_account->text();

    operation["arguments"] = arguments;
    json_obj["auth"] = auth;
    json_obj["operation"] = operation;
    QJsonDocument doc_send(json_obj);
    QString json = doc_send.toJson(QJsonDocument::Compact).toStdString().c_str();
    qDebug("json %s", json.toStdString().c_str());

    if(this->sock->waitForConnected(5000)) {
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
            this->timer->setInterval(3000);
            this->timer->setSingleShot(true);
            this->timer->start();
            QJsonObject return_val = root["return_val"].toObject();
            if (this->client_type == "entry") {
                int floor = return_val["floor"].toInt();
                int spot = return_val["spot"].toInt();
                this->d->update("WELCOME " + this->ui->lineEdit_account->text() + "\n"
                                + "YOUR SPOT\n" +
                                "FLOOR " + QString(std::to_string(floor).c_str()) +
                                " SPOT " + QString(std::to_string(spot).c_str()));
            } else {
                int fee = return_val["fee"].toInt();
                this->d->update(this->ui->lineEdit_account->text() + "\n"
                                + "PLEASE PAY YOUR FEE\n$" +
                                QString(std::to_string(fee).c_str()));
            }
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
    }
}

void OperatorWindow::select_car() {
    this->car_type = "car";
}

void OperatorWindow::select_motorcycle() {
    this->car_type = "motorcycle";
}

void OperatorWindow::select_trunk() {
    this->car_type = "trunk";
}

void OperatorWindow::select_van() {
    this->car_type = "van";
}

void OperatorWindow::update() {
    this->d->update("WELCOME");
}