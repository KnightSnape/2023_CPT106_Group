#pragma once

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class DisplayBoard; }
QT_END_NAMESPACE

class DisplayBoard : public QMainWindow
{
    Q_OBJECT

public:
    DisplayBoard(QWidget *parent = nullptr);
    ~DisplayBoard();
    QString account;
    QString password;
    QString server_ip;
    QString port;
    QString floor;

public slots:
    void update();

private:
    Ui::DisplayBoard *ui;
    QTcpSocket *sock;
    QTimer *timer;
};
