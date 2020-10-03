#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QDialog>
#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include "clientproc.h"

namespace Ui {
class profilewindow;
}

class profilewindow : public QDialog
{
    Q_OBJECT

public:
    explicit profilewindow(ClientProc* client, int files, QWidget *parent = nullptr);
    ~profilewindow();

signals:
    void closeUserProfile();

private slots:
    void on_closeButton_clicked();
    void replyFinished(QNetworkReply *reply);

private:
    void getImage(QString url);
    Ui::profilewindow *ui;
    QPoint oldPos;
    ClientProc *_client;

protected:
    //Function for mouse related event
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
};


#endif // PROFILEWINDOW_H
