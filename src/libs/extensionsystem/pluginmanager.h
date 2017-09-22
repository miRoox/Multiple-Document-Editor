#pragma once

#include "extensionsystem_global.h"

#include <QObject>

class MdeApplication;

namespace ExtensionSystem {

namespace Internal { class PluginManagerPrivate; }

class EXTENSIONSYSTEM_EXPORT PluginManager final : public QObject
{
    Q_OBJECT
    
public:
    static PluginManager* instance();
    
private:
    PluginManager(); //only MdeApplication can create an instance
    ~PluginManager();
    
    friend class ::MdeApplication;
    friend class Internal::PluginManagerPrivate;
};

}//namespace ExtensionSystem
