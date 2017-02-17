#ifndef MDISUBWINDOW_P_H
#define MDISUBWINDOW_P_H

#include <QObject>

#include "mdisubwindow.h"

class MdiSubWindowPrivate : public QObject
{
    Q_OBJECT
private:
    explicit MdiSubWindowPrivate(MdiSubWindow *parent = 0);
    ~MdiSubWindowPrivate();

private:
    IEditor * editor;
    friend class MdiSubWindow;
};

#endif // MDISUBWINDOW_P_H
