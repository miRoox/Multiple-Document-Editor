#ifndef PLUGINMANAGER_P_H
#define PLUGINMANAGER_P_H

#include <QObject>
#include <QHash>
#include <QSet>

#include "pluginmanager.h"

class PluginManagerPrivate : public QObject
{
    Q_OBJECT

    using PluginSpec = PluginManager::PluginSpec;
private:
    explicit PluginManagerPrivate(PluginManager *parent = 0);

    void loadSettings();
    void saveSettings();
    void initViewer();
    void checkDisabled();
    void checkMapper();

    static PluginSpec specFromVariantHash(const QVariantHash &data);
    static QVariantHash variantHashFromSpec(const PluginSpec &spec);

private:
    PluginManager * p;
    MdeSettings * coreSettings;
    MdeWindow * win;
    QHash<PluginSpec, QObject*> plugins;
    QSet<PluginSpec> editors;
    QSet<PluginSpec> disabledPlugins;
    QHash<QString, PluginSpec> mapper;
    QStringList suffixDesc;
    friend class PluginManager;
};

#define DEFEDITSUFX "."
#define DEFBRSWSUFX "*"

#endif // PLUGINMANAGER_P_H
