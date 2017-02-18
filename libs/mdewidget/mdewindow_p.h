#ifndef MDEWINDOW_P_H
#define MDEWINDOW_P_H

#include <QObject>
#include "mdewindow.h"

#include <QString>

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

    void installGeneralSettings();
    void initActions();
    void loadSettings();
    void saveSettings();
    MdiSubWindow * findSubWindow(QString fileName);
    QString defaultDir();
    void warningNoEditor(bool noEditor = true);
    void warningOpenFailed(QString fileName);
    void warningSaveFailed(QString fileName);

private:
    MdeWindow * w;
    Ui::MdeWindow *ui;

    GeneralSettings * genSettings;
    PluginManager * plugManager;

    QString lastOperatePath;

    friend class MdeWindow;
};

#endif // MDEWINDOW_P_H
