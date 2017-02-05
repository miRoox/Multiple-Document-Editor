#include "pluginmanager.h"
#include "ipluginbase.h"
#include "ieditor.h"
#include "mdewindow.h"
#include <QDir>
#include <QPluginLoader>
#include <QMenu>
#include <QApplication>
#include <QDebug>

PluginManager::PluginManager(MdeWindow *parent) : QObject(parent),
    win(parent)
{
    plugins = QList<IPluginBase*>();
    editors = QList<IEditorPlugin*>();
}

void PluginManager::extraInitialize()
{
    win->menuSettings()->addAction(tr("Plugin Manager"));
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
            IPluginBase * plugin = qobject_cast<IPluginBase *>(obj);
            if(plugin) {
                plugin->install(win);
                plugins.append(plugin);
                IEditorPlugin * editorPlug = qobject_cast<IEditorPlugin *>(obj);
                if(editorPlug)
                    editors.append(editorPlug);
            }
        }
    }
}
