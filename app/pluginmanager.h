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
    void loadSettings();
    void saveSettings();//before close
    void initViewer();
    IEditor * defaultEditor() const;
    IEditor * defaultBrowser() const;
    IEditor * editor(QString file) const;
    IEditor * selectEditor();

signals:

public slots:
    void setDisabled(const PluginSpec spec);//restart to make effect
    void setEnabled(const PluginSpec spec);//restart to make effect
    void setDefaultEditor(const PluginSpec spec);
    void setDefaultBrowser(const PluginSpec spec);
    void setEditor(const QString suffix, const PluginSpec spec);
    void loadPlugins();
    void unloadPlugins();//before  close
    void checkDisabled();
    void checkMapper();

private:
    MdeWindow * win;
    QHash<PluginSpec, QObject*> plugins;
    QSet<PluginSpec> editors;
    QSet<PluginSpec> disabledPlugins;
    QHash<QString, PluginSpec> mapper;
};

#endif // PLUGINMANAGER_H
