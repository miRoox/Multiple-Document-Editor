#include "mylogger.h"
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QTextStream>
#include <QString>
#include <QDebug>

QFile * MyLogger::logFile = 0;

MyLogger::MyLogger()
{
    if(!logFile) {
        if(!QDir("./debug").exists()) {
            QDir(".").mkdir("debug");
        }
        //QDir::setCurrent("/debug");
        QString name = QString("./debug/")
                + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")
                +".log";
        logFile = new QFile(name);
        if(!logFile->open(QFile::WriteOnly|QFile::Text))
            qWarning() << logFile->errorString();
        //QDir::setCurrent("..");
    }
}

MyLogger::~MyLogger()
{
    if(logFile) {
        if(logFile->isOpen()) {
            logFile->close();
        }
        delete logFile;
    }
}

void MyLogger::confirmLogging(bool log)
{
    if(!logFile)
        return;
    if(!log) {
        if(logFile->isOpen()) {
            logFile->close();
        }
        logFile->remove();
    }
}

void MyLogger::MyMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(!logFile)
        return;
    if(!logFile->isOpen())
        return;
    QString msgText;
    QString contextStr = QString("File:\"%1\" Line:\"%2\" Function:\"%3\"").
            arg(context.file).arg(context.line).arg(context.function);
    QString currentTime = QDateTime::currentDateTime().toString("HH:mm:ss:zzz");
    switch (type) {
    case QtDebugMsg:
        msgText = QString("[%1]Debug(%3): %2\n").
                arg(currentTime).arg(msg).arg(contextStr);
        break;
    case QtInfoMsg:
        msgText = QString("[%1]Info: %2\n").arg(currentTime).arg(msg);
        break;
    case QtWarningMsg:
        msgText = QString("[%1]Warning(%3): %2\n").
                arg(currentTime).arg(msg).arg(contextStr);
        break;
    case QtCriticalMsg:
        msgText = QString("[%1]Critical(%3): %2\n").
                arg(currentTime).arg(msg).arg(contextStr);
        break;
    case QtFatalMsg:
        msgText = QString("[%1]Fatal(%3): %2\n").
                arg(currentTime).arg(msg).arg(contextStr);
        break;
    }
    QTextStream out(logFile);
    out << msgText;
    logFile->flush();
}
