#include "homewindow.h"
#include "ui_homewindow.h"
#include <boost/asio.hpp>
#include <QMessageBox>
#include <QInputDialog>
//#include "editorwindow.h"
#include "startwindow.h"
#include <iostream>
#include <QListWidgetItem>

HomeWindow::HomeWindow(ClientProc* client, QWidget *parent) : QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint), ui(new Ui::HomeWindow()), _client(client) {
    ui->setupUi(this);
    //connect(_client, &ClientProc::opResultSuccess, this, &HomeWindow::showPopupSuccess);
    //connect(_client, &ClientProc::opResultFailure, this, &HomeWindow::showPopupFailure);
    //connect(_client, &ClientProc::listFileResult, this, &HomeWindow::showListFile);
    //connect(_client, &ClientProc::backToHomeWindow,this, &HomeWindow::resumeWindow);

    this->show();
    setFixedSize(size());   //IS AN HALF HELP WITH THE DPI-Related-BUG - DON'T DELETE ME FOR NOW
    qRegisterMetaType<std::vector<File>>("std::vector<File>");

    //ui->listWidget->setViewMode(QListView::IconMode);
    //ui->listWidget->setGridSize(QSize(100,100));
    //ui->listWidget->setIconSize(QSize(60,60));
    //ui->listWidget->setFlow(QListView::LeftToRight);
    /*ui->listWidget->setWrapping(true);
    ui->listWidget->setWordWrap(true);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setAlternatingRowColors(false);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setTextElideMode(Qt::ElideRight);
    */
    //on_listFiles_clicked(); //See note on showPopupSuccess() function.
}

//DESTRUCTOR
HomeWindow::~HomeWindow() {
    delete ui;
    delete _client;
    //delete _ew;
}

