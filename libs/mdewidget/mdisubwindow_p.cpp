#include "mdisubwindow_p.h"

MdiSubWindowPrivate::MdiSubWindowPrivate(MdiSubWindow *parent)
    : QObject(parent)
{
    editor = 0;
}

MdiSubWindowPrivate::~MdiSubWindowPrivate()
{

}
