#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <qglobal.h>

namespace ExtensionSystem {

using namespace Internal;

static PluginManagerPrivate* d = nullptr;
static PluginManager* m_instance = nullptr;

/*!
    Gets the unique plugin manager instance.
*/
PluginManager* PluginManager::instance()
{
    Q_ASSERT(m_instance);
    return m_instance;
}

/*!
    \internal
*/
PluginManager::PluginManager()
{
    Q_ASSERT(!m_instance);
    m_instance = this;
    d = new PluginManagerPrivate(this);
}

/*!
    \internal
*/
PluginManager::~PluginManager()
{
    delete d;
    d = 0;
}


namespace Internal {

/*!
    \internal
*/
PluginManagerPrivate::PluginManagerPrivate(PluginManager* pluginManager)
    : QObject(pluginManager)
{
    q = pluginManager;
}

/*!
    \internal
*/
PluginManagerPrivate::~PluginManagerPrivate()
{
}

}//namespace ExtensionSystem::Internal

}//namespace ExtensionSystem
