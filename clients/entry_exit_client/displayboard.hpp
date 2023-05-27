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
    Ui::DisplayBoard *ui;
    void update(QString str);

private:
};
