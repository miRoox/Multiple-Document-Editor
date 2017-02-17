#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <QObject>
#include <QtPlugin>
#include <extensionsystem/ipluginbase.h>

class HelloWorld : public QObject,
                   public IPluginBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.miroox.mde-1.2-IPluginBase" FILE "helloworld.json")
    Q_INTERFACES(IPluginBase)

public:
    bool initialize(GeneralSettings *settings) override;
    void setMDE(MdeWindow * w) override;

private:
    MdeWindow * win;
};

#endif // HELLOWORLD_H
