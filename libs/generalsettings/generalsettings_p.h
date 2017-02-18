#ifndef GENERALSETTINGS_P_H
#define GENERALSETTINGS_P_H

#include <QObject>

#include "generalsettings.h"

class GeneralSettingsPrivate : public QObject
{
    Q_OBJECT

    explicit GeneralSettingsPrivate(QObject *parent = 0);
    ~GeneralSettingsPrivate();
    void loadSettings();
    void saveSettings();

private:
    bool autoLog;
    QString overrideLang;
    QString uiLanguage;
    GeneralSettings::DefDirOption defDirOption;
    QString defDir;
    friend class GeneralSettings;
};


#endif // GENERALSETTINGS_P_H
