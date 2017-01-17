#ifndef INTERFACE_H
#define INTERFACE_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
template<typename T> class QVector<T>;
class QMainWindow;
class QString;
class QDir;
class QTextDocument;
QT_END_NAMESPACE

class DataInterface
{
public:
    struct FileData
    {
        QDir dir;
        QString name;
        QTextDocument * doc;
    };
    virtual FileData currentFile() = 0;
    virtual QVector<FileData> files() = 0;
};

class PluginInferface
{
public:
    virtual ~PluginInferface() { }
    virtual bool install(QMainWindow * win, DataInterface * dat) = 0;
    virtual bool uninstall() = 0;
};

QT_BEGIN_NAMESPACE

#define PluginInferface_iid "org.miroox.mde-1.2-PluginInferface"
Q_DECLARE_INTERFACE(PluginInferface, PluginInferface_iid)

QT_END_NAMESPACE
#endif // INTERFACE_H
