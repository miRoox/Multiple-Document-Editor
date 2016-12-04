
TEMPLATE      = lib
CONFIG       += plugin
QT           += core widgets gui printsupport
INCLUDEPATH  += ../../mde
HEADERS       = htmlviewer.h
SOURCES       = htmlviewer.cpp
TARGET        = $$qtLibraryTarget(htmlviewer)
DESTDIR       = ../../plugins
#OTHER_FILES  += htmlviewer.json
