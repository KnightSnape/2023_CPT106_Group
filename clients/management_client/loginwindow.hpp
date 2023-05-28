#pragma once

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    bool succ = false;
    QString account;
    QString password;
    QString server_ip;
    QString port;
    QString role;

public slots:
    void login();
    
private:
    Ui::LoginWindow *ui;
    QTcpSocket *sock;
};
