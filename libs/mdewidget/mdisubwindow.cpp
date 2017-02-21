#include "mdisubwindow.h"
#include "mdisubwindow_p.h"
#include <extensionsystem/ieditor.h>
#include <QMouseEvent>

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
    QWidget * widget = p->editor->widget();
    widget->setAttribute(Qt::WA_DeleteOnClose);
    setWidget(widget);
    p->editor->extraInitialize(this);
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
        event->accept();
    }
    else {
        event->ignore();
    }
}
