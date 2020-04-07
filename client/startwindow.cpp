#include "startwindow.h"
#include "ui_startwindow.h"
//#include "menuwindow.h"
#include <QMessageBox>
#include <iostream>
#include <thread>
#include <QInputDialog>
//#include "userprofile.h"
//#include "jsonUtility.h"
//#include "editorwindow.h"
//#include "message.h"
#include <QDesktopWidget>

//using json = nlohmann::json;
//using boost::asio::ip::tcp;
//typedef std::deque<message> message_queue;

//CONSTRUCTOR
StartWindow::StartWindow(QWidget *parent): QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::StartWindow)
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    double width = screenGeometry.width();
    int minWidth = 1920;
    double scale = width / minWidth;
    std::string scaleAsString = std::to_string(scale);
    QByteArray scaleAsQByteArray(scaleAsString.c_str(), scaleAsString.length());
    qputenv("QT_SCALE_FACTOR", scaleAsQByteArray);

    ui->setupUi(this);  //IS AN HALF HELP WITH THE DPI-Related-BUG - DON'T DELETE ME FOR NOW
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
