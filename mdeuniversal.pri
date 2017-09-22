!isEmpty(MDE_UNIVERSAL_PRI_INCLUDED):error("mdeuniversal.pri has been included")
MDE_UNIVERSAL_PRI_INCLUDED = 1

include(mdeinfo.pri)

isEmpty(VERSION): VERSION = $$MDE_VERSION
CONFIG += c++14
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # disables all the APIs deprecated before Qt 6.0.0

qt {
    contains(QT, core): QT += concurrent
    contains(QT, gui): QT += widgets
}

INCLUDEPATH += \
    $$MDE_SOURCE_ROOT/src  \
    $$MDE_SOURCE_ROOT/src/libs  \
    $$MDE_BUILD_ROOT/src

LIBS *= -L$$MDE_LIBRARY_PATH

!isEmpty(MDE_PLUGIN_DEPENDS) {
    LIBS *= -L$$MDE_PLUGIN_PATH
}

DEFINES += $$shell_quote(MDE_VERSION=\"$$MDE_VERSION\")
DEFINES += $$shell_quote(MDE_COMPAT_VERSION=\"$$MDE_COMPAT_VERSION\")

RELATIVE_PLUGIN_PATH = $$relative_path($$MDE_PLUGIN_PATH, $$MDE_BIN_PATH)
RELATIVE_LIBRARY_PATH = $$relative_path($$MDE_LIBRARY_PATH, $$MDE_BIN_PATH)
RELATIVE_DATA_PATH = $$relative_path($$MDE_DATA_PATH, $$MDE_BIN_PATH)
RELATIVE_DOC_PATH = $$relative_path($$MDE_DOC_PATH, $$MDE_BIN_PATH)
RELATIVE_TRANSLATION_PATH = $$relative_path($$MDE_TRANSLATION_PATH, $$MDE_BIN_PATH)
DEFINES += $$shell_quote(RELATIVE_PLUGIN_PATH=\"$$RELATIVE_PLUGIN_PATH\")
DEFINES += $$shell_quote(RELATIVE_LIBRARY_PATH=\"$$RELATIVE_LIBRARY_PATH\")
DEFINES += $$shell_quote(RELATIVE_DATA_PATH=\"$$RELATIVE_DATA_PATH\")
DEFINES += $$shell_quote(RELATIVE_DOC_PATH=\"$$RELATIVE_DOC_PATH\")
DEFINES += $$shell_quote(RELATIVE_TRANSLATION_PATH=\"$$RELATIVE_TRANSLATION_PATH\")

defineReplace(qtLibraryName) {
   RET = $$qtLibraryTarget($$1)
   win32 {
      VER = $$2
      isEmpty(VER): VER = $$VERSION
      VERSION_LIST = $$split(VER, .)
      RET = $$RET$$first(VERSION_LIST)
   }
   return($$RET)
}

# recursively resolve plugin deps
# usage: PLUGIN_DEPENDS 
done_plugins =
for(ever) {
    isEmpty(PLUGIN_DEPENDS): \
        break()
    done_plugins += $$PLUGIN_DEPENDS
    for(dep, PLUGIN_DEPENDS) {
        include($$MDE_SOURCE_ROOT/src/plugins/$$dep/$${dep}_dependencies.pri)
        LIBS += -l$$qtLibraryName($$PLUGIN_NAME, $$PLUGIN_VERSION)
    }
    PLUGIN_DEPENDS = $$unique(PLUGIN_DEPENDS)
    PLUGIN_DEPENDS -= $$unique(done_plugins)
}

# recursively resolve library deps
# usage: LIB_DEPENDS
done_libs =
for(ever) {
    isEmpty(LIB_DEPENDS): \
        break()
    done_libs += $$LIB_DEPENDS
    for(dep, LIB_DEPENDS) {
        include($$MDE_SOURCE_ROOT/src/libs/$$dep/$${dep}_dependencies.pri)
        LIBS += -l$$qtLibraryName($$LIB_NAME)
    }
    LIB_DEPENDS = $$unique(LIB_DEPENDS)
    LIB_DEPENDS -= $$unique(done_libs)
}
