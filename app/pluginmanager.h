#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QList>
#include <QHash>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
QT_END_NAMESPACE

class MdeSettings;
class MdeWindow;
class IPluginBase;
class IEditorPlugin;
class IEditor;

class PluginManager : public QObject
{
    Q_OBJECT

    using PluginSpec = QHash<QString,QString>;
public:
    explicit PluginManager(QObject *parent = 0);
    ~PluginManager();
    void loadSettings(MdeSettings * s);
    void setMDE(MdeWindow * w);
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
    void loadSettings();
    void saveSettings();
    void initViewer();
    void checkDisabled();
    void checkMapper();

    static PluginSpec specFromVariantHash(const QVariantHash &data);
    static QVariantHash variantHashFromSpec(const PluginSpec &spec);

private:
    MdeSettings * coreSettings;
    MdeWindow * win;
    QHash<PluginSpec, QObject*> plugins;
    QSet<PluginSpec> editors;
    QSet<PluginSpec> disabledPlugins;
    QHash<QString, PluginSpec> mapper;
    QStringList suffixDesc;
};

#endif // PLUGINMANAGER_H
