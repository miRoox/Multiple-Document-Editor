#include "helloworld.h"
#include <generalsettings/generalsettings.h>
#include <mdewidget/mdewindow.h>
#include <QString>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

bool HelloWorld::initialize(GeneralSettings *settings)
{
    // load translation
    const QString &appTrPath = qApp->applicationDirPath() + "/../translations";
    QTranslator * translator = new QTranslator(this);
    if(translator->load(QLatin1String("helloworld_") + settings->uiLanguage(),appTrPath))
        qApp->installTranslator(translator);
    else
        qWarning() << "loading translation failed";
    // load source code
    src += "\n/** Project File - HelloWorld.pro **/\n";
    QFile proFile(":/src/HelloWorld.pro");
    proFile.open(QIODevice::ReadOnly | QIODevice::Text);
    src += proFile.readAll();
    proFile.close();
    src += "\n\n/** Header File - helloworld.h **/\n";
    QFile hFile(":/src/helloworld.h");
    hFile.open(QIODevice::ReadOnly | QIODevice::Text);
    src += hFile.readAll();
    hFile.close();
    src += "\n\n/** Cpp Source File - helloworld.cpp **/\n";
    QFile cppFile(":/src/helloworld.cpp");
    cppFile.open(QIODevice::ReadOnly | QIODevice::Text);
    src += cppFile.readAll();
    cppFile.close();
    src += "\n\n/** Plugin Meta Data File - helloworld.json **/\n";
    QFile jsonFile(":/src/helloworld.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    src += jsonFile.readAll();
    jsonFile.close();
    src += "\n\n/** Resource File - src.qrc **/\n";
    QFile qrcFile(":/src/src.qrc");
    qrcFile.open(QIODevice::ReadOnly | QIODevice::Text);
    src += qrcFile.readAll();
    qrcFile.close();
    return true;
}

void HelloWorld::setMDE(MdeWindow *w)
{
    win = w;
    win->menuTools()->addAction(tr("Hello World"),[this]{
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(tr("Hello World")+"\t\t\t\t");
        msgBox.setInformativeText(tr("This is an example plugin."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDetailedText(src);
        msgBox.exec();
    });
}
