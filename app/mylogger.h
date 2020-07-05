#ifndef MYLOGGER_H
#define MYLOGGER_H

#include <QtGlobal>

class QFile;

class MyLogger
{
public:
    MyLogger();
    ~MyLogger();
    static void installMyMsgHandle();
    static void confirmLogging(bool log);
private:
    static void myMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static QFile * logFile;
};

#endif // MYLOGGER_H
