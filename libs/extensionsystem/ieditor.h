#ifndef IEDITOR_H
#define IEDITOR_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QWidget;
class QString;
class QStringList;
class QFileInfo;
QT_END_NAMESPACE

class MdiSubWindow;

class IEditorPlugin;

class IEditor
{
public:
    virtual ~IEditor() { }
    virtual IEditorPlugin * base() const = 0;
    virtual QWidget * widget() const = 0;//widget that will be set to mdi subwindow
    virtual void extraInitialize(MdiSubWindow *tab) = 0;//after widget set to mdi subwindow
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void newFile() = 0;
    virtual void loadFile(QString fileName) = 0;
    virtual void reload() = 0;
    virtual void save() = 0;
    virtual void saveAs() = 0;
    virtual QString title() const = 0;
    virtual QFileInfo file() const = 0;
    virtual bool close() = 0;
};

class IEditorPlugin
{
public:
    virtual ~IEditorPlugin() { }
    virtual IEditor * create() = 0;
    virtual QStringList designedTypes() const = 0;
    virtual QString typeDescription(QString suffix) const = 0;
};

QT_BEGIN_NAMESPACE

#define IEditor_iid "org.miroox.mde-1.2-IEditor"
Q_DECLARE_INTERFACE(IEditorPlugin, IEditor_iid)

QT_END_NAMESPACE

#endif // IEDITOR_H
