#include "displayboard.hpp"
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "./ui_displayboard.h"

DisplayBoard::DisplayBoard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DisplayBoard)
{
    this->ui->setupUi(this);

    //this->connect(this->timer, SIGNAL(timeout()), this, SLOT(update()));
}

DisplayBoard::~DisplayBoard()
{
    delete ui;
}

void DisplayBoard::update(QString str) {
    this->ui->label_text->setText(str);
}
