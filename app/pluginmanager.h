#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>

class MdeWindow;
class IPluginBase;
class IEditorPlugin;
class IEditor;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(MdeWindow *parent = 0);
    void extraInitialize();
    IEditor * defaultEditor();
    IEditor * defaultBrowser();
    IEditor * editor(QString file);
    IEditor * selectEditor();

signals:

public slots:
    void loadPlugins();

private:
    MdeWindow * win;
    QList<IPluginBase*> plugins;
    QList<IEditorPlugin*> editors;
};

#endif // PLUGINMANAGER_H
