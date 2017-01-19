#include "mdemanager.h"
#include "mdewindow.h"
#include <QStringList>
#include <QCommandLineOption>
#include <QDebug>

MdeManager::MdeManager(QObject *parent)
    : QObject(parent) , mdes()
{
    ;
}


void MdeManager::initCmdlParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(qApp->applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("path",
                                 tr("The file or folder to open."));
    QCommandLineOption newWinOpt("newwin",
                                 tr("Open a new window."));
    QCommandLineOption noPluginsOpt("noplugins",
                                    tr("Do not load plugins."));
    parser.addOption(newWinOpt);
    parser.addOption(noPluginsOpt);
}

bool MdeManager::createMde(const QStringList &args)
{
    QCommandLineParser parser;
    initCmdlParser(parser);
    parser.parse(args);
    qDebug() << args;
    if(parser.isSet("newwin") || mdes.isEmpty())
    {
        MdeWindow *mde = new MdeWindow;
        mdes.append(mde);
        connect(mde,MdeWindow::closed,
                this,MdeManager::closedMde);
        mde->show();
        return true;
    }
    else
    {
        return false;
    }
}

void MdeManager::closedMde(MdeWindow * w)
{
    mdes.removeOne(w);
    if(mdes.isEmpty())
        qApp->quit();
}
