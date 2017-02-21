#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <mdewidget/mdewidget_global.h>

#include <QMdiSubWindow>

class MdiSubWindowPrivate;
class IEditor;

class MDEWIDGETSHARED_EXPORT MdiSubWindow
        : public QMdiSubWindow
{
    Q_OBJECT
    Q_PROPERTY(IEditor* editor READ editor WRITE setEditor)

public:
    MdiSubWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());
    ~MdiSubWindow();
    IEditor * editor() const;

public slots:
    void setEditor(IEditor * editor);//only once
    void slotSubWindowActivated(QMdiSubWindow*);

protected:
    void mousePressEvent(QMouseEvent * event);//TODO: drag & drop
    void closeEvent(QCloseEvent * event);

private:
    MdiSubWindowPrivate * p;
};

#endif // MDISUBWINDOW_H
