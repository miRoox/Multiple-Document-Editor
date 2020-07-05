#-------------------------------------------------
#
# Project created by QtCreator 2017-02-17T11:16:45
#
#-------------------------------------------------

QT       += widgets

TARGET = $$qtLibraryTarget(mdewidget)
DESTDIR       = ../../libs
TEMPLATE = lib
CONFIG += shared dll
CONFIG += c++14

INCLUDEPATH  += ../../libs

LIBS *= -L../../libs \
        -l$$qtLibraryTarget(generalsettings) \
        -l$$qtLibraryTarget(extensionsystem)

DEFINES += MDEWIDGET_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    mdewindow.cpp \
    mdewindow_p.cpp \
    mdisubwindow.cpp \
    mdisubwindow_p.cpp \
    generalsettingswidget.cpp

HEADERS +=\
        mdewidget_global.h \
    mdewindow.h \
    mdewindow_p.h \
    mdisubwindow.h \
    mdisubwindow_p.h \
    generalsettingswidget.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    mdewindow.ui \
    generalsettingswidget.ui

RESOURCES += \
    myimage.qrc
