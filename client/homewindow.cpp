#include "homewindow.h"
#include "ui_homewindow.h"
#include <boost/asio.hpp>
#include <QMessageBox>
#include <QInputDialog>
#include "editorwindow.h"
#include "startwindow.h"
#include <iostream>
#include <QListWidgetItem>

HomeWindow::HomeWindow(ClientProc* client, QWidget *parent) : QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint), ui(new Ui::HomeWindow()), _client(client) {
    ui->setupUi(this);
    connect(_client, &ClientProc::opResultSuccess, this, &HomeWindow::showPopupSuccess);
    connect(_client, &ClientProc::opResultFailure, this, &HomeWindow::showPopupFailure);
    connect(_client, &ClientProc::listFileResult, this, &HomeWindow::showListFile);

    this->show();
    ui->logoutButton->setToolTip("Esci dalla sessione");
    setFixedSize(size());   //IS AN HALF HELP WITH THE DPI-Related-BUG - DON'T DELETE ME FOR NOW
    qRegisterMetaType<std::vector<File>>("std::vector<File>");

    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setGridSize(QSize(100,100));
    ui->listWidget->setIconSize(QSize(60,60));
    ui->listWidget->setFlow(QListView::LeftToRight);
    ui->listWidget->setWrapping(true);
    ui->listWidget->setWordWrap(true);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setAlternatingRowColors(false);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setTextElideMode(Qt::ElideRight);

    on_listFiles_clicked(); //See note on showPopupSuccess() function.
}

//DESTRUCTOR
HomeWindow::~HomeWindow() {
    delete ui;
    delete _client;
    delete _ew;
}

void HomeWindow::mousePressEvent(QMouseEvent *evt){
    oldPos = evt->globalPos();
}

