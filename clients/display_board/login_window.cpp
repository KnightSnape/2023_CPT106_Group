#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "login_window.hpp"
#include "./ui_login.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , sock(new QTcpSocket(this))
{
    this->ui->setupUi(this);

    this->connect(this->ui->pushButton_login, SIGNAL(clicked()), this, SLOT(login()));
}

LoginWindow::~LoginWindow()
{
    delete this->ui;
    delete this->sock;
}

void LoginWindow::login() {
    QJsonObject json_obj, auth, operation, arguments;
    auth["account"] = this->ui->lineEdit_account->text();
    auth["password"] = this->ui->lineEdit_password->text();
    operation["name"] = "ping";
    operation["arguments"] = arguments;
    json_obj["auth"] = auth;
    json_obj["operation"] = operation;
    QJsonDocument doc(json_obj);
    QString json = doc.toJson(QJsonDocument::Compact).toStdString().c_str();
    qDebug("login json %s", json.toStdString().c_str());
    this->ui->pushButton_login->setText("Logging");
    this->ui->pushButton_login->setDisabled(true);

    this->sock->connectToHost(this->ui->lineEdit_server_ip->text(), this->ui->lineEdit_port->text().toUInt());
    if(this->sock->waitForConnected(5000)) {
        this->sock->write(json.toStdString().c_str());
        qDebug() << "connect succ";
        this->sock->waitForReadyRead();
        QByteArray data = this->sock->readAll();
        qDebug("receiving data %s", data.toStdString().c_str());
        this->sock->close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject root = doc.object();
        QJsonObject state = root["state"].toObject();
        if (state["succ"].toInt() == 1) {
            qDebug() << "server succ";
            this->succ = true;
            this->account = this->ui->lineEdit_account->text();
            this->password = this->ui->lineEdit_password->text();
            this->server_ip = this->ui->lineEdit_server_ip->text();
            this->port = this->ui->lineEdit_port->text();
            this->floor = this->ui->lineEdit_floor->text();
            this->close();
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
            this->ui->pushButton_login->setDisabled(false);
            this->ui->pushButton_login->setText("Login");
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
        this->ui->pushButton_login->setDisabled(false);
        this->ui->pushButton_login->setText("Login");
    }
}
