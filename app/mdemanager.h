#ifndef MDEMANAGER_H
#define MDEMANAGER_H

#include <QObject>
#include <QList>
#include <QCommandLineParser>

class MdeWindow;

class MdeManager : public QObject
{
    Q_OBJECT
public:
    explicit MdeManager(QObject *parent = 0);

signals:

public slots:
    bool createMde(const QStringList &args);
    void closedMde(MdeWindow * w);

private:
    QList<MdeWindow*> mdes;

    void initCmdlParser(QCommandLineParser &parser);
};

#endif // MDEMANAGER_H
