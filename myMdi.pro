#-------------------------------------------------
#
# Project created by QtCreator 2016-10-29T10:01:14
#
#-------------------------------------------------

QT       += core gui

QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myMdi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mdichild.cpp \
    findreplacedialog.cpp \
    singleapplication.cpp

HEADERS  += mainwindow.h \
    mdichild.h \
    findreplacedialog.h \
    singleapplication.h

FORMS    += mainwindow.ui \
    findreplacedialog.ui

RESOURCES += \
    myimage.qrc

RC_FILE += myico.rc

TRANSLATIONS = zh_cn.ts \
    en_us.ts
