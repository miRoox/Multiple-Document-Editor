#-------------------------------------------------
#
# Project created by QtCreator 2016-10-29T10:01:14
#
#-------------------------------------------------

QT       += core \
    gui \
    network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MDE
TEMPLATE = app


SOURCES += main.cpp \
    singleapplication.cpp \
    mdemanager.cpp \
    mdewindow.cpp

RESOURCES += \
    myimage.qrc

RC_ICONS = notepad.ico

HEADERS += \
    ../interface.h \
    singleapplication.h \
    mdemanager.h \
    mdewindow.h

FORMS += \
    mdewindow.ui

