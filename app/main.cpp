#include "singleapplication.h"
#include <QStringList>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTextEdit>

void initCmdlParser(QCommandLineParser &parser);

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv,"miroox/MDE");
    if(app.isRunning())
    {
        app.sendMessage(app.arguments().join(' '));
    }
    else
    {
        QCommandLineParser parser;
        initCmdlParser(parser);
        parser.process(app);
        /*QApplication::connect(&app,SingleApplication::messageAvailable,
                             [&parser](QString args){
                                parser.process(QStringList(args));
                             });*/
        QTextEdit w;
        w.show();
        return app.exec();
    }
    return 0;
}

void initCmdlParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(QApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("path",
                                 QApplication::translate("main","The file or folder to open."));
    QCommandLineOption newWinOpt("newwin",
                                 QApplication::translate("main","Open a new window."));
    QCommandLineOption noPluginsOpt("noplugins",
                                    QApplication::translate("main","Do not load plugins."));
    parser.addOption(newWinOpt);
    parser.addOption(noPluginsOpt);
}
