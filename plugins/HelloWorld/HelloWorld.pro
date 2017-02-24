TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
INCLUDEPATH  += ../../app \
                ../../libs
TARGET        = $$qtLibraryTarget(helloworld)
DESTDIR       = ../../plugins

LIBS *= -L../../libs \
        -l$$qtLibraryTarget(generalsettings) \
        -l$$qtLibraryTarget(mdewidget)

HEADERS += \
    helloworld.h

SOURCES += \
    helloworld.cpp

DISTFILES += \
    helloworld.json
TRANSLATIONS += \
    helloworld_en.ts  \
    helloworld_zh_CN.ts

RESOURCES += \
    src.qrc
