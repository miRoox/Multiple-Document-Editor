TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libs\
          app \
          plugins

app.depends = libs
plugins.depends = libs
