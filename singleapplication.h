#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

/** These code is from "http://www.qtcentre.org/wiki/index.php?title=SingleApplication" **/

#include <QApplication>
#include <QSharedMemory>

QT_BEGIN_NAMESPACE
class QLocalServer;
QT_END_NAMESPACE

class SingleApplication : public QApplication
{
    Q_OBJECT

public:
    SingleApplication(int &argc, char *argv[], const QString uniqueKey);
    ~SingleApplication();

private:
    bool _isRunning;
    QString _uniqueKey;
    QSharedMemory sharedMemory;
    QLocalServer *localServer;
    static const int timeout = 1000;

public:
    bool isRunning() {return _isRunning;}
    bool sendMessage(const QString &message);

public slots:
    void receiveMessage();

signals:
    void messageAvailable(QString message);

};

#endif // SINGLEAPPLICATION_H
