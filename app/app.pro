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
DESTDIR    = ..
TEMPLATE = app

CONFIG += c++14

INCLUDEPATH  += ../libs
LIBS *= -L../libs \
        -l$$qtLibraryTarget(generalsettings) \
        -l$$qtLibraryTarget(extensionsystem) \
        -l$$qtLibraryTarget(mdewidget)

include(../shared/qtsingleapplication/qtsingleapplication.pri)

SOURCES += main.cpp \
    mylogger.cpp 

RC_ICONS = notepad.ico

HEADERS += \
    mylogger.h 

