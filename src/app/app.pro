include(../../mdeuniversal.pri)

TEMPLATE = app
TARGET = $$MDE_APP_TARGET
DESTDIR = $$MDE_APP_PATH
VERSION = $$MDE_VERSION

MDE_FULL_NAME = Multiple Document Editor
DEFINES += $$shell_quote(MDE_FULL_NAME=\"$$MDE_FULL_NAME\")

SOURCES += main.cpp \
    mdeapplication.cpp

HEADERS += \
    mdeapplication.h

