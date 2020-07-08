#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QDialog>
#include <QtGui>
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

private:
    Ui::profilewindow *ui;
    QPoint oldPos;
    ClientProc *_client;

protected:
    //Function for mouse related event
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
};


#endif // PROFILEWINDOW_H
