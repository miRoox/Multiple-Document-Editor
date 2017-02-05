#ifndef IPLUGINBASE_H
#define IPLUGINBASE_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class MdeWindow;

class IPluginBase
{
public:
    virtual ~IPluginBase() { }
    virtual QString name() const;
    virtual QString version() const;
    virtual QString vendor() const;
    virtual bool install(MdeWindow * win) = 0;
    virtual bool uninstall() = 0;
};

QT_BEGIN_NAMESPACE

#define IPluginBase_iid "org.miroox.mde-1.2-IPluginBase"
Q_DECLARE_INTERFACE(IPluginBase, IPluginBase_iid)

QT_END_NAMESPACE
#endif // IPLUGINBASE_H
