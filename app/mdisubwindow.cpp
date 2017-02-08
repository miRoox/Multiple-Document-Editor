#include "mdisubwindow.h"
#include "ieditor.h"
#include <QMouseEvent>

MdiSubWindow::MdiSubWindow(QWidget *parent, Qt::WindowFlags flags)
    :QMdiSubWindow(parent,flags)
{
    editor=0;
}

MdiSubWindow::~MdiSubWindow()
{

}

IEditor * MdiSubWindow::editor()
{
    return editor;
}

void MdiSubWindow::setEditor(IEditor *editor)
{
    if(!editor || this->editor)
        return;
    this->editor = editor;
    QWidget * widget = editor->widget();
    widget->setAttribute(Qt::WA_DeleteOnClose);
    setWidget(widget);
    editor->extraInitialize(this);
}

void MdiSubWindow::subWindowActivated(QMdiSubWindow * active)
{
    if(this==active) editor->activate();
    else editor->deactivate();
}

void MdiSubWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}
