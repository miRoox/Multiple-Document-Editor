#include "mdisubwindow.h"
#include "mdisubwindow_p.h"
#include <extensionsystem/ieditor.h>
#include <generalsettings/generalsettings.h>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QMouseEvent>
#include <QCloseEvent>

MdiSubWindow::MdiSubWindow(QWidget *parent, Qt::WindowFlags flags)
    :QMdiSubWindow(parent,flags)
{
    p = new MdiSubWindowPrivate(this);
}

MdiSubWindow::~MdiSubWindow()
{

}

IEditor * MdiSubWindow::editor() const
{
    return p->editor;
}

void MdiSubWindow::setEditor(IEditor *editor)
{
    if(!editor || p->editor)
        return;
    p->editor = editor;

    QTimer * autoSaveTimer = new QTimer(this);
#define min 60*1000/*msec*/
    connect(p->genSettings,GeneralSettings::autoSaveChanged,
            autoSaveTimer,[autoSaveTimer,this](bool sav){
        if(sav) connect(autoSaveTimer,QTimer::timeout,p,MdiSubWindowPrivate::save);
        else disconnect(autoSaveTimer,QTimer::timeout,p,MdiSubWindowPrivate::save);
    });
    connect(p->genSettings,GeneralSettings::autoSaveIntervalChanged,
            autoSaveTimer,[autoSaveTimer](int tmin){
        autoSaveTimer->setInterval(tmin * min);
    });
    if(p->genSettings->autoSave()) {
        connect(autoSaveTimer,QTimer::timeout,p,MdiSubWindowPrivate::save);
        autoSaveTimer->start(p->genSettings->autoSaveInterval() * min);
    }

    QWidget * widget = p->editor->widget();
    widget->setAttribute(Qt::WA_DeleteOnClose);
    setWidget(widget);
    p->editor->extraInitialize(this);
}

void MdiSubWindow::loadGenSettings(GeneralSettings *settings)
{
    if(!p->genSettings && settings) {
        p->genSettings = settings;
    }
}

void MdiSubWindow::slotSubWindowActivated(QMdiSubWindow * active)
{
    if(this==active) p->editor->activate();
    else p->editor->deactivate();
}

void MdiSubWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void MdiSubWindow::closeEvent(QCloseEvent *event)
{
    if(p->editor->canBeClosed()) {
        emit closedFile(QDir::toNativeSeparators(p->editor->file().canonicalFilePath()));
        event->accept();
    }
    else {
        event->ignore();
    }
}
