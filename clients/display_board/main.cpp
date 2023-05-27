#include "login_window.hpp"
#include "displayboard.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow w;
    w.show();
    a.exec();
    if (w.succ) {
        DisplayBoard d;
        d.server_ip = w.server_ip;
        d.port = w.port;
        d.account = w.account;
        d.password = w.password;
        d.floor = w.floor;
        qDebug() << d.floor << w.floor;
        d.show();
        a.exec();
    }
    return 0;
}
