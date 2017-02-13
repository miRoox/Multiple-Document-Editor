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
    mylogger.cpp \
    mdesettings.cpp \
    mdesettings_p.cpp \
    mdewindow_p.cpp \
    mdisubwindow_p.cpp \
    pluginmanager_p.cpp

RESOURCES += \
    myimage.qrc

RC_ICONS = notepad.ico

HEADERS += \
    mdewindow.h \
    ipluginbase.h \
    ieditor.h \
    pluginmanager.h \
    mdisubwindow.h \
    mylogger.h \
    mdesettings.h \
    mdesettings_p.h \
    mdewindow_p.h \
    mdisubwindow_p.h \
    pluginmanager_p.h

FORMS += \
    mdewindow.ui

TRANSLATIONS += \
    mde_en.ts  \
    mde_zh_CN.ts
