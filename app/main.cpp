#include "qtsingleapplication.h"
#include "mdewindow.h"
#include <QTranslator>
#include <QLocale>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QMessageBox>
#include <QDebug>

const char appId[] = "miroox/Multiple Document Editor";

const char MultInst_OPT[] = "multInst";
const char Block_OPT[] = "block";
const char Pid_OPT[] = "pid";

QMessageBox::StandardButton askMsgSendFailed();

int main(int argc, char *argv[])
{
    SharedTools::QtSingleApplication app(QLatin1String(appId),argc, argv);
    app.setApplicationName("Multiple Document Editor");
    app.setApplicationVersion("1.2");

    QCommandLineParser parser;
    //Options that require preprocessing
    QCommandLineOption helpOpt = parser.addHelpOption();
    QCommandLineOption versionOpt = parser.addVersionOption();
    parser.addOptions({
          {MultInst_OPT,
           QApplication::translate("CommandLine","Launch another MDE instance.")},
          {Block_OPT,
           QApplication::translate("CommandLine","Block until editor is closed")},
          {Pid_OPT,
           QApplication::translate("CommandLine","Attempt to connect to instance given by <pid>"),
           QLatin1String("pid")}
    });

    parser.parse(app.arguments());
    qDebug() << parser.errorText();
    if(parser.isSet(helpOpt)) {
        parser.showHelp();
        return 0;
    }
    if(parser.isSet(versionOpt)) {
        parser.showVersion();
        return 0;
    }

    qint64 pid = -1;
    if(parser.isSet(Pid_OPT)) {
        bool available;
        qint64 tmpPid = parser.value(Pid_OPT).toInt(&available);
        if(available && tmpPid>=0)
            pid = tmpPid;
    }
    if(app.isRunning() && !parser.isSet(MultInst_OPT)) {
        app.setBlock(parser.isSet(Block_OPT));
        if(app.sendMessage(app.arguments().join(' '),5000,pid))
            return 0;

        // Message could not be send, maybe it was in the process of quitting
        if(app.isRunning(pid)) {
            // Nah app is still running, ask the user
            QMessageBox::StandardButton button = askMsgSendFailed();
            while (QMessageBox::Retry == button) {
                if(app.sendMessage(app.arguments().join(' '),5000,pid))
                    return 0;
                if(!app.isRunning(pid)) // App quit while we were trying so start a new instance
                    button = QMessageBox::Yes;
                else
                    button = askMsgSendFailed();
            }
            if(QMessageBox::No == button)
                return -1;
        }
    }

    MdeWindow win;
    app.setActivationWindow(&win);
    win.show();
    return app.exec();
}

QMessageBox::StandardButton askMsgSendFailed()
{
    return QMessageBox::question(0, QApplication::translate("Application","Could not send message"),
                                 QApplication::translate("Application",
                                                         "Unable to send command line arguments to the already running instance. "
                                                         "It appears to be not responding. "
                                                         "Do you want to start a new instance of editor?"),
                                 QMessageBox::Yes | QMessageBox::No | QMessageBox::Retry,
                                 QMessageBox::Retry);
}
