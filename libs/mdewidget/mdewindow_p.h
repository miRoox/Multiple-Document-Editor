#ifndef MDEWINDOW_P_H
#define MDEWINDOW_P_H

#include <QObject>
#include "mdewindow.h"

#include <QString>

class PluginManager;
class IEditor;

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
    void initPreferrence();
    void addToPreferrence(QString name, QWidget *page);
    bool loadStyleSheet(QString fileName);
    void loadSettings();
    void saveSettings();
    MdiSubWindow * addToSubWindow(IEditor * editor);
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

    QDialog * preferrence;

    QString lastOperatePath;

    friend class MdeWindow;
};

#endif // MDEWINDOW_P_H
