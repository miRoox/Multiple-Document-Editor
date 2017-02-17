#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "extensionsystem/extensionsystem_global.h"

#include <QObject>
#include <QHash>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
QT_END_NAMESPACE

class PluginManagerPrivate;
class GeneralSettings;
class MdeWindow;
class IPluginBase;
class IEditorPlugin;
class IEditor;

class EXTENSIONSYSTEMSHARED_EXPORT PluginManager : public QObject
{
    Q_OBJECT

public:
    using PluginSpec = QHash<QString,QString>;

    explicit PluginManager(QObject *parent = 0);
    ~PluginManager();
    void loadGeneralSettings(GeneralSettings * s);//only once
    void setMDE(MdeWindow * w);//only once
    void loadPlugins();
    void loadSuffixDescription();
    IEditor * defaultEditor() const;
    IEditor * defaultBrowser() const;
    IEditor * editor(QString file) const;
    IEditor * selectEditor();
    QString fileNameFilter() const;

signals:

public slots:
    void setDisabled(const PluginSpec spec);//restart to make effect
    void setEnabled(const PluginSpec spec);//restart to make effect
    void setDefaultEditor(const PluginSpec spec);
    void setDefaultBrowser(const PluginSpec spec);
    void setEditor(const QString suffix, const PluginSpec spec);

private:
    PluginManagerPrivate * p;
};

#define SPECKEY_CATEGORY "category"

#endif // PLUGINMANAGER_H
