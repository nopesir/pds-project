#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H


#include <QMainWindow>
#include <QListWidgetItem>
#include "editorwindow.h"
#include "clientproc.h"
#include "profilewindow.h"

namespace Ui{class HomeWindow;}

class HomeWindow: public QMainWindow{
    Q_OBJECT

public:
    HomeWindow(ClientProc* client, QWidget *parent = nullptr);
    ~HomeWindow();

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listFiles_clicked();
    void handleTheConnectionLoss();
    void on_newFile_clicked();
    void on_openFiles_clicked();
    void on_sharedFiles_clicked();
    void on_pushButton_clicked();
    void on_logoutButton_clicked();
    void on_viewProfile_clicked();

public slots:
    void showPopupSuccess(QString result);
    void showPopupFailure(QString result);
    void showListFile(std::vector<File> files);
    void resumeWindow();
    void setEditorClosed();
    void setprofileWindowClosed();

private:
    Ui::HomeWindow *ui;
    QPoint oldPos; //This "oldPos" object is used to save the old position of the windows for the translation of windows
    ClientProc *_client;
    EditorWindow *_ew;
    profilewindow *_pw;
    bool Logout = false;
    bool profile = false;
    bool FirstTimeWindowOpens=true;
    bool profile_closed = true;
    bool editor_closed = true;
    //UserProfile *up;

protected:
    //Function for mouse related event
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);

};

#endif // HOMEWINDOW_H
