include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
# a library dependency file 
# need LIB_NAME
# can have LIB_DEPENDS
isEmpty(LIB_NAME): \
    error("LIB_NAME was not defined .")

TARGET = $$LIB_NAME

include(../mdeuniversal.pri)

DESTDIR = MDE_LIBRARY_PATH

TARGET = $$qtLibraryTarget($$TARGET)

TEMPLATE = lib
CONFIG += shared dll

