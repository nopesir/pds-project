#include "startwindow.h"
#include "ui_startwindow.h"
//#include "menuwindow.h"
#include <QMessageBox>
#include <iostream>
#include <thread>
#include <QInputDialog>
//#include "userprofile.h"
#include "jsonize.h"
//#include "editorwindow.h"
#include "message.h"
#include <QDesktopWidget>

//using json = nlohmann::json;
//using boost::asio::ip::tcp;
//typedef std::deque<message> message_queue;

//CONSTRUCTOR
StartWindow::StartWindow(QWidget *parent): QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::StartWindow), _client(new ClientProc())
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    double width = screenGeometry.width();
    int minWidth = 1920;
    double scale = width / minWidth;
    std::string scaleAsString = std::to_string(scale);
    QByteArray scaleAsQByteArray(scaleAsString.c_str(), scaleAsString.length());
    qputenv("QT_SCALE_FACTOR", scaleAsQByteArray);

    ui->setupUi(this);
    //ui->version->setText(qstr);
    ui->loginPage->setFocus();
    ui->labelError->hide();

    setStatus(_client->getStatus());
    connect(_client, &ClientProc::statusChanged, this, &StartWindow::setStatus);
    connect(_client, &ClientProc::formResultSuccess, this, &StartWindow::showPopupSuccess);
    connect(_client, &ClientProc::formResultFailure, this, &StartWindow::showPopupFailure);
    connect(_client, &ClientProc::jsonMsgFailure, this, &StartWindow::showJsonPopupFailure);
    setFixedSize(size());   //IS AN HALF HELP WITH THE DPI-Related-BUG - DON'T DELETE ME FOR NOW
}


void StartWindow::setStatus(bool newStatus) {
    //if(newStatus)
        //ui->label_status->setText(tr("<font color=\"green\">CONNECTED</font>"));
    //else
        //ui->label_status->setText(tr("<font color=\"red\">DISCONNECTED</font>"));
}

void StartWindow::showPopupSuccess(QString result) {
    if(result == "LOGIN_SUCCESS") {
        //MenuWindow *m = new MenuWindow(_client);
        //this->close(); //this startWindow will be then created (new) when user press Logout button on menuWindow
        //m->show();
    } else if(result == "SIGNUP_SUCCESS") {
        QMessageBox::information(this,"Complimenti", "La registrazione è avvenuta correttamente!");
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void StartWindow::showPopupFailure(QString result) {
    if(result == "LOGIN_FAILURE") {
       QMessageBox::critical(this,"Errore", "Il login non è stato completato correttamente! Riprova!");                                 //Stay in the same window
    } else if(result == "SIGNUP_FAILURE") {
        QMessageBox::critical(this,"Errore", "La registrazione non è avvenuta correttamente! Riprova!");                                //Stay in the same window
    } else {
        QMessageBox::information(nullptr, "Attenzione", "Qualcosa è andato storto! Riprova!");
    }
}

void StartWindow::showJsonPopupFailure(QString windowName,QString msg) {
    if(windowName == "StartWindow") {
        QMessageBox::critical(this, "Errore", msg);
        QApplication::exit();
    }
}


//DESTRUCTOR
StartWindow::~StartWindow() {
    delete ui;
}


void StartWindow::on_regButton_clicked()
{
   ui->stackedWidget->setCurrentIndex(1);
}

void StartWindow::on_buttonBack_clicked()
{
   ui->stackedWidget->setCurrentIndex(0);
}

void StartWindow::on_exitButton_clicked()
{
    QApplication::exit();
}

void StartWindow::on_loginButton_clicked()
{
    if(_client->getStatus()==false){
        QMessageBox::warning(nullptr, "Attenzione", "Non sono riuscito a contattare il server!\n"
                                                        "Riprova più tardi");
    } else {
        //Get data from the form
        QString user = ui->lineUser->text();
        QByteArray ba_user = user.toLocal8Bit();
        const char *c_user = ba_user.data();
        QString pass = ui->linePassword->text();
        QByteArray ba_pass = pass.toLocal8Bit();
        const char *c_pass = ba_pass.data();

        //update client data
        _client->setUsername(user);

        //Serialize data
        json j;
        Jsonize::to_json(j, "LOGIN_REQUEST", c_user, c_pass);
        const std::string req = j.dump();

        //Send data (header and body)
        _client->sendRequestMsg(req);
    }
}
