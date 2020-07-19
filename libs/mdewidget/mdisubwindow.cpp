#include "mdisubwindow.h"
#include "mdisubwindow_p.h"
#include <extensionsystem/ieditor.h>
#include <generalsettings/generalsettings.h>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
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

#define _min *60*1000/*msec*/
    QTimer * autoSaveTimer = new QTimer(this);
    connect(p->genSettings,&GeneralSettings::autoSaveChanged,
            autoSaveTimer,[autoSaveTimer,this](bool sav){
        if(sav) connect(autoSaveTimer,&QTimer::timeout,p,&MdiSubWindowPrivate::autoSave);
        else disconnect(autoSaveTimer,&QTimer::timeout,p,&MdiSubWindowPrivate::autoSave);
    });
    connect(p->genSettings,&GeneralSettings::autoSaveIntervalChanged,
            autoSaveTimer,[autoSaveTimer](int tmin){
        autoSaveTimer->setInterval(tmin _min);
    });
    if(p->genSettings->autoSave()) {
        connect(autoSaveTimer,&QTimer::timeout,p,&MdiSubWindowPrivate::autoSave);
        autoSaveTimer->start(p->genSettings->autoSaveInterval() _min);
    }
#undef _min

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

bool MdiSubWindow::save()
{
    QFileInfo file = p->editor->file();
    if(file.exists()) {
        p->editor->save();
    }
    else {
        return saveAs();
    }
    return true;
}

bool MdiSubWindow::saveAs()
{
    QStringList suffixes = p->editor->base()->designedTypes();
    QString filter;
    foreach (QString suffix, suffixes) {
        filter += p->editor->base()->typeDescription(suffix);
        filter += suffix;
        filter += ";;";
    }
    filter.chop(2);
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save as"),
                                                    p->genSettings->defDir(),filter);
    if(fileName.isEmpty())
        return false;
    p->editor->saveAs(fileName);
    return true;
}

void MdiSubWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void MdiSubWindow::closeEvent(QCloseEvent *event)
{
    if(p->editor->close()) {
        event->accept();
    }
    else {
        event->ignore();
    }
}
