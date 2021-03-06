﻿#include "qtsingleapplication.h"
#include <generalsettings/generalsettings.h>
#include <extensionsystem/pluginmanager.h>
#include <mdewidget/mdewindow.h>
#include "mylogger.h"
#include <QTranslator>
#include <QLocale>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QMessageBox>
#include <QLibraryInfo>
#include <QDebug>

const char appId[] = "miroox/Multiple Document Editor";

const char MultInst_OPT[] = "multInst";
const char Pid_OPT[] = "pid";
const char Block_OPT[] = "block";
const char Log_OPT[] = "log";
const char Recus_OPT1[] = "r";
const char Recus_OPT2[] = "recus";

const char divChar = '\n';

QMessageBox::StandardButton askMsgSendFailed();
void initRemoteCmdlParser(QCommandLineParser & parser);
void dealRemoteCmdlOptions(QCommandLineParser & parser, MdeWindow * win);

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Multiple Document Editor");
    QCoreApplication::setApplicationVersion("1.2");
    QCoreApplication::setOrganizationName("miroox");
#ifndef QT_DEBUG
    MyLogger logger;
    logger.installMyMsgHandle();
#endif
    qInfo() << QCoreApplication::applicationName() << "is starting...";
    SharedTools::QtSingleApplication app(QLatin1String(appId),argc, argv);
    qAddPostRoutine([]{
        qInfo() << QCoreApplication::applicationName() << "has quit.";
    });

    GeneralSettings generalSettings;

    QTranslator translator;
    QTranslator qtTranslator;
    qInfo() << "Loading translation..";
    QStringList uiLanguages = QLocale::system().uiLanguages();
    const QString & appTrPath = app.applicationDirPath() + "/../translations";
    const QString & qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QString overrideLanguage = generalSettings.overrideLang();
    if(!overrideLanguage.isEmpty())
        uiLanguages.prepend(overrideLanguage);
    foreach (QString locale, uiLanguages) {
        locale = QLocale(locale).name();
        if(translator.load(QLatin1String("mde_")+locale,appTrPath)) {
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            if(qtTranslator.load(qtTrFile,qtTrPath) ||
               qtTranslator.load(qtTrFile,appTrPath)) {
                app.installTranslator(&translator);
                app.installTranslator(&qtTranslator);
                generalSettings.setUiLanguage(locale);
                break;
            }
            translator.load(QString()); // unload
        } else if (locale == QLatin1String("C") ||
                   locale.startsWith(QLatin1String("en"))) {
            generalSettings.setUiLanguage(QLatin1String("en"));
            break; //english is built-in
        }
    }
    qInfo() << "UI language:" << generalSettings.uiLanguage();

    PluginManager plugManager;
    plugManager.loadGeneralSettings(&generalSettings);
    plugManager.loadPlugins();

    app.setApplicationDisplayName(QApplication::translate("Application",
                                                          "Multiple Document Editor"));

    QCommandLineParser parser;
    qInfo() << "Initializing commandline parser..";
    parser.setApplicationDescription(app.applicationName());
    //Options that require preprocessing
    QCommandLineOption helpOpt = parser.addHelpOption();
    QCommandLineOption versionOpt = parser.addVersionOption();
    parser.addOptions({
          {MultInst_OPT,
           QApplication::translate("CommandLine","Launch another MDE instance.")},
          {Pid_OPT,
           QApplication::translate("CommandLine","Attempt to connect to instance given by <pid>"),
           QLatin1String("pid")},
          {Block_OPT,
           QApplication::translate("CommandLine","Block until editor is closed")},
          {Log_OPT,
           QApplication::translate("CommandLine","Turn the log file writing on initially")}
    });
    //Options that can use in remote arguments
    initRemoteCmdlParser(parser);

    if(!parser.parse(app.arguments()))
        qWarning() << parser.errorText();
#ifndef QT_DEBUG
    logger.confirmLogging(parser.isSet(Log_OPT) || coreSettings.autoLog());
#endif
    if(parser.isSet(helpOpt)) {
        qInfo() << "Show commandline paramter";
        app.removeCurrentPid();
        parser.showHelp();
        return 0;
    }
    if(parser.isSet(versionOpt)) {
        qInfo() << "Show application version:" << app.applicationVersion();
        app.removeCurrentPid();
        parser.showVersion();
        return 0;
    }

    qint64 pid = -1;
    if(parser.isSet(Pid_OPT)) {
        bool available;
        qint64 tmpPid = parser.value(Pid_OPT).toInt(&available);
        if(available && tmpPid>=0) {
            pid = tmpPid;
            qInfo() << "Attempt to connect to another instance with pid:" << pid;
        }
    }
    if(app.isRunning() && !parser.isSet(MultInst_OPT)) {
        app.setBlock(parser.isSet(Block_OPT));
        if(app.sendMessage(app.arguments().join(divChar),1000,pid))
            return 0;

        qWarning() << "Message could not be send, maybe it was in the process of quitting.";
        if(app.isRunning(pid)) {
            // Nah app is still running, ask the user
            QMessageBox::StandardButton button = askMsgSendFailed();
            while (QMessageBox::Retry == button) {
                qDebug() << "Try again..";
                if(app.sendMessage(app.arguments().join(divChar),1000,pid))
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

    plugManager.loadSuffixDescription();

    MdeWindow win(&generalSettings);
    win.installPluginManager(&plugManager);
    win.show();
    app.setActivationWindow(&win);
    QApplication::connect(&app,&SharedTools::QtSingleApplication::messageReceived,
                          [&win](QString args){
        QCommandLineParser remoteParser;
        initRemoteCmdlParser(remoteParser);
        if(!remoteParser.parse(args.split(divChar))) {
            qWarning() << remoteParser.errorText();
        }
        dealRemoteCmdlOptions(remoteParser,&win);
    });
    dealRemoteCmdlOptions(parser,&win);
    return app.exec();
}

void initRemoteCmdlParser(QCommandLineParser &parser)
{
    parser.addOptions({
          {{Recus_OPT1,Recus_OPT2},
           QApplication::translate("CommandLine","Open files recursively."
                                   "This argument will be ignored if [file] contain no wildcard character")},
    });
    parser.addPositionalArgument("files",
                                 QApplication::translate("CommandLine","file path to open"),"[files...]");
}

void dealRemoteCmdlOptions(QCommandLineParser & parser, MdeWindow *win)
{
    if(parser.isSet(Recus_OPT1)) {
        foreach (QString file, parser.positionalArguments())
            win->openFilesRecursively(file);
    }
    else {
        foreach (QString file, parser.positionalArguments())
            win->openFile(file);
    }
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
