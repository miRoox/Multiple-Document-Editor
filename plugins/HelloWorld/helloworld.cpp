#include "helloworld.h"
#include <generalsettings/generalsettings.h>
#include <mdewidget/mdewindow.h>
#include <QString>
#include <QTranslator>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

bool HelloWorld::initialize(GeneralSettings *settings)
{
    const QString &appTrPath = qApp->applicationDirPath() + "/../translations";
    QTranslator translator;
    if(translator.load(QLatin1String("helloworld_") + settings->uiLanguage(),appTrPath)) {
        if(qApp->installTranslator(&translator))
            qDebug() << "loading translation successfully";//no effect?
    }
    else
        qWarning() << "loading translation failed";
    return true;
}

void HelloWorld::setMDE(MdeWindow *w)
{
    win = w;
    win->menuTools()->addAction(tr("Hello World"),[this]{
        QMessageBox::about(win,tr("Hello World"),tr("This is a hello world plugin."));
    });
}
