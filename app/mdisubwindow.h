#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <QMdiSubWindow>

class IEditor;

class MdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    MdiSubWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());
    ~MdiSubWindow();

protected:
    void mousePressEvent(QMouseEvent * event);//TODO: drag & drop
};

#endif // MDISUBWINDOW_H
