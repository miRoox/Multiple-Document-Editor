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

include(../shared/qtsingleapplication/qtsingleapplication.pri)

SOURCES += main.cpp \
    mdewindow.cpp \
    plaintexteditor.cpp

RESOURCES += \
    myimage.qrc

RC_ICONS = notepad.ico

HEADERS += \
    ../interface.h \
    mdewindow.h \
    plaintexteditor.h

FORMS += \
    mdewindow.ui