//CHECK HOW THIS WINDOW IS CLOSED
/*void HomeWindow::closeEvent(QCloseEvent * event) {
    if(Logout==false){
        //If isn't a simple logout, but a forced close then disconnect the user
        RapidUserLogout();
        qDebug()<<"FORCED CLOSE - USER " << _client->getUsername() <<"DISCONNECT";
    }
}

//SEE UP
void HomeWindow::RapidUserLogout() {
    //Get data from the form
    QString user = _client->getUsername();
    QByteArray ba_user = user.toLocal8Bit();
    const char *c_user = ba_user.data();

    //Serialize data
    json j;
    Jsonize::to_jsonUser(j, "LOGOUT_REQUEST", c_user);
    const std::string req = j.dump();

    _client->sendRequestMsg(req);    //Send data (header and body)
}

//LOGOUT BUTTON
void HomeWindow::on_LogoutButton_clicked(){

    Logout=true;    //set simple logout true

    if(_client->getStatus()==false){
        handleTheConnectionLoss();
    }else{
        //Get data from the form
        QString user = _client->getUsername();
        QByteArray ba_user = user.toLocal8Bit();
        const char *c_user = ba_user.data();

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Uscita", "Vuoi disconnetterti?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            //Serialize data
            json j;
            Jsonize::to_jsonUser(j, "DISCONNECT_REQUEST", c_user);
            const std::string req = j.dump();

            //Send data (header and body)
            _client->sendRequestMsg(req);
        }
    }
}

void HomeWindow::mousePressEvent(QMouseEvent *evt){
    oldPos = evt->globalPos();
}

void HomeWindow::mouseMoveEvent(QMouseEvent *evt){
    const QPoint delta = evt->globalPos() - oldPos;
    move(x()+delta.x(), y()+delta.y());
    oldPos = evt->globalPos();
}

//USERNAME BUTTON
void HomeWindow::on_Username_clicked(){
    if(_client->getStatus()==false){
        handleTheConnectionLoss();
    }else{

        if(profile_closed){//you can access to the stats, else you must close the current UserProfile Window

            profile = true;
            on_listFiles_clicked();
            on_backButton_clicked();
            QString filename, owner, timestamp;
            QList<QListWidgetItem*> fileItem;
            int Contafile=0;
            int ContaFileOwner=0;

            if(!_client->getVectorFile().empty()){
                std::vector<File> files = _client->getVectorFile();
                foreach (File f, files) {
                    filename  = QString::fromUtf8(f.getfilename().c_str());
                    owner     = QString::fromUtf8(f.getowner().c_str());
                    timestamp = QString::fromUtf8(f.gettimestamp().c_str());
                    Contafile++;
                    if(owner==_client->getUsername()){
                        ContaFileOwner++;
                    }
                }
            }else{
                Contafile=0;
                ContaFileOwner=0;
            }

            //up = new UserProfile(_client, _client->getUsername(), _client->getMail(), Contafile, ContaFileOwner); //with parameters
            //connect(up, &UserProfile::closeUserProfile, this, &HomeWindow::setUserProfileClosed);
            profile_closed = false;
            //up->show(); Not necessary is done by the costructor
        }
    }
}

//EXIT BUTTON
void HomeWindow::on_exitButton_clicked() {

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Uscita", "Sei sicuro di voler uscire?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if(_client->getStatus()==false){
            handleTheConnectionLoss();
        }else{
            //Get data from the form
            QString user = _client->getUsername();
            QByteArray ba_user = user.toLocal8Bit();
            const char *c_user = ba_user.data();

            //Serialize data
            json j;
            Jsonize::to_jsonUser(j, "LOGOUT_REQUEST", c_user);
            const std::string req = j.dump();

            //Send data (header and body)
            _client->sendRequestMsg(req);
        }
    }
}

//BACK BUTTON
void HomeWindow::on_backButton_clicked() {
    //ui->stackedWidget->setCurrentIndex(0);
}

//NEW DOCUMENT
void HomeWindow::on_newDoc_clicked(){
    //close userProfile Window if it was opened;
    if(!profile_closed){
        //delete up;
    }
    if(_client->getStatus()==false){
        handleTheConnectionLoss();
    }else{
        bool ok;
        QString text = QInputDialog::getText(this, tr("Titolo documento"),
                                             tr("Inserisci un nome per il nuovo documento:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty() && text.size()<=25){

            //Get data from the form
            QString user = _client->getUsername();
            QByteArray ba_user = user.toLocal8Bit();
            const char *c_user = ba_user.data();

            QString filename = QLatin1String(text.toUtf8());
            QByteArray ba_filename = filename.toLocal8Bit();
            const char *c_filename = ba_filename.data();

            //Serialize data
            json j;
            Jsonize::to_jsonFilename(j, "NEWFILE_REQUEST", c_user, c_filename);
            const std::string req = j.dump();

            //update client data
            _client->setUsername(user);
            _client->setFilename(filename);

            //Send data (header and body)
            _client->sendRequestMsg(req);
        }
        else if (ok && !text.isEmpty() && text.size()>25) {
            QMessageBox::critical(this,"Errore", "Inserire un nome minore di 25 caratteri!");
            on_newDoc_clicked();
        }
        else if (ok && text.isEmpty()) {
            QMessageBox::critical(this,"Errore", "Inserire il nome del documento!");
            on_newDoc_clicked();
        }
    }
}

void HomeWindow::on_listFiles_clicked() {
    //close userProfile Window if it was opened;
    if(!profile_closed){
        //delete up;
    }
    if(_client->getStatus()==false){
        handleTheConnectionLoss();
    }else{
        //Get data from the form
        QString user = _client->getUsername();
        QByteArray ba_user = user.toLocal8Bit();
        const char *c_user = ba_user.data();

        //Serialize data
        json j;
        Jsonize::to_jsonUser(j, "LISTFILE_REQUEST", c_user);
        const std::string req = j.dump();

        //update client data
        _client->setUsername(user);

        //Send data (header and body)
        _client->sendRequestMsg(req);

    }
}

void HomeWindow::on_uriDoc_clicked() {
    //close userProfile Window if it was opened;
    if(!profile_closed){
        //delete up;
    }
    if(_client->getStatus()==false) {
        handleTheConnectionLoss();
    } else {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Titolo documento"),
                                             tr("Inserisci URI del documento:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty() && text.size()<=25) { //TODO: better controls
            //TODO controllo file database (nome e utente)

            //Get data from the form
            QString user = _client->getUsername();
            QByteArray ba_user = user.toLocal8Bit();
            const char *c_user = ba_user.data();
            QString uri = text;
            QByteArray ba_uri = uri.toLocal8Bit();
            const char *c_uri = ba_uri.data();

            //Serialize data
            json j;
            Jsonize::to_jsonUri(j, "OPENWITHURI_REQUEST", c_user, c_uri);
            const std::string req = j.dump();

            //update client data
            _client->setUsername(user);
            _client->setFileURI(uri);

            //Send data (header and body)
            _client->sendRequestMsg(req);
        }
        else if (ok && !text.isEmpty() && text.size()>25) {
            QMessageBox::critical(this,"Errore", "Inserire un nome minore di 25 caratteri!!");
            on_uriDoc_clicked();
        }
        else if (ok && text.isEmpty()){
            QMessageBox::critical(this,"Errore", "Inserire il nome del documento!");
            on_uriDoc_clicked();
        }
    }
}

//OPEN ONE DOCUMENT FROM A LIST OF USER'S DOC
void HomeWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
    //close userProfile Window if it was opened;
    if(!profile_closed){
        //delete up;
    }
    auto t_start1 = std::chrono::high_resolution_clock::now();
    if(_client->getStatus()==false) {
        handleTheConnectionLoss();
    } else {
        //Get data from the form
        QString user = _client->getUsername();
        QByteArray ba_user = user.toLocal8Bit();
        const char *c_user = ba_user.data();
        std::vector<QString> uriAndFilename = item->data(Qt::UserRole).value<std::vector<QString>>();
        QString uri = uriAndFilename.at(0);
        QString filename = uriAndFilename.at(1);
        qDebug() << "Opening file: "<<filename<< " - with URI: " << uri;
        filename = QLatin1String(filename.toUtf8());
        QByteArray ba_uri = uri.toLocal8Bit();
        const char *c_uri = ba_uri.data();

        //Serialize data
        json j;
        Jsonize::to_jsonUri(j, "OPENFILE_REQUEST", c_user, c_uri);
        const std::string req = j.dump();

        //update client data
        _client->setUsername(user);
        _client->setFileURI(uri);
        _client->setFilename(filename);

        //Send data (header and body)
        _client->sendRequestMsg(req);
    }
    auto t_end1 = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms1 = std::chrono::duration<double, std::milli>(t_end1-t_start1).count();
    std::cout << "ITEM DOUBLE CLICKED - ELAPSED (ms): " << elapsed_time_ms1 << std::endl;
}

void HomeWindow::showPopupSuccess(QString result) {
    if(result == "DISCONNECT_SUCCESS") {
        StartWindow *s = new StartWindow();
        this->close();
        s->show();
    } else if(result == "LOGOUT_SUCCESS") {
        QApplication::exit();
    } else if(result == "NEWFILE_SUCCESS") {
        //_ew = new EditorWindow(_client);
        this->hide();
        //_ew->showMaximized();
    } else if(result == "OPENFILE_SUCCESS") {
        //_ew = new EditorWindow(_client);
        this->hide();
        //_ew->showMaximized();
    } else if(result == "OPENWITHURI_SUCCESS") {
        //_ew = new EditorWindow(_client);
        this->hide();
        //_ew->showMaximized();
    } else if(result == "LISTFILE_SUCCESS") {
        if(profile) {
            profile = false;
        } else {
            /*
             * Note: First I call on_listFiles_clicked() in the constructor. This is for getting the number of files from server.
             * Then I want to return to initial stackedWidget (index=0), if is the first time that I open this window.
             * In this way, when user clicks on his profile button, he can see how many files has.
             * On the second call of on_listFiles_clicked(), i want to go on the "list file page" (index=1).

            if(FirstTimeWindowOpens==true){
                FirstTimeWindowOpens=false;
                //ui->stackedWidget->setCurrentIndex(0);
            }else{
                //ui->stackedWidget->setCurrentIndex(1);
            }
        }
    }
}

void HomeWindow::showPopupFailure(QString result) {
    if(result == "LOGOUT_FAILURE") {
        QMessageBox::critical(this,"Errore", "Logout non completato!");           //Stay in the same window
    } else if(result == "DISCONNECT_FAILURE") {
        QMessageBox::critical(this,"Errore", "Disconnect non completato!");       //Stay in the same window
    } else if(result == "NEWFILE_FAILURE") {
        QMessageBox::critical(this,"Errore", "Newfile non completata!");          //Stay in the same window
    } else if(result == "OPENFILE_FAILURE") {
        QMessageBox::critical(this,"Errore", "Openfile non completata!");         //Stay in the same window (HomeWindow(1))
    } else if(result == "OPENWITHURI_FAILURE") {
        QMessageBox::critical(this,"Errore", "Openwithuri non completata!");      //Stay in the same window
    } else if(result == "LISTFILE_FAILURE") {
        QMessageBox::critical(this,"Errore", "Listfile non completata!");         //Stay in the same window (HomeWindow(1))
    } else if(result == "LISTFILE_FAILURE_LISTNOTEXIST") {
        //QMessageBox::warning(this,"Attenzione", "Non hai ancora nessun un documento!");  //Stay in the same window (HomeWindow(1))

        /*
         * Note: If the user has no file, then first I call on_listFiles_clicked() in the constructor. This is for getting the number of files from server.
         * Then I want to return to initial stackedWidget (index=0), if is the first time that I open this window.
         * In this way, when user clicks on his profile button, he can see how many files has.
         * On the second call of on_listFiles_clicked(), i want to go on the "list file page" (index=1).

        if(FirstTimeWindowOpens==true){
            FirstTimeWindowOpens=false;
            //ui->stackedWidget->setCurrentIndex(0);
        }else{
            if(profile) {
                profile = false;
            } else {
                //ui->stackedWidget->setCurrentIndex(1);
            }
        }
    } else if(result == "RESPONSE_FAILURE") {
        QMessageBox::critical(this,"Errore", "Risposta non gestita!\nErrore di tipo RESPONSE_FAILURE");
    } else {
        QMessageBox::information(nullptr, "Attenzione", "Qualcosa è andato storto! Riprova!");
    }
}

void HomeWindow::showListFile(std::vector<File> files) {
    if(_client->getStatus()==false) {
        handleTheConnectionLoss();
    } else {
        QString user = _client->getUsername();
        QString filename, owner, timestamp;
        //int littlechar=0;
        QString itemString;
        QList<QListWidgetItem*> fileItem;

        //ui->listWidget->clear();
        foreach (File f, files) {
            //in filename the conversion .toLatin1() because the conversion is already done by the server
            filename  = QString::fromUtf8(f.getfilename().c_str());
            owner     = QString::fromUtf8(f.getowner().c_str()).toLatin1();
            timestamp = QString::fromUtf8(f.gettimestamp().c_str()).toLatin1();
            QListWidgetItem* item;

            itemString = filename;
            if(user == owner){
                //item = new QListWidgetItem(QIcon(":/image/document-own.png"), itemString, ui->listWidget);
            }
            else{
                //item = new QListWidgetItem(QIcon(":/image/document-col.png"), itemString, ui->listWidget);

            }
            std::vector<QString> uriAndFilename;
            uriAndFilename.push_back(QString::fromStdString(f.getidfile()));
            uriAndFilename.push_back(filename);
            QVariant var;
            var.setValue(uriAndFilename);
            item->setData(Qt::UserRole, var);
            _client->setVectorFile(files);
            fileItem.append(item);

            item->setToolTip("Nome: "+filename+"\nAutore: "+owner+"\nCreato il: "+timestamp);
        }
        //if(ui->listWidget->count()==0){
        //    ui->noFile->show();
        //}else{
        //    ui->noFile->hide();
        //}
    }
}

void HomeWindow::resumeWindow() {
    this->show();
    //ui->stackedWidget->setCurrentIndex(0);
}

void HomeWindow:: setUserProfileClosed(){
    profile_closed = true;
}

void HomeWindow::handleTheConnectionLoss() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(nullptr, "Warning", "Cannot reach the server!\nDo you want to quit?",  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::exit(-1000);
    }
}*/
