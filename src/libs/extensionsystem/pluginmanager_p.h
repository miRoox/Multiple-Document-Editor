#pragma once

#include <QObject>

namespace ExtensionSystem {

class PluginManager;

namespace Internal {

class PluginManagerPrivate final : QObject
{
    Q_OBJECT
    
public:
    PluginManagerPrivate(PluginManager* pluginManager);
    ~PluginManagerPrivate();
    
private:
    PluginManager* q;
};

}//namespace ExtensionSystem::Internal

}//namespace ExtensionSystem
