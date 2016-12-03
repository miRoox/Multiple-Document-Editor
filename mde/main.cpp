#include "singleapplication.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv,"miroox-MDE");
    QTranslator translator;
    if(translator.load("current.qm","./translations"))
    {
        app.installTranslator(&translator);
    }
    QApplication::setApplicationDisplayName(QApplication::translate("main","多文档编辑器"));
    QApplication::setApplicationName(QApplication::translate("main","Multiple Document Editor"));
    QApplication::setApplicationVersion("1.1");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file",
                                 QApplication::translate("main","The file to open."));
    QCommandLineOption noPluginsOption("noplugin",
                                         QApplication::translate("main","Do not load plugins."));
    parser.addOption(noPluginsOption);
    parser.process(app);

    if(app.isRunning())
    {
        if(!parser.positionalArguments().isEmpty())
        {
            app.sendMessage(parser.positionalArguments().at(0));
        }
        else
        {
            app.sendMessage(QString());
        }
    }
    else
    {
        MainWindow w;
        QObject::connect(&app,SIGNAL(messageAvailable(QString)),
                         &w,SLOT(openFile(QString)));
        w.show();
        if(!parser.isSet(noPluginsOption))
        {
            w.loadPlugins();
        }
        if(!parser.positionalArguments().isEmpty())
        {
            w.openFile(parser.positionalArguments().at(0));
        }

        return app.exec();
    }
    return 0;
}
