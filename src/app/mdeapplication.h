#pragma once

#include <QApplication>

class MdeApplication final : public QApplication
{
public:
    MdeApplication(int &argc, char **argv);
    ~MdeApplication() {}
    int run();
};
