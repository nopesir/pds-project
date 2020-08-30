#include "startwindow.h"
#include "ui_startwindow.h"
#include "homewindow.h"
#include <QMessageBox>
#include <iostream>
#include <thread>
#include <QInputDialog>
//#include "userprofile.h"
#include "jsonize.h"
//#include "editorwindow.h"
#include "message.h"
#include <QDesktopWidget>
#include <QProgressBar>

using json = nlohmann::json;
using boost::asio::ip::tcp;
typedef std::deque<Message> message_queue;

//CONSTRUCTOR
StartWindow::StartWindow(QWidget *parent): QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::StartWindow), hidePass(true), _client(new ClientProc())
{
    ui->setupUi(this);
    //ui->version->setText(qstr);
    ui->loginPage->setFocus();
    ui->labelError->hide();

    setStatus(_client->getStatus());


    connect(_client, &ClientProc::statusChanged, this, &StartWindow::setStatus);
    connect(_client, &ClientProc::formResultSuccess, this, &StartWindow::showPopupSuccess);
    connect(_client, &ClientProc::formResultFailure, this, &StartWindow::showPopupFailure);
    connect(_client, &ClientProc::jsonMsgFailure, this, &StartWindow::showJsonPopupFailure);

    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = desktop->width(); // get width of screen
    screenHeight = desktop->height(); // get height of screen

    windowSize = size(); // size of our application window
    width = windowSize.width();
    height = windowSize.height();

    // little computations
    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 50;

    // move window to desired coordinates
    move ( x, y );

}

void StartWindow::mousePressEvent(QMouseEvent *evt) {
     oldPos = evt->globalPos();
}


void StartWindow::mouseMoveEvent(QMouseEvent *evt) {
    const QPoint delta = evt->globalPos() - oldPos;
    move(x()+delta.x(), y()+delta.y());
    oldPos = evt->globalPos();
}

void StartWindow::setStatus(bool newStatus) {
    if(newStatus)
        ui->labelStatus->setText(tr("<font color=\"black\">CONNECTED</font>"));
        //ui->label_status->setText(tr("<font color=\"green\">CONNECTED</font>"));
    else
        ui->labelStatus->setText(tr("<font color=\"black\">DISCONNECTED</font>"));
        //ui->statusBar->statusLabel->setText(tr("<font color=\"red\">DISCONNECTED</font>"));
}

void StartWindow::showPopupSuccess(QString result) {
    if(result == "LOGIN_SUCCESS") {
        HomeWindow *m = new HomeWindow(_client);
        this->close(); //this startWindow will be then created (new) when user press Logout button on HomeWindow
        m->show();
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
        _client ->do_connect();
        #ifdef _WIN32
            Sleep(1000);
        #else
            sleep(1);
        #endif

        qDebug () << "IL SERVER é connesso?--> " <<_client->getStatus();
        if(_client->getStatus()==false){
            //secondo controllo se non sono riuscito a ricollegarmi al server
            QMessageBox::warning(nullptr, "Attenzione", "Non sono riuscito a contattare il server!\n" "Riprova più tardi");
        }else{
            LoginProcedure();
        }
    } else {
        LoginProcedure();
    }
}

void StartWindow::LoginProcedure(){
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

void StartWindow::on_buttonReg_clicked()
{
    if(_client->getStatus()==false) {
        QMessageBox::warning(nullptr, "Attenzione", "Non sono riuscito a contattare il server!\n"
                                                    "Riprova più tardi");
    } else {
        if (ui->lineUserForm->text().isEmpty()) {
            ui->labelError->show();
        } else {
            ui->labelError->hide();

            if (ui->linePasswordForm->text().length() < 6) {
                ui->labelError->show();
            } else {
                ui->labelError->hide();

                QRegularExpression mailREX("^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$");
                regMat = mailREX.match(ui->lineMailForm->text()).hasMatch();

                if (!regMat) {
                    ui->labelError->show();
                } else {
                    ui->labelError->hide();

                    //Get data from the form
                    QString user = ui->lineUserForm->text();
                    QByteArray ba_user = user.toLocal8Bit();
                    const char *c_user = ba_user.data();
                    QString pass = ui->linePasswordForm->text();
                    QByteArray ba_pass = pass.toLocal8Bit();
                    const char *c_pass = ba_pass.data();
                    QString email = ui->lineMailForm->text();
                    QByteArray ba_email = email.toLocal8Bit();
                    const char *c_email = ba_email.data();

                    //Serialize data
                    json j;
                    Jsonize::to_json(j, "SIGNUP_REQUEST", c_user, c_pass, c_email);
                    const std::string req = j.dump();

                    //Send data (header and body)
                    _client->sendRequestMsg(req);
                }
            }
        }
    }

}

void StartWindow::on_hideButton_clicked()
{
    if(hidePass){
        ui->linePassword->setEchoMode(QLineEdit::Normal);
        ui->hideButton->setStyleSheet("background-color: rgb(255, 255, 255); border-image: url(:/images/show.png);");
        hidePass=false;
    }
    else {
        ui->linePassword->setEchoMode(QLineEdit::Password);
        ui->hideButton->setStyleSheet("background-color: rgb(255, 255, 255); border-image: url(:images/hide.png);");
        hidePass=true;
    }
}

void StartWindow::on_linePassword_returnPressed()
{
    on_loginButton_clicked();
}

void StartWindow::on_linePasswordForm_returnPressed()
{
    on_buttonReg_clicked();
}
