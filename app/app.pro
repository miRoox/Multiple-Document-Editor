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

INCLUDEPATH  += ../libs
LIBS *= -L../libs \
        -l$$qtLibraryTarget(generalsettings) \
        -l$$qtLibraryTarget(extensionsystem) \
        -l$$qtLibraryTarget(mdewidget)

include(../shared/qtsingleapplication/qtsingleapplication.pri)

SOURCES += main.cpp \
    mylogger.cpp 

RESOURCES += \
    myimage.qrc

RC_ICONS = notepad.ico

HEADERS += \
    mylogger.h 

TRANSLATIONS += \
    mde_en.ts  \
    mde_zh_CN.ts
