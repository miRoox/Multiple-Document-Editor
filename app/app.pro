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

include(../shared/qtsingleapplication/qtsingleapplication.pri)

SOURCES += main.cpp \
    mdewindow.cpp \
    pluginmanager.cpp \
    mdisubwindow.cpp \
    mylogger.cpp

RESOURCES += \
    myimage.qrc

RC_ICONS = notepad.ico

HEADERS += \
    mdewindow.h \
    ipluginbase.h \
    ieditor.h \
    pluginmanager.h \
    mdisubwindow.h \
    mylogger.h

FORMS += \
    mdewindow.ui

TRANSLATIONS += \
    mde_zh_CN.ts
