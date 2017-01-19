#include "singleapplication.h"
#include "mdemanager.h"
#include <QStringList>

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv,"miroox/MDE");
    if(app.isRunning())
    {
        app.sendMessage(app.arguments().join(' '));
    }
    else
    {
        MdeManager manager;
        QApplication::connect(&app,SingleApplication::messageAvailable,
                              [&manager](QString msg){
                                manager.createMde(msg.split(' '));
                              });
        manager.createMde(app.arguments());
        return app.exec();
    }
    return 0;
}
