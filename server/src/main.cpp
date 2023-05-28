#include <QCoreApplication>
#include"tcp_server.h"

int main(int argc,char **argv)
{
    QCoreApplication a(argc, argv);
    CPT106_Group::Server server;
    server.new_connection();
    return a.exec();
}