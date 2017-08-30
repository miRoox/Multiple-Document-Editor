include(../../mdeuniversal.pri)

TEMPLATE  = subdirs

SUBDIRS   = 


# resolve dependencies
for(p, SUBDIRS) {
    PLUGIN_DEPENDS =
    include($$p/$${p}_dependencies.pri)
    pv = $${p}.depends
    $$pv = $$PLUGIN_DEPENDS
}

