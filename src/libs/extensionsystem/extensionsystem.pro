DEFINES += EXTENSIONSYSTEM_LIBRARY
include(../../mdelibrary.pri)

HEADERS += extensionsystem_global.h \
    iplugin.h \
    pluginmanager.h \
    pluginmanager_p.h

SOURCES += \
    pluginmanager.cpp
