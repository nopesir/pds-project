QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = editoRT


HEADERS         = \
    CRDT.h \
    MyQTextEdit.h \
    clientproc.h \
    editorwindow.h \
    file.h \
    homewindow.h \
    json.hpp \
    jsonize.h \
    message.h \
    profilewindow.h \
    sstyle.h \
    startwindow.h \
    symbol.h
SOURCES         = \
                  CRDT.cpp \
                  MyQTextEdit.cpp \
                  clientproc.cpp \
                  editorwindow.cpp \
                  file.cpp \
                  homewindow.cpp \
                  jsonize.cpp \
                  main.cpp \
                  message.cpp \
                  profilewindow.cpp \
                  sstyle.cpp \
                  startwindow.cpp \
                  symbol.cpp
FORMS           += startwindow.ui \
    editor.ui \
    editorwindow.ui \
    homewindow.ui \
    profilewindow.ui

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
