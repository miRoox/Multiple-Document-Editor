#include "pluginmanager.h"
#include "ipluginbase.h"
#include "ieditor.h"
#include "mdewindow.h"
#include <QSettings>
#include <QDir>
#include <QPluginLoader>
#include <QMenu>
#include <QApplication>
#include <QDebug>

PluginManager::PluginManager(MdeWindow *parent) : QObject(parent),
    win(parent)
{
    plugins = QHash<PluginSpec, QObject*>();
    disabledPlugins = QSet<PluginSpec>();
    editors = QSet<PluginSpec>();
    mapper = QHash<QString, PluginSpec>();
}

void PluginManager::extraInitialize()
{
    win->menuSettings()->addAction(tr("Plugin Manager"));
}

void PluginManager::loadSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    //disabled plugins
    int size = settings.beginReadArray("disabled");
    for(int i=0; i<size; ++i) {
        settings.setArrayIndex(i);
        PluginSpec spec = settings.value("spec").toStringList();
        disabledPlugins.insert(spec);
    }
    settings.endArray();
    //editor mapper
    size = settings.beginReadArray("mapper");
    for(int i=0; i<size; ++i) {
        settings.setArrayIndex(i);
        PluginSpec spec = settings.value("spec").toStringList();
        if(disabledPlugins.contains(spec))
            continue;
        QString suffix = settings.value("suffix");
        mapper.insert(suffix,spec);
    }
    settings.endArray();
    settings.endGroup();
}

void PluginManager::saveSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    //disabled plugins
    settings.beginWriteArray("disabled");
    int i = 0;
    foreach (PluginSpec spec, disabledPlugins) {
        settings.setArrayIndex(i++);
        settings.setValue("spec",spec);
    }
    settings.endArray();
    //editor mapper
    settings.beginWriteArray("mapper");
    i = 0;
    foreach (QString suffix, mapper.keys()) {
        settings.setArrayIndex(i++);
        settings.setValue("suffix",suffix);
        settings.setValue("spec",mapper.value(suffix));
    }
    settings.endArray();
    settings.endGroup();
}

void PluginManager::loadPlugins()
{
    QDir pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject * obj = loader.instance();
        qDebug() << loader.errorString();
        if(obj) {
            auto plugin = qobject_cast<IPluginBase *>(obj);
            if(plugin) {
                PluginSpec spec;
                spec << plugin->name();
                spec << plugin->version();
                spec << plugin->vendor();
                if(plugins.contains(spec) || disabledPlugins.contains(spec))
                    continue;
                plugin->install(win);
                plugins.insert(spec,obj);
                auto editorPlug = qobject_cast<IEditorPlugin *>(obj);
                if(editorPlug) {
                    QStringList suffixes = editorPlug->designedTypes();
                    foreach (QString suffix, suffixes) {
                        if(!mapper.contains(suffix))
                            mapper.insert(suffix,spec);
                    }
                    editors.insert(spec);
                }
            }
        }
    }
}
