#include "mdeapplication.h"

MdeApplication::MdeApplication(int &argc, char **argv)
    : QApplication(argc,argv)
{
}

int MdeApplication::run()
{
    int ret = 0;
    ret = exec();
    return ret;
}
