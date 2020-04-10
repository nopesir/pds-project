/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#include "texteditor.h"
#include "startwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{


    qRegisterMetaType<std::pair<int,wchar_t>>("std::pair<int,wchar_t>");
    qRegisterMetaType<std::string>("std::string");
    //qRegisterMetaType<symbolStyle>("symbolStyle");

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    /*
     * FOLLOWING COMMENTED LINE IS FOR DPI SCALING -> IT DOESN'T WORK FOR ME. Check yourself with your monitor
     * I tryed to copy this line in the constructor of EditorWindow, it doesn't work too!
     * This is big smoking shit! Can't find a solution.
     * @rinaldoclemente suggest to use image instead of text, but I (HidroSaphire) don't like it as solution
     *
     * Futhermore, I discover that is a Well Known Bug of Qt
     * See this and https://bugreports.qt.io/browse/QTBUG-53022
     *
    QApplication::setAttribute(Qt::AA_Use96Dpi);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("1")); //Deprecated in Qt 5.6
    qputenv("QT_SCALE_FACTORS", "2");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTORS", "2");
    */

    //The Following 4 lines is crucial and NECESSARY for class "settings". Is the path for Register in Windows (or .ini file in \AppData\Roaming)
    QCoreApplication::setOrganizationName("C.A.R.T.E. Studio");
    QCoreApplication::setOrganizationDomain("https://github.com/giovannic96/Real-time-collaborative-text-editor");
    QCoreApplication::setApplicationName("legoRT");

    //Launch the application
    QApplication a(argc, argv);

    //Building first window (login).       //TODO --> Add a splashscreen

    StartWindow w;
    w.show();

    return a.exec();
    //Starting point of the event loop of "thread GUI" (or primary thread)
    /*Q_INIT_RESOURCE(textedit);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("QtProject");
    QCoreApplication::setApplicationName("Rich Text");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(a);

    TextEdit mw;

    const QRect availableGeometry = QApplication::desktop()->availableGeometry(&mw);
    mw.resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    mw.move((availableGeometry.width() - mw.width()) / 2,
            (availableGeometry.height() - mw.height()) / 2);

    if (!mw.load(parser.positionalArguments().value(0, QLatin1String(":/example.html"))))
        mw.fileNew();

    mw.show();
    return a.exec();*/
}
