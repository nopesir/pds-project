QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = textedit

HEADERS         = textedit.h \
    clientproc.h \
    file.h \
    json.hpp \
    jsonize.h \
    message.h \
    sstyle.h \
    startwindow.h \
    symbol.h \
    tsymbol.h
SOURCES         = textedit.cpp \
                  clientproc.cpp \
                  file.cpp \
                  jsonize.cpp \
                  main.cpp \
                  message.cpp \
                  sstyle.cpp \
                  startwindow.cpp \
                  symbol.cpp \
                  tsymbol.cpp
FORMS           += startwindow.ui

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

EXAMPLE_FILES = textedit.qdoc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/textedit
INSTALLS += target

FORMS += \
    startwindow.ui
