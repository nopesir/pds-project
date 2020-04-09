#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H


#include <QMainWindow>
#include <QListWidgetItem>
//#include "editorwindow.h"
#include "clientproc.h"
//#include "userprofile.h"

namespace Ui{class HomeWindow;}

class HomeWindow: public QMainWindow{
    Q_OBJECT

public:
    HomeWindow(ClientProc* client, QWidget *parent = nullptr);
    ~HomeWindow();

private slots:
    //void on_LogoutButton_clicked();
    //void on_Username_clicked();
    //void on_newDoc_clicked();
    //void on_uriDoc_clicked();
    //void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    //void on_listFiles_clicked();
    //void on_exitButton_clicked();
    //void on_backButton_clicked();
    //void closeEvent(QCloseEvent * event);
    //void RapidUserLogout();
    //void handleTheConnectionLoss();

    void on_newFile_clicked();

    void on_openFiles_clicked();

    void on_sharedFiles_clicked();

public slots:
    //void showPopupSuccess(QString result);
    //void showPopupFailure(QString result);
    //void showListFile(std::vector<File> files);
    //void resumeWindow();
    //void setUserProfileClosed();

private:
    Ui::HomeWindow *ui;
    QPoint oldPos; //This "oldPos" object is used to save the old position of the windows for the translation of windows
    ClientProc *_client;
    //EditorWindow *_ew;
    bool Logout = false;
    bool profile = false;
    bool FirstTimeWindowOpens=true;
    bool profile_closed = true;
    //UserProfile *up;

protected:
    //Function for mouse related event
    //void mousePressEvent(QMouseEvent *evt);
    //void mouseMoveEvent(QMouseEvent *evt);

};

#endif // HOMEWINDOW_H
