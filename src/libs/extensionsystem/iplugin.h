#pragma once

#include <QtPlugin>
#include <QObject>

class QString;

namespace ExtensionSystem {

class IPlugin {
public:
    enum InitializeState {
        Failed = 0,
        Successful = 1,
        Finished = 0xffffffff
    };
    virtual InitializeState initialize(PluginManager::ProgramPeriod period) = 0;
    virtual QString errorString() = 0;
    virtual bool attempToShutdown() = 0;
};

#define IPlugin_iid "org.miroox.mde-2.0-IPlugin"
Q_DECLARE_INTERFACE(IPlugin, IPlugin_iid)

}//namespace ExtensionSystem
