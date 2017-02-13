#include "pluginmanager_p.h"
#include "ipluginbase.h"
#include "ieditor.h"
#include "mdewindow.h"
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QMenu>
#include <QDebug>

PluginManagerPrivate::PluginManagerPrivate(PluginManager *parent)
    : QObject(parent),
      p(parent),
      plugins(),
      editors(),
      disabledPlugins(),
      mapper(),
      suffixDesc()
{
    win = 0;
}

void PluginManagerPrivate::loadSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    qInfo() << "Plugin manager: loading settings..";
    //disabled plugins
    int size = settings.beginReadArray("disabled");
    qInfo() << "loading disabled plugins list..";
    for(int i=0; i<size; ++i) {
        settings.setArrayIndex(i);
        PluginSpec spec =  specFromVariantHash(settings.value("spec").toHash());
        disabledPlugins.insert(spec);
    }
    settings.endArray();
    //editor mapper
    size = settings.beginReadArray("mapper");
    qInfo() << "loading file-editor mapper..";
    for(int i=0; i<size; ++i) {
        settings.setArrayIndex(i);
        PluginSpec spec = specFromVariantHash(settings.value("spec").toHash());
        if(disabledPlugins.contains(spec))
            continue;
        QString suffix = settings.value("suffix").toString();
        mapper.insert(suffix,spec);
    }
    settings.endArray();
    settings.endGroup();
    qInfo() << "Plugin manager: settings are loaded.";
}

void PluginManagerPrivate::saveSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    qInfo() << "Plugin manager: saving settings..";
    //disabled plugins
    settings.beginWriteArray("disabled");
    qInfo() << "saving disabled plugins list..";
    int i = 0;
    foreach (PluginSpec spec, disabledPlugins.toList()) {
        settings.setArrayIndex(i++);
        settings.setValue("spec",variantHashFromSpec(spec));
    }
    settings.endArray();
    //editor mapper
    settings.beginWriteArray("mapper");
    qInfo() << "saving file-editor mapper..";
    i = 0;
    foreach (QString suffix, mapper.keys()) {
        settings.setArrayIndex(i++);
        settings.setValue("suffix",suffix);
        settings.setValue("spec",variantHashFromSpec(mapper.value(suffix)));
    }
    settings.endArray();
    settings.endGroup();
    qInfo() << "Plugin manager: settings are saved.";
}

void PluginManagerPrivate::initViewer()
{
    win->menuSettings()->addAction(tr("Plugin Manager"));
}

void PluginManagerPrivate::checkDisabled()
{
    disabledPlugins &= QSet<PluginSpec>::fromList(plugins.keys());
}

void PluginManagerPrivate::checkMapper()
{
    foreach (QString suffix, mapper.keys()) {
        if(!editors.contains(mapper.value(suffix)))
            mapper.remove(suffix);
    }
    if(!mapper.contains(DEFEDITSUFX)) { //default editor
        if(mapper.contains("txt"))
            p->setDefaultEditor(mapper.value("txt"));
    }
}

PluginManagerPrivate::PluginSpec PluginManagerPrivate::specFromVariantHash(const QVariantHash &data)
{
    PluginSpec spec;
    spec.insert(PLUGINMETADATA_NAME,data.value(PLUGINMETADATA_NAME).toString());
    spec.insert(PLUGINMETADATA_VER,data.value(PLUGINMETADATA_VER).toString());
    spec.insert(PLUGINMETADATA_VENDOR,data.value(PLUGINMETADATA_VENDOR).toString());
    spec.insert(PLUGINMETADATA_PLATFORM,data.value(PLUGINMETADATA_PLATFORM).toString());
    return spec;
}

QVariantHash PluginManagerPrivate::variantHashFromSpec(const PluginManagerPrivate::PluginSpec &spec)
{
    QVariantHash data;
    data.insert(PLUGINMETADATA_NAME,spec.value(PLUGINMETADATA_NAME));
    data.insert(PLUGINMETADATA_VER,spec.value(PLUGINMETADATA_VER));
    data.insert(PLUGINMETADATA_VENDOR,spec.value(PLUGINMETADATA_VENDOR));
    data.insert(PLUGINMETADATA_PLATFORM,spec.value(PLUGINMETADATA_PLATFORM));
    return data;
}
