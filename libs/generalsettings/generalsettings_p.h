#ifndef GENERALSETTINGS_P_H
#define GENERALSETTINGS_P_H

#include <QObject>

#include "generalsettings.h"

class GeneralSettingsPrivate : public QObject
{
    Q_OBJECT

    explicit GeneralSettingsPrivate(GeneralSettings *parent = 0);
    ~GeneralSettingsPrivate();
    void loadSettings();
    void saveSettings();

private slots:
    void checkHistory();

signals:
    void historyChange(QStringList);

private:
    GeneralSettings * p;

    bool autoLog;
    QString overrideLang;
    QString uiLanguage;
    GeneralSettings::DefDirOption defDirOption;
    QString defDir;
    QString styleSheetFile;
    bool autoSave;
    int autoSaveInterval;
    int maxHistory;
    GeneralSettings::HistoryOption historyOption;
    QStringList history;
    bool reload;

    friend class GeneralSettings;
};


#endif // GENERALSETTINGS_P_H
