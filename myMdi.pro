#-------------------------------------------------
#
# Project created by QtCreator 2016-10-29T10:01:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myMdi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mdichild.cpp

HEADERS  += mainwindow.h \
    mdichild.h

FORMS    += mainwindow.ui

RESOURCES += \
    myimage.qrc

RC_FILE += myico.rc
