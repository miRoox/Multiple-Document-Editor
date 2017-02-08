#include "pluginmanager.h"
#include "ipluginbase.h"
#include "ieditor.h"
#include "mdewindow.h"
#include <QSettings>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QPluginLoader>
#include <QLibrary>
#include <QMenu>
#include <QApplication>
#include <QDebug>

PluginManager::PluginManager(MdeWindow *parent) : QObject(parent),
    win(parent), plugins(), editors(), disabledPlugins(), mapper(),
    suffixDesc()
{
    suffixDesc += tr("Any file") + "(*)";
}

void PluginManager::initViewer()
{
    win->menuSettings()->addAction(tr("Plugin Manager"));
}

IEditor * PluginManager::defaultEditor() const
{
    PluginSpec spec = mapper.value(".");
    if(spec.isEmpty())
        return 0;
    QObject * obj = plugins.value(spec);
    if(!obj)
        return 0;
    auto plug = qobject_cast<IEditorPlugin*>(obj);
    return plug->create();
}

IEditor * PluginManager::defaultBrowser() const
{
    PluginSpec spec = mapper.value("*");
    if(spec.isEmpty())
        return 0;
    QObject * obj = plugins.value(spec);
    if(!obj)
        return 0;
    auto plug = qobject_cast<IEditorPlugin*>(obj);
    return plug->create();
}

IEditor * PluginManager::editor(QString file) const
{
    QFileInfo info(file);
    PluginSpec spec = mapper.value(info.suffix());
    if(spec.isEmpty())
        spec = mapper.value(info.completeSuffix());
    if(spec.isEmpty())
        return 0;
    QObject * obj = plugins.value(spec);
    if(!obj)
        return 0;
    auto plug = qobject_cast<IEditorPlugin*>(obj);
    return plug->create();
}

void PluginManager::setDisabled(const PluginSpec spec)
{
    if(plugins.contains(spec))
        disabledPlugins.insert(spec);
}

void PluginManager::setEnabled(const PluginSpec spec)
{
    if(plugins.contains(spec))
        disabledPlugins.remove(spec);
}

void PluginManager::setDefaultEditor(const PluginSpec spec)
{
    if(editors.contains(spec))
        mapper.insert(".",spec);
}

void PluginManager::setDefaultBrowser(const PluginSpec spec)
{
    if(editors.contains(spec))
        mapper.insert("*",spec);
}

void PluginManager::setEditor(const QString suffix, const PluginSpec spec)
{
    if(editors.contains(spec))
        mapper.insert(suffix,spec);
}

QString PluginManager::fileNameFilter()
{
    return suffixDesc.join(";;");
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
        QString suffix = settings.value("suffix").toString();
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
    foreach (PluginSpec spec, disabledPlugins.toList()) {
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
        if(!QLibrary::isLibrary(fileName))
            continue;
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
                if(plugins.contains(spec))
                    continue;
                if(disabledPlugins.contains(spec)) {
                    plugins.insert(spec,0);
                    continue;
                }
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

void PluginManager::unloadPlugins()
{
    foreach (PluginSpec spec, plugins.keys()) {
        QObject * obj = plugins.value(spec);
        if(obj) {
            auto plugin = qobject_cast<IPluginBase *>(obj);
            plugin->uninstall();
        }
    }
}

void PluginManager::checkDisabled()
{
    disabledPlugins &= QSet<PluginSpec>::fromList(plugins.keys());
}

void PluginManager::checkMapper()
{
    foreach (QString suffix, mapper.keys()) {
        if(!editors.contains(mapper.value(suffix)))
            mapper.remove(suffix);
    }
    if(!mapper.contains(".")) { //default editor
        if(mapper.contains("txt"))
            setDefaultEditor(mapper.value("txt"));
    }
}


void PluginManager::loadSuffixDescription()
{
    QStringList descs;
    foreach (QString suffix, mapper.keys()) {
        if(suffix=="*" || suffix==".")
            continue;
        IEditorPlugin * plug = qobject_cast<IEditorPlugin*>(mapper.value(suffix));
        QString desc = plug->typeDescription(suffix);
        if(desc.isEmpty()) {
            desc = suffix.toUpper() + tr(" file");
        }
        desc = desc + "(*." + suffix + ")";
        qDebug() << "Add suffix description:" << desc;
        descs += desc;
    }
    descs.sort(Qt::CaseInsensitive);
    suffixDesc += descs;
}
