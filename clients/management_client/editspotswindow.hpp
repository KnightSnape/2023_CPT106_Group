#pragma once

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class EWindow; }
QT_END_NAMESPACE

class EditSpotsWindow : public QMainWindow
{
    Q_OBJECT

public:
    EditSpotsWindow(QWidget *parent = nullptr);
    ~EditSpotsWindow();
    bool update_data();
    QString account;
    QString password;
    QString server_ip;
    QString port;

public slots:
    void save();
    void exit();
    
private:
    Ui::EWindow *ui;
    QTcpSocket *sock;
};
