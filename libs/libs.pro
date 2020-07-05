TEMPLATE = subdirs

SUBDIRS += \
    generalsettings \
    extensionsystem \
    mdewidget

mdewidget.depends = extensionsystem \
                    generalsettings
extensionsystem.depends = generalsettings
