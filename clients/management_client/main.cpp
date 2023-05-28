#include "loginwindow.hpp"
#include "managerwindow.hpp"
#include "./ui_managerwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow w;
    w.show();
    a.exec();
    if (w.succ) {
        if (w.role == "manager") {
            ManagerWindow m;
            m.server_ip = w.server_ip;
            m.port = w.port;
            m.account = w.account;
            m.password = w.password;
            m.show();
            a.exec();
        } else if (w.role == "customer") {
            ManagerWindow m;
            m.server_ip = w.server_ip;
            m.port = w.port;
            m.account = w.account;
            m.password = w.password;
            m.ui->pushButton_edit_spots->hide();
            m.ui->pushButton_add_customer->hide();
            m.ui->pushButton_query_available_spots->setGeometry(QRect(10, 120, 141, 61));
            m.ui->pushButton_find_my_car->setGeometry(QRect(170, 120, 141, 61));
            m.ui->label_banner->setText("Customer Panel");
            m.show();
            a.exec();
        }
    }
    return 0;
}
