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
    virtual IEditorPlugin * base() = 0;
    virtual QWidget * widget() = 0;//widget that will be added to mdi subwindow
    virtual void extraInitialize(MdiSubWindow *tab) = 0;//after widget added to mdi subwindow, setParent maybe a good idea
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void newFile() = 0;
    virtual bool loadFile(QString fileName) = 0;
    virtual bool reload() = 0;
    virtual QString save(bool saveAs) = 0;
    virtual QString title() = 0;
    virtual QFileInfo file() = 0;
    virtual bool canBeClosed() = 0;
};

class IEditorPlugin
{
public:
    virtual ~IEditorPlugin() { }
    virtual IEditor * create() = 0;
    virtual QStringList designedTypes() const = 0;
    virtual QString typeDescription(QString) const = 0;
};

QT_BEGIN_NAMESPACE

#define IEditor_iid "org.miroox.mde-1.2-IEditor"
Q_DECLARE_INTERFACE(IEditorPlugin, IEditor_iid)

QT_END_NAMESPACE

#endif // IEDITOR_H
