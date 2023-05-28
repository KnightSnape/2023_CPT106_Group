#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "editspotswindow.hpp"
#include "./ui_editspotswindow.h"

EditSpotsWindow::EditSpotsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EWindow)
    , sock(new QTcpSocket(this))
{
    this->ui->setupUi(this);

    this->connect(this->ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));
    this->connect(this->ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(exit()));
}

EditSpotsWindow::~EditSpotsWindow()
{
    delete this->ui;
    delete this->sock;
}

bool EditSpotsWindow::update_data() {

    this->ui->tableWidget->clear();

    QJsonObject json_obj, auth, operation, arguments;
    auth["account"] = this->account;
    auth["password"] = this->password;
    operation["name"] = "query_spots";
    operation["arguments"] = arguments;
    json_obj["auth"] = auth;
    json_obj["operation"] = operation;
    QJsonDocument doc(json_obj);
    QString json = doc.toJson(QJsonDocument::Compact).toStdString().c_str();
    qDebug("query_spots json %s", json.toStdString().c_str());

    this->sock->connectToHost(this->server_ip, this->port.toUInt());
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
            QJsonObject return_val = root["return_val"].toObject();
            QJsonObject floors_obj = return_val["floor"].toObject();
            QStringList floors = floors_obj.keys();
            int count = 0;
            foreach(QString floor, floors) {
                QJsonObject spots_obj = floors_obj[floor].toObject()["spots"].toObject();
                QStringList spots = spots_obj.keys();
                foreach(QString spot, spots) {
                    count++;
                }
            }
            if (count < 100) count = 100;
            this->ui->tableWidget->setRowCount(count);
            int row = 0;
            foreach(QString floor, floors) {
                QJsonObject spots_obj = floors_obj[floor].toObject()["spots"].toObject();
                QStringList spots = spots_obj.keys();
                foreach(QString spot, spots) {
                    QTableWidgetItem *w_floor = new QTableWidgetItem(), *w_spot = new QTableWidgetItem(), *w_type = new QTableWidgetItem();
                    w_floor->setText(floor);
                    w_spot->setText(spot);
                    w_type->setText(spots_obj[spot].toString());
                    this->ui->tableWidget->setItem(row, 0, w_floor);
                    this->ui->tableWidget->setItem(row, 1, w_spot);
                    this->ui->tableWidget->setItem(row, 2, w_type);
                    row++;
                }
            }
            QStringList q;
            q.append("floor");
            q.append("spot");
            q.append("type");
            this->ui->tableWidget->setHorizontalHeaderLabels(q);
            this->ui->tableWidget->verticalHeader()->hide();
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
    return false;
}

void EditSpotsWindow::save() {
    QJsonObject json_obj, auth, operation, arguments;
    auth["account"] = this->account;
    auth["password"] = this->password;
    operation["name"] = "update_spots";

    arguments["floor"] = QJsonObject();

    for (int i = 0; i < this->ui->tableWidget->rowCount(); ++i) {
        if ((this->ui->tableWidget->item(i, 0) == nullptr) ||
            (this->ui->tableWidget->item(i, 1) == nullptr) ||
            (this->ui->tableWidget->item(i, 2) == nullptr) ) continue;

        if (!this->ui->tableWidget->item(i, 0)->text().isEmpty() &&
            !this->ui->tableWidget->item(i, 1)->text().isEmpty() &&
            !this->ui->tableWidget->item(i, 2)->text().isEmpty()) {
//            qDebug() << this->ui->tableWidget->item(i, 0)->text() << this->ui->tableWidget->item(i, 1)->text() << this->ui->tableWidget->item(i, 2)->text();

            QJsonObject tmp_floor = arguments["floor"].toObject();
//            qDebug() << "tmp_floor" << tmp_floor;
            QJsonObject tmp_floor_num;
            if (tmp_floor[this->ui->tableWidget->item(i, 0)->text()].isNull()) {
                QJsonObject tmp_spots;
                tmp_spots["spots"] = QJsonObject();
                tmp_floor_num = tmp_spots;
            } else {
                tmp_floor_num = tmp_floor[this->ui->tableWidget->item(i, 0)->text()].toObject();
            }
            QJsonObject tmp_spots = tmp_floor_num["spots"].toObject();

//            qDebug() << "tmp_floor_num" << tmp_floor_num;

            tmp_spots[this->ui->tableWidget->item(i, 1)->text()] = this->ui->tableWidget->item(i, 2)->text();

//            qDebug() << "tmp_spots" << tmp_spots;

            tmp_floor_num["spots"] = tmp_spots;

//            qDebug() << "new tmp_floor_num" << tmp_floor_num;

            tmp_floor[this->ui->tableWidget->item(i, 0)->text()] = tmp_floor_num;

//            qDebug() << "new tmp_floor" << tmp_floor;

            arguments["floor"] = tmp_floor;

//            qDebug() << "new arguments" << arguments;
        }
    }

//    qDebug() << "operation" << operation;


    operation["arguments"] = arguments;

//    qDebug() << "new operation" << operation;

    json_obj["auth"] = auth;
    json_obj["operation"] = operation;

//    qDebug() << "json_obj" << json_obj;

    QJsonDocument doc(json_obj);
    QString json = doc.toJson(QJsonDocument::Compact).toStdString().c_str();
    qDebug("update_spots json %s", json.toStdString().c_str());

    this->sock->connectToHost(this->server_ip, this->port.toUInt());
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
            this->exit();
        } else {
            qDebug() << "server failed";
            QMessageBox::critical(this, "error", state["reason"].toString(), QMessageBox::Ok);
        }
    } else {
        qDebug() << "connect failed";
        QMessageBox::critical(this, "error", "cannot connect to server", QMessageBox::Ok);
    }

}

void EditSpotsWindow::exit() {
    this->close();
    this->parentWidget()->show();
}
