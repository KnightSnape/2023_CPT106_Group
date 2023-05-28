#pragma once

#include "editspotswindow.hpp"
#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ManagerWindow; }
QT_END_NAMESPACE

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ManagerWindow(QWidget *parent = nullptr);
    ~ManagerWindow();
    bool succ = false;
    Ui::ManagerWindow *ui;
    EditSpotsWindow *sub_ui;
    QString account;
    QString password;
    QString server_ip;
    QString port;
    QString role;

public slots:
    void query_available_spots();
    void find_my_car();
    void edit_spots();
    void add_customer();

private:
    QTcpSocket *sock;
};
