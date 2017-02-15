#ifndef PLUGINMANAGER_P_H
#define PLUGINMANAGER_P_H

#include <QObject>
#include <QHash>
#include <QSet>

#include "pluginmanager.h"

template <typename T> class QList;
class QStandardItem;
class QStandardItemModel;
class QModelIndex;

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

    QList<QStandardItem *> createPluginSpecItem(const PluginSpec &spec);
    QStandardItemModel * createPluginSpecModel(const QList<PluginSpec> &specs);
    PluginSpec specFromIndex(const QStandardItemModel *model, QModelIndex index);

    static PluginSpec specFromVariantHash(const QVariantHash &data);
    static QVariantHash variantHashFromSpec(const PluginSpec &spec);

private slots:

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