void HomeWindow::mouseMoveEvent(QMouseEvent *evt){
    const QPoint delta = evt->globalPos() - oldPos;
    move(x()+delta.x(), y()+delta.y());
    oldPos = evt->globalPos();
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

void HomeWindow:: setEditorClosed(){
    editor_closed = true;
    on_openFiles_clicked();
    this->show();
}

void HomeWindow::showPopupSuccess(QString result) {
    if(result == "DISCONNECT_SUCCESS") {
        StartWindow *s = new StartWindow();
        this->close();
        s->show();
    } else if(result == "LOGOUT_SUCCESS") {
        QApplication::exit();
    } else if(result == "NEWFILE_SUCCESS" || result == "OPENFILE_SUCCESS" || result == "OPENWITHURI_SUCCESS") {
        if(editor_closed){
            _ew = new EditorWindow(_client);
            connect(_ew, &EditorWindow::closeEditor, this, &HomeWindow::setEditorClosed);
            editor_closed = false;
            this->hide();
            _ew->showMaximized();
        }
    } else if(result == "LISTFILE_SUCCESS") {
        if(profile) {
            profile = false;
        } else {
            /*
             * Note: First I call on_listFiles_clicked() in the constructor. This is for getting the number of files from server.
             * Then I want to return to initial stackedWidget (index=0), if is the first time that I open this window.
             * In this way, when user clicks on his profile button, he can see how many files has.
             * On the second call of on_listFiles_clicked(), i want to go on the "list file page" (index=1).
            */
            if(FirstTimeWindowOpens==true){
                FirstTimeWindowOpens=false;
                ui->stackedWidget->setCurrentIndex(1);
            }else{
                ui->stackedWidget->setCurrentIndex(1);
            }
        }
    }
}

void HomeWindow::showPopupFailure(QString result) {
    if(result == "LOGOUT_FAILURE") {
        QMessageBox::critical(this,"Errore", "C'è stato un errore, impossibile effettuare il logout!");           //Stay in the same window
    } else if(result == "DISCONNECT_FAILURE") {
        QMessageBox::critical(this,"Errore", "C'è stato un errore, disconnessione non completata!");       //Stay in the same window
    } else if(result == "NEWFILE_FAILURE") {
        QMessageBox::critical(this,"Errore", "C'è stato un errore, impossibile creare il file!");          //Stay in the same window
    } else if(result == "OPENFILE_FAILURE") {
        QMessageBox::critical(this,"Errore", "C'è stato un errore, impossibile aprire il file!");         //Stay in the same window (MenuWindow(1))
    } else if(result == "OPENWITHURI_FAILURE") {
        QMessageBox::critical(this,"Errore", "C'è stato un errore, impossibile aprire il file!");      //Stay in the same window
    } else if(result == "LISTFILE_FAILURE") {
        QMessageBox::critical(this,"Errore", "C'è stato un errore, impossibile visualizzare la lista dei file!");         //Stay in the same window (MenuWindow(1))
    } else if(result == "LISTFILE_FAILURE_LISTNOTEXIST") {
        //QMessageBox::warning(this,"Attenzione", "Non hai ancora nessun un documento!");  //Stay in the same window (MenuWindow(1))

        /*
         * Note: If the user has no file, then first I call on_listFiles_clicked() in the constructor. This is for getting the number of files from server.
         * Then I want to return to initial stackedWidget (index=0), if is the first time that I open this window.
         * In this way, when user clicks on his profile button, he can see how many files has.
         * On the second call of on_listFiles_clicked(), i want to go on the "list file page" (index=1).
        */
        if(FirstTimeWindowOpens==true){
            FirstTimeWindowOpens=false;
            ui->stackedWidget->setCurrentIndex(1);
        }else{
            if(profile) {
                profile = false;
            } else {
                ui->stackedWidget->setCurrentIndex(1);
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

        ui->listWidget->clear();
        foreach (File f, files) {
            //in filename the conversion .toLatin1() because the conversion is already done by the server
            filename  = QString::fromUtf8(f.getfilename().c_str());
            owner     = QString::fromUtf8(f.getowner().c_str()).toLatin1();
            timestamp = QString::fromUtf8(f.gettimestamp().c_str()).toLatin1();
            QListWidgetItem* item;

            itemString = filename;
            if(user == owner){
                item = new QListWidgetItem(QIcon(":/images/doc.png"), itemString, ui->listWidget);
            }
            else{
                item = new QListWidgetItem(QIcon(":/images/doc-share.png"), itemString, ui->listWidget);

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
        if(ui->listWidget->count()==0){
            ui->noFile->show();
        }else{
            ui->noFile->hide();
        }
    }
}

void HomeWindow::resumeWindow() {
    on_openFiles_clicked();
    this->show();
    //ui->stackedWidget->setCurrentIndex(1);
}

void HomeWindow:: setprofileWindowClosed(){
    profile_closed = true;
}

void HomeWindow::handleTheConnectionLoss() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(nullptr, "Warning", "Cannot reach the server!\nDo you want to quit?",  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::exit(-1000);
    }
}

void HomeWindow::on_newFile_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->newFile->setStyleSheet("#newFile {color: rgb(0, 0, 0); background-color: rgb(211, 215, 207); border: transparent;}");
    ui->labelNew->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->openFiles->setStyleSheet("#openFiles {color: rgb(0, 0, 0); background-color: rgb(186, 189, 182);border: transparent;} #openFiles:hover {border: 2px solid rgb(164, 0, 0);}");
    ui->labelOpen->setStyleSheet("background-color: rgb(186, 189, 182);");
    ui->sharedFiles->setStyleSheet("#sharedFiles {color: rgb(0, 0, 0); background-color: rgb(186, 189, 182);border: transparent;} #sharedFiles:hover {border: 2px solid rgb(164, 0, 0);}");
    ui->labelShare->setStyleSheet("background-color: rgb(186, 189, 182);");

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
            on_newFile_clicked();
        }
        else if (ok && text.isEmpty()) {
            QMessageBox::critical(this,"Errore", "Inserire il nome del documento!");
            on_newFile_clicked();
        }
        else
            on_openFiles_clicked();
    }
}


void HomeWindow::on_openFiles_clicked()
{
    on_listFiles_clicked();
    ui->stackedWidget->setCurrentIndex(1);
    ui->newFile->setStyleSheet("#newFile {color: rgb(0, 0, 0); background-color: rgb(186, 189, 182); border: transparent;} #newFile:hover {border: 2px solid rgb(164, 0, 0);}");
    ui->labelNew->setStyleSheet("background-color: rgb(186, 189, 182);");
    ui->openFiles->setStyleSheet("#openFiles {color: rgb(0, 0, 0); background-color: rgb(211, 215, 207); border: transparent;}");
    ui->labelOpen->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->sharedFiles->setStyleSheet("#sharedFiles {color: rgb(0, 0, 0); background-color: rgb(186, 189, 182);border: transparent;} #sharedFiles:hover {border: 2px solid rgb(164, 0, 0);}");
    ui->labelShare->setStyleSheet("background-color: rgb(186, 189, 182);");
}

void HomeWindow::on_sharedFiles_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->newFile->setStyleSheet("#newFile {color: rgb(0, 0, 0); background-color: rgb(186, 189, 182); border: transparent;} #newFile:hover {border: 2px solid rgb(164, 0, 0);}");
    ui->labelNew->setStyleSheet("background-color: rgb(186, 189, 182);");
    ui->openFiles->setStyleSheet("#openFiles {color: rgb(0, 0, 0); background-color: rgb(186, 189, 182); border: transparent;} #openFiles:hover {border: 2px solid rgb(164, 0, 0);}");
    ui->labelOpen->setStyleSheet("background-color: rgb(186, 189, 182);");
    ui->sharedFiles->setStyleSheet("#sharedFiles {color: rgb(0, 0, 0); background-color: rgb(211, 215, 207);border: transparent;}");
    ui->labelShare->setStyleSheet("background-color: rgb(211, 215, 207);");

    if(_client->getStatus()==false){
        handleTheConnectionLoss();
    }else{
        bool ok;
        QString text = QInputDialog::getText(this, tr("URL Documento"),
                                             tr("Inserisci l'URL ricevuta via e-mail del documento:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty() && text.size()<=25){

            //Get data from the urltextedit
            QString uri = text;
            QString user = _client->getUsername();
            QByteArray ba_user = user.toLocal8Bit();
            const char *c_user = ba_user.data();
            qDebug() << "Opening file with URI: " << uri;
            QByteArray ba_uri = uri.toLocal8Bit();
            const char *c_uri = ba_uri.data();
            std::cout << "url messo è :" << c_uri << std::endl;
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
            QMessageBox::critical(this,"Errore", "Inserire un URL minore di 25 caratteri!");
            on_sharedFiles_clicked();
        }
        else if (ok && text.isEmpty()) {
            QMessageBox::critical(this,"Errore", "Inserire l'URL!");
            on_sharedFiles_clicked();
        }
        else
            on_openFiles_clicked();
    }
}

void HomeWindow::on_pushButton_clicked()
{
    StartWindow *s = new StartWindow();
    this->close();
    s->show();
}


void HomeWindow::on_logoutButton_clicked()
{
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

void HomeWindow::on_viewProfile_clicked()
{
    if(_client->getStatus()==false){
        handleTheConnectionLoss();
    }else{
        int Nfile=0;
        if(profile_closed){
            QList<QListWidgetItem*> fileItem;


            if(!_client->getVectorFile().empty()){
                std::vector<File> files = _client->getVectorFile();
                foreach (File f, files) {
                    if(QString::fromUtf8(f.getowner().c_str()) == _client->getUsername()){
                        Nfile++;
                    }
                }
            }else{
                Nfile=0;
            }
        }
        _pw = new profilewindow(_client, Nfile);
        _pw->setModal(true);
        connect(_pw, &profilewindow::closeUserProfile, this, &HomeWindow::setprofileWindowClosed);
        profile_closed = false;
    }
}
