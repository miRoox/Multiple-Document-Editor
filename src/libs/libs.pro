include(../../mdeuniversal.pri)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS   = \
    utils \
    extensionsystem


# resolve dependencies
for(l, SUBDIRS) {
    LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$LIB_DEPENDS
}
