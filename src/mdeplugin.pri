depfile = $$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri)
# a plugin dependency file 
# need PLUGIN_NAME , LIB_DEPENDS
# can have PLUGIN_VERSION , PLUGIN_DEPENDS , PLUGIN_RECOMANDS
exists($$depfile) {
    include($$depfile)
    isEmpty(PLUGIN_NAME): \
        error("$$basename(depfile) does not define PLUGIN_NAME.")
} else {
    isEmpty(PLUGIN_NAME): \
        error("PLUGIN_NAME is empty. Maybe you meant to create $$basename(depfile)?")
}

TARGET = $$PLUGIN_NAME

plugin_dep = $$PLUGIN_DEPENDS
plugin_rcmd = $$PLUGIN_RECOMANDS

include(../mdeuniversal.pri)

!isEmpty(PLUGIN_VERSION): VERSION = $$PLUGIN_VERSION

defineReplace(dependencyName) {
    include($$MDE_SOURCE_ROOT/src/plugins/$$1/$${1}_dependencies.pri)
    return($$PLUGIN_NAME)
}

defineReplace(dependencyVersion) {
    include($$MDE_SOURCE_ROOT/src/plugins/$$1/$${1}_dependencies.pri)
    isEmpty(PLUGIN_VERSION): \
        return($$MDE_VERSION)
    return($$PLUGIN_VERSION)
}

# for substitution in the .json
dependencyList =
for(dep, plugin_deps) {
    dependencyList += "        { \"Name\" : \"$$dependencyName($$dep)\", \"Version\" : \"$$dependencyVersion($$dep)\" }"
}
for(dep, plugin_recmds) {
    dependencyList += "        { \"Name\" : \"$$dependencyName($$dep)\", \"Version\" : \"$$$$dependencyVersion($$dep)\", \"Optional\" : true }"
}
dependencyList = $$join(dependencyList, ",$$escape_expand(\\n)")

dependencyList = "\"Dependencies\" : [$$escape_expand(\\n)$$dependencyList$$escape_expand(\\n)    ]"

PLUGINJSON = $$_PRO_FILE_PWD_/$${TARGET}.json
PLUGINJSON_IN = $${PLUGINJSON}.in
exists($$PLUGINJSON_IN) {
    DISTFILES += $$PLUGINJSON_IN
    QMAKE_SUBSTITUTES += $$PLUGINJSON_IN
    PLUGINJSON = $$OUT_PWD/$${TARGET}.json
} else {
    # need to support that for external plugins
    DISTFILES += $$PLUGINJSON
}

DESTDIR = $$MDE_PLUGIN_PATH
LIBS += -L$$DESTDIR

TARGET = $$qtLibraryTarget($$TARGET)
TEMPLATE = lib
CONFIG += plugin plugin_with_soname
