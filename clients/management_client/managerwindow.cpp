#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QInputDialog>
#include "managerwindow.hpp"
#include "editspotswindow.hpp"
#include "./ui_managerwindow.h"

ManagerWindow::ManagerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ManagerWindow)
    , sock(new QTcpSocket(this))
    , sub_ui(new EditSpotsWindow(this))
{
    this->ui->setupUi(this);
    this->sub_ui->hide();

    this->connect(this->ui->pushButton_query_available_spots, SIGNAL(clicked()), this, SLOT(query_available_spots()));
    this->connect(this->ui->pushButton_find_my_car, SIGNAL(clicked()), this, SLOT(find_my_car()));
    this->connect(this->ui->pushButton_edit_spots, SIGNAL(clicked()), this, SLOT(edit_spots()));
    this->connect(this->ui->pushButton_add_customer, SIGNAL(clicked()), this, SLOT(add_customer()));
}

ManagerWindow::~ManagerWindow()
{
    delete this->ui;
    delete this->sock;
}

void ManagerWindow::query_available_spots() {
    QString floor = QInputDialog::getText(this, "query available spots", "Please input floor:");

    this->sock->connectToHost(this->server_ip, this->port.toInt());
    if(this->sock->waitForConnected(5000)) {
        QJsonObject json_obj, auth, operation, arguments;
        auth["account"] = this->account;
        auth["password"] = this->password;
        arguments["floor"] = floor;
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
            QString str = "car available: " + QString(std::to_string(return_val["car_available"].toInt()).c_str()) + "\n"
                          + "motorcycle available: " + QString(std::to_string(return_val["motorcycle_available"].toInt()).c_str()) + "\n"
                          + "van available: " + QString(std::to_string(return_val["van_available"].toInt()).c_str()) + "\n"
                          + "trunk available: " + QString(std::to_string(return_val["trunk_available"].toInt()).c_str()) + "\n";
            QMessageBox::information(this, "info", str, QMessageBox::Ok);
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
    }
}

void ManagerWindow::find_my_car() {
    QString license = QInputDialog::getText(this, tr("find my car"), tr("please input license plate number:"));
    this->sock->connectToHost(this->server_ip, this->port.toInt());
    if(this->sock->waitForConnected(5000)) {
        QJsonObject json_obj, auth, operation, arguments;
        auth["account"] = this->account;
        auth["password"] = this->password;
        arguments["plate_number"] = license;
        operation["name"] = "query_vehicle";
        operation["arguments"] = arguments;
        json_obj["auth"] = auth;
        json_obj["operation"] = operation;
        QJsonDocument doc_send(json_obj);
        QString json = doc_send.toJson(QJsonDocument::Compact).toStdString().c_str();
        qDebug("query_vehicle json %s", json.toStdString().c_str());
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
            QString str = QString("Your vehicle position:\n")
                          + "floor: " + QString(std::to_string(return_val["floor"].toInt()).c_str()) + "\n"
                          + "spot: " + QString(std::to_string(return_val["spot"].toInt()).c_str());
            QMessageBox::information(this, "info", str, QMessageBox::Ok);
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
    }
}

void ManagerWindow::edit_spots() {
    this->sub_ui->server_ip = this->server_ip;
    this->sub_ui->port = this->port;
    this->sub_ui->account = this->account;
    this->sub_ui->password = this->password;
    if (this->sub_ui->update_data()) {
        this->hide();
        this->sub_ui->show();
    }
}

void ManagerWindow::add_customer() {
    QString c_account = QInputDialog::getText(this, tr("add customer"), tr("please input customer account:"));
    QString c_password = QInputDialog::getText(this, tr("add customer"), tr("please input customer password:"));

    this->sock->connectToHost(this->server_ip, this->port.toInt());
    if(this->sock->waitForConnected(5000)) {
        QJsonObject json_obj, auth, operation, arguments;
        auth["account"] = this->account;
        auth["password"] = this->password;
        arguments["account"] = c_account;
        arguments["password"] = c_password;
        operation["name"] = "add_account";
        operation["arguments"] = arguments;
        json_obj["auth"] = auth;
        json_obj["operation"] = operation;
        QJsonDocument doc_send(json_obj);
        QString json = doc_send.toJson(QJsonDocument::Compact).toStdString().c_str();
        qDebug("add_account json %s", json.toStdString().c_str());
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
            QMessageBox::information(this, "info", "add customer success", QMessageBox::Ok);
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
    }
}
