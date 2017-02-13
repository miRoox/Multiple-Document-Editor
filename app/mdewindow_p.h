#ifndef MDEWINDOW_P_H
#define MDEWINDOW_P_H

#include <QObject>
#include "mdewindow.h"

QT_BEGIN_NAMESPACE
class QSplitter;
QT_END_NAMESPACE

class PluginManager;

namespace Ui {
class MdeWindow;
}

class MdeWindowPrivate : public QObject
{
    Q_OBJECT
private:
    explicit MdeWindowPrivate(MdeWindow *parent = 0);
    ~MdeWindowPrivate();

    void initActions();
    void loadSettings();
    void saveSettings();
    MdiSubWindow * findSubWindow(QString fileName);
    void warningNoEditor(bool noEditor = true);

private:
    MdeWindow * w;
    Ui::MdeWindow *ui;

    PluginManager * plugManager;
    friend class MdeWindow;
};

#endif // MDEWINDOW_P_H
