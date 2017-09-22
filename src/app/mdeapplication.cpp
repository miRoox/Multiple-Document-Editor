#include "mdeapplication.h"
#include "extensionsystem/pluginmanager.h"

MdeApplication::MdeApplication(int &argc, char **argv)
    : QApplication(argc,argv)
{
}

int MdeApplication::run()
{
    int ret = 0;
    ExtensionSystem::PluginManager pluginManager;
    pluginManager.instance();
    ret = exec();
    return ret;
}
