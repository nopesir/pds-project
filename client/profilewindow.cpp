#include "profilewindow.h"
#include "ui_profilewindow.h"
#include "clientproc.h"

profilewindow::profilewindow(ClientProc* client, int files, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::profilewindow),
    _client(client)
{
    ui->setupUi(this);
    ui->userName->setText(_client->getUsername());
    ui->userEmail->setText(_client->getMail());
    ui->userNDoc->setText(QString::number(files));
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowTitleHint);
    this->show();
}

profilewindow::~profilewindow()
{
    emit closeUserProfile();
    delete ui;
}

void profilewindow::mousePressEvent(QMouseEvent *evt) {
     oldPos = evt->globalPos();
}


void profilewindow::mouseMoveEvent(QMouseEvent *evt) {
    const QPoint delta = evt->globalPos() - oldPos;
    move(x()+delta.x(), y()+delta.y());
    oldPos = evt->globalPos();
}


void profilewindow::on_closeButton_clicked()
{
    QWidget::close();
    delete this;
}
