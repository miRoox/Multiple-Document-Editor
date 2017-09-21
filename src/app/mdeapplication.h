#pragma once

#include <QApplication>

class MdeApplication final : public QApplication
{
public:
    MdeApplication(int &argc, char **argv);
    int run();
};
