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

#Boost library
win32 {
    QMAKE_CXXFLAGS += -DWIN32_LEAN_AND_MEAN
    INCLUDEPATH += C:/Boost/include/boost-1_74
    LIBS += -LC:/Boost/lib \
            -lboost_serialization-mgw8-mt-x64-1_74 \
            -lboost_filesystem-mgw8-mt-x64-1_74 \
            -lboost_system-mgw8-mt-x64-1_74 \
            -lboost_thread-mgw8-mt-x64-1_74 \
            -lws2_32
}
