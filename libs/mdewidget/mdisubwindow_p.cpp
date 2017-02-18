#include "mdisubwindow_p.h"

MdiSubWindowPrivate::MdiSubWindowPrivate(MdiSubWindow *parent)
    : QObject(parent)
{
    editor = nullptr;
}

MdiSubWindowPrivate::~MdiSubWindowPrivate()
{

}
