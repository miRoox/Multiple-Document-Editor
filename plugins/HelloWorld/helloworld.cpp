#include "helloworld.h"
#include <generalsettings/generalsettings.h>
#include <mdewidget/mdewindow.h>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QDebug>

bool HelloWorld::initialize(GeneralSettings *settings)
{
    qDebug() << "hello world"
             << settings->uiLanguage();
    return true;
}

void HelloWorld::setMDE(MdeWindow *w)
{
    win = w;
    win->menuTools()->addAction(tr("Hello World"),[this]{
        QMessageBox::about(win,tr("Hello World"),tr("This is a hello world plugin."));
    });
}
