TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libs\
          app \
          plugins

app.depends = libs
plugins.depends = libs


TRANSLATIONS += \
    mde_en.ts  \
    mde_zh_CN.ts
