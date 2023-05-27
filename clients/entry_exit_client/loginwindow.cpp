#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "loginwindow.hpp"
#include "./ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , sock(new QTcpSocket(this))
{
    this->ui->setupUi(this);

    this->connect(this->ui->pushButton_login_entry, SIGNAL(clicked()), this, SLOT(login_entry()));
    this->connect(this->ui->pushButton_login_exit, SIGNAL(clicked()), this, SLOT(login_exit()));
}

LoginWindow::~LoginWindow()
{
    delete this->ui;
    delete this->sock;
}

void LoginWindow::login_entry() {
    this->ui->pushButton_login_entry->setText("Logging As \n Entry");
    this->ui->pushButton_login_entry->setDisabled(true);
    this->client_type = "entry";
    if(this->login()) {
        this->close();
    } else {
        this->ui->pushButton_login_entry->setDisabled(false);
        this->ui->pushButton_login_entry->setText("Login As \n Entry");
    }
}

void LoginWindow::login_exit() {
    this->ui->pushButton_login_exit->setText("Logging As \n Exit");
    this->ui->pushButton_login_exit->setDisabled(true);
    this->client_type = "exit";
    if(this->login()) {
        this->close();
    } else {
        this->ui->pushButton_login_exit->setDisabled(false);
        this->ui->pushButton_login_exit->setText("Login As \n Exit");
    }
}

bool LoginWindow::login() {
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
            return true;
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
            return false;
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
        return false;
    }
}
