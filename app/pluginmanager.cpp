#include "pluginmanager.h"
#include "pluginmanager_p.h"
#include "ipluginbase.h"
#include "ieditor.h"
#include "mdewindow.h"
#include <QHash>
#include <QSet>
#include <QSettings>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QPluginLoader>
#include <QLibrary>
#include <QApplication>
#include <QDebug>


PluginManager::PluginManager(QObject *parent) : QObject(parent)
{
    p = new PluginManagerPrivate(this);
    p->suffixDesc += tr("Any file") + "(*)";
    p->loadSettings();
}

PluginManager::~PluginManager()
{
    p->saveSettings();
}

void PluginManager::loadSettings(MdeSettings *s)
{
    p->coreSettings = s;
}

void PluginManager::setMDE(MdeWindow *w)
{
    p->win = w;
    foreach (QObject * obj, p->plugins.values()) {
        if(obj) {
            auto plugin = qobject_cast<IPluginBase*>(obj);
            plugin->setMDE(p->win);
        }
    }
    p->win->setPluginManager(this);
    p->initViewer();
}

IEditor * PluginManager::defaultEditor() const
{
    PluginSpec spec = p->mapper.value(DEFEDITSUFX);
    if(spec.isEmpty()) {
        qWarning() << "The default editor is not set.";
        return 0;
    }
    QObject * obj = p->plugins.value(spec);
    if(!obj)
        return 0;
    auto plug = qobject_cast<IEditorPlugin*>(obj);
    return plug->create();
}

IEditor * PluginManager::defaultBrowser() const
{
    PluginSpec spec = p->mapper.value(DEFBRSWSUFX);
    if(spec.isEmpty()) {
        qWarning() << "The default browser is not set.";
        return 0;
    }
    QObject * obj = p->plugins.value(spec);
    if(!obj)
        return 0;
    auto plug = qobject_cast<IEditorPlugin*>(obj);
    return plug->create();
}

IEditor * PluginManager::editor(QString file) const
{
    QFileInfo info(file);
    PluginSpec spec = p->mapper.value(info.suffix());
    if(spec.isEmpty())
        spec = p->mapper.value(info.completeSuffix());
    if(spec.isEmpty()) {
        qWarning() << "No suitable editor for this file.";
        return 0;
    }
    QObject * obj = p->plugins.value(spec);
    if(!obj)
        return 0;
    auto plug = qobject_cast<IEditorPlugin*>(obj);
    return plug->create();
}

IEditor *PluginManager::selectEditor()
{
    return 0;//TODO
}

void PluginManager::setDisabled(const PluginSpec spec)
{
    if(p->plugins.contains(spec))
        p->disabledPlugins.insert(spec);
}

void PluginManager::setEnabled(const PluginSpec spec)
{
    if(p->plugins.contains(spec))
        p->disabledPlugins.remove(spec);
}

void PluginManager::setDefaultEditor(const PluginSpec spec)
{
    if(p->editors.contains(spec))
        p->mapper.insert(DEFEDITSUFX,spec);
}

void PluginManager::setDefaultBrowser(const PluginSpec spec)
{
    if(p->editors.contains(spec))
        p->mapper.insert(DEFBRSWSUFX,spec);
}

void PluginManager::setEditor(const QString suffix, const PluginSpec spec)
{
    if(p->editors.contains(spec))
        p->mapper.insert(suffix,spec);
}

QString PluginManager::fileNameFilter() const
{
    return p->suffixDesc.join(";;");
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
    qInfo() << "Plugin manager: loading plugins...";

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if(!QLibrary::isLibrary(fileName))
            continue;
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject * obj = loader.instance();
        if(obj) {
            auto plugin = qobject_cast<IPluginBase *>(obj);
            if(plugin) {
                PluginSpec spec = p->specFromVariantHash(loader.metaData().toVariantHash());
                if(p->plugins.contains(spec))
                    continue;
                if(p->disabledPlugins.contains(spec)) {
                    p->plugins.insert(spec,0);
                    continue;
                }
                qInfo() << "Plugin manager: loading plugin" << spec;
                plugin->initialize(p->coreSettings);
                auto editorPlug = qobject_cast<IEditorPlugin *>(obj);
                if(editorPlug) {
                    QStringList suffixes = editorPlug->designedTypes();
                    foreach (QString suffix, suffixes) {
                        if(!p->mapper.contains(suffix))
                            p->mapper.insert(suffix,spec);
                    }
                    spec[SPECKEY_CATEGORY] = tr("editors");
                    p->editors.insert(spec);
                }
                else {
                    spec[SPECKEY_CATEGORY] = tr("utilities");
                }
                p->plugins.insert(spec,obj);
            }
        }
        else {
            qWarning() << loader.errorString();
        }
    }
    p->checkDisabled();
    p->checkMapper();
}

void PluginManager::loadSuffixDescription()
{
    QStringList descs;
    foreach (QString suffix, p->mapper.keys()) {
        if(suffix==DEFBRSWSUFX || suffix==DEFEDITSUFX)
            continue;
        auto plug = qobject_cast<IEditorPlugin*>(p->plugins.value(p->mapper.value(suffix)));
        QString desc = plug->typeDescription(suffix);
        if(desc.isEmpty()) {
            desc = suffix.toUpper() + tr(" file");
        }
        desc = desc + "(*." + suffix + ")";
        qInfo() << "Add suffix description:" << desc;
        descs += desc;
    }
    descs.sort(Qt::CaseInsensitive);
    p->suffixDesc += descs;
}
