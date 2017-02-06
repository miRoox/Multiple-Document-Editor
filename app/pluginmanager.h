#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QList>
#include <QHash>

class MdeWindow;
class IPluginBase;
class IEditorPlugin;
class IEditor;

class PluginManager : public QObject
{
    Q_OBJECT

    using PluginSpec = QStringList;
public:
    explicit PluginManager(MdeWindow *parent = 0);
    void extraInitialize();
    IEditor * defaultEditor();
    IEditor * defaultBrowser();
    IEditor * editor(QString file);
    IEditor * selectEditor();

signals:

public slots:
    void setDisabled(PluginSpec spec);
    void setDefaultEditor(PluginSpec spec);
    void setDefaultBrowser(PluginSpec spec);
    void setEditor(QString suffix, PluginSpec spec);
    void loadPlugins();
    void unloadPlugins();

private:
    void loadSettings();
    void saveSettings();

private:
    MdeWindow * win;
    QHash<PluginSpec, QObject*> plugins;
    QSet<PluginSpec> editors;
    QSet<PluginSpec> disabledPlugins;
    QHash<QString, PluginSpec> mapper;
};

#endif // PLUGINMANAGER_H
