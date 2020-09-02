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

    if(QFile::exists(_client->getUsername().left(2) + ".png")) {
        ui->userImage->setFlat(true);
        ui->userImage->setAutoFillBackground(true);
        ui->userImage->setStyleSheet("#userImage{ background-repeat: no-repeat;background-position: center;border-image: url(" + _client->getUsername().left(2) + ".png" + "); }");
    }
    else
        getImage("https://eu.ui-avatars.com/api/?size=128&rounded=true&background=babdb6&color=e3e8df&name=" + _client->getUsername());

    this->show();
}

void profilewindow::getImage(QString url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));

    QUrl urll = QUrl(url);

    QNetworkRequest request(urll);

    manager->get(request);
}

void profilewindow::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        // do nothing
    }
    else
    {
        QByteArray responseData = reply->readAll();
        QFile file(_client->getUsername().left(2) + ".png");
        file.open(QIODevice::WriteOnly);
        file.write((responseData));
        file.close();
        ui->userImage->setFlat(true);
        ui->userImage->setAutoFillBackground(true);
        ui->userImage->setStyleSheet("#userImage{ background-repeat: no-repeat;background-position: center;border-image: url(" + _client->getUsername().left(2) + ".png" + "); }");
    }
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
