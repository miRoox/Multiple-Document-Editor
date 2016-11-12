#include "singleapplication.h"
#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv,"miroox-MDE");
    QApplication::setApplicationDisplayName("多文档编辑器");
    QApplication::setApplicationName(QApplication::translate("main","Multiple Document Editor"));
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file",QApplication::translate("main","The file to open."));
    parser.process(app);

    if(app.isRunning()){
        if(!parser.positionalArguments().isEmpty()){
            app.sendMessage(parser.positionalArguments().at(0));
        }
        else {
            app.sendMessage(QString());
        }
    }
    else {
        MainWindow w;
        QObject::connect(&app,SIGNAL(messageAvailable(QString)),
                         &w,SLOT(openFile(QString)));
        if(!parser.positionalArguments().isEmpty())
        {
            w.openFile(parser.positionalArguments().at(0));
        }
        w.show();

        return app.exec();
    }
    return 0;
}
