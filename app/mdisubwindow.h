#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <QMdiSubWindow>

class MdiSubWindowPrivate;
class IEditor;

class MdiSubWindow : public QMdiSubWindow
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

private:
    MdiSubWindowPrivate * p;
};

#endif // MDISUBWINDOW_H
