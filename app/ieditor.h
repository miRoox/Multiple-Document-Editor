#ifndef IEDITOR_H
#define IEDITOR_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QWidget;
class QString;
class QFileInfo;
QT_END_NAMESPACE

class MdiSubWindow;

class IEditor
{
public:
    virtual ~IEditor() { }
    virtual QWidget * widget() = 0;//widget that will be added to mdi subwindow
    virtual void extraInitialize(MdiSubWindow *) = 0;//after widget added to mdi subwindow, setParent maybe a good idea
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void newFile() = 0;
    virtual bool loadFile(QString) = 0;
    virtual bool reload() = 0;
    virtual bool save() = 0;
    virtual bool saveAs() = 0;
    virtual QString title() = 0;
    virtual QFileInfo file() = 0;
    /*virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void cut() = 0;
    virtual void copy() = 0;
    virtual void paste() = 0;
    virtual bool isUndoAvailable() = 0;*/
};

class IEditorPlugin
{
public:
    virtual ~IEditorPlugin() { }
    virtual IEditor * create() = 0;
};

QT_BEGIN_NAMESPACE

#define IEditor_iid "org.miroox.mde-1.2-IEditor"
Q_DECLARE_INTERFACE(IEditorPlugin, IEditor_iid)

QT_END_NAMESPACE

#endif // IEDITOR_H
