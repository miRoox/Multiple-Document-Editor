#ifndef IPLUGINBASE_H
#define IPLUGINBASE_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class GeneralSettings;
class MdeWindow;

class IPluginBase
{
public:
    virtual ~IPluginBase() { }
    virtual bool initialize(GeneralSettings * settings) = 0;
    virtual void setMDE(MdeWindow * w) = 0;
};

#define PLUGINMETADATA_NAME     "name"
#define PLUGINMETADATA_VER      "version"
#define PLUGINMETADATA_VENDOR   "vendor"
#define PLUGINMETADATA_PLATFORM "platforms"

QT_BEGIN_NAMESPACE

#define IPluginBase_iid "org.miroox.mde-1.2-IPluginBase"
Q_DECLARE_INTERFACE(IPluginBase, IPluginBase_iid)

QT_END_NAMESPACE
#endif // IPLUGINBASE_H
