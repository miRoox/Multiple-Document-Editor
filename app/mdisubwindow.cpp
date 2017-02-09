#include "mdisubwindow.h"
#include "ieditor.h"
#include <QMouseEvent>

MdiSubWindow::MdiSubWindow(QWidget *parent, Qt::WindowFlags flags)
    :QMdiSubWindow(parent,flags)
{
    m_editor=0;
}

MdiSubWindow::~MdiSubWindow()
{

}

IEditor * MdiSubWindow::editor() const
{
    return m_editor;
}

void MdiSubWindow::setEditor(IEditor *editor)
{
    if(!editor || m_editor)
        return;
    m_editor = editor;
    QWidget * widget = m_editor->widget();
    widget->setAttribute(Qt::WA_DeleteOnClose);
    setWidget(widget);
    m_editor->extraInitialize(this);
}

void MdiSubWindow::slotSubWindowActivated(QMdiSubWindow * active)
{
    if(this==active) m_editor->activate();
    else m_editor->deactivate();
}

void MdiSubWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}
