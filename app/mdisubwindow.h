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
    IEditor * editor() const;

public slots:
    void setEditor(IEditor * editor);
    void subWindowActivated(QMdiSubWindow*);

protected:
    void mousePressEvent(QMouseEvent * event);//TODO: drag & drop

private:
    IEditor * editor;
};

#endif // MDISUBWINDOW_H
