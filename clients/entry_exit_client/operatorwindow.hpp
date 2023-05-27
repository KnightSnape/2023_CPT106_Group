#pragma once

#include <QMainWindow>
#include <QTcpSocket>
#include "displayboard.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class OperatorWindow; }
QT_END_NAMESPACE

class OperatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    OperatorWindow(QWidget *parent = nullptr);
    ~OperatorWindow();
    Ui::OperatorWindow *ui;
    QString account;
    QString password;
    QString server_ip;
    QString port;
    QString client_type;
    QString car_type;
    DisplayBoard *d;

public slots:
    void confirm();
    void select_car();
    void select_motorcycle();
    void select_van();
    void select_trunk();
    void update();

private:
    QTcpSocket *sock;
    QTimer *timer;
};
