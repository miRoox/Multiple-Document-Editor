#include "singleapplication.h"

#include <QLocalServer>
#include <QLocalSocket>

SingleApplication::SingleApplication(int &argc, char *argv[], const QString uniqueKey)
    : QApplication(argc, argv), _uniqueKey(uniqueKey)
{
    // sharedMemory 来获取是否已有实例化
    sharedMemory.setKey(_uniqueKey);
    if (sharedMemory.attach())
        _isRunning = true;
    else
    {
        _isRunning = false;
        // create shared memory.
        if (!sharedMemory.create(1))
        {
            qDebug("Unable to create single instance.");
            return;
        }

        // localServer 来与其它实例通信
        localServer = new QLocalServer(this);
        connect(localServer, SIGNAL(newConnection()), this, SLOT(receiveMessage()));
        localServer->listen(_uniqueKey);
    }
}

SingleApplication::~SingleApplication()
{

}

/** 公有方法 **/

// 发送信息
bool SingleApplication::sendMessage(const QString &message)
{
    if (!_isRunning)
        return false;
    QLocalSocket localSocket(this);
    localSocket.connectToServer(_uniqueKey, QIODevice::WriteOnly);
    if (!localSocket.waitForConnected(timeout))
    {
        qDebug(localSocket.errorString().toLatin1());
        return false;
    }

    localSocket.write(message.toUtf8());
    if (!localSocket.waitForBytesWritten(timeout))
    {
        qDebug(localSocket.errorString().toLatin1());
        return false;
    }

    localSocket.disconnectFromServer();
    return true;
}

/** 公有槽 **/

// 获取连接后接收信息
void SingleApplication::receiveMessage()
{
    QLocalSocket *localSocket = localServer->nextPendingConnection();
    if (!localSocket->waitForReadyRead(timeout))
    {
        qDebug(localSocket->errorString().toLatin1());
        return;
    }

    QByteArray byteArray = localSocket->readAll();
    QString message = QString::fromUtf8(byteArray.constData());
    emit messageAvailable(message);
    localSocket->disconnectFromServer();
}
