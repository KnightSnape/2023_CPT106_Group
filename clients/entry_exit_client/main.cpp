#include "loginwindow.hpp"
#include "displayboard.hpp"
#include "operatorwindow.hpp"
#include "ui_displayboard.h"
#include "ui_operatorwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow w;
    w.show();
    a.exec();
    if (w.succ) {
        DisplayBoard d;
        OperatorWindow o;

        o.server_ip = w.server_ip;
        o.port = w.port;
        o.account = w.account;
        o.password = w.password;
        o.d = &d;

        if (w.client_type == "entry") {
            o.ui->label_banner->setText("ENTRY");
            d.ui->label_parking_lot->setText("PARKING LOT ENTRY");
            o.client_type = "entry";
        } else if(w.client_type == "exit") {
            o.ui->label_banner->setText("EXIT");
            o.client_type = "exit";
            o.ui->radioButton_car->hide();
            o.ui->radioButton_motorcycle->hide();
            o.ui->radioButton_trunk->hide();
            o.ui->radioButton_van->hide();
            o.ui->label_license->setGeometry(QRect(20, 120, 101, 21));
            o.ui->lineEdit_account->setGeometry(QRect(130, 120, 171, 25));
            d.ui->label_parking_lot->setText("PARKING LOT EXIT");
        }
        o.show();
        d.show();
        a.exec();
    }

    return 0;
}
