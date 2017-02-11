#ifndef MDESETTINGS_P_H
#define MDESETTINGS_P_H

#include <QObject>

#include "mdesettings.h"

class MdeSettingsPrivate : public QObject
{
    Q_OBJECT

    explicit MdeSettingsPrivate(QObject *parent = 0);
    ~MdeSettingsPrivate();
    void loadSettings();
    void saveSettings();

private:
    bool autoLog;
    QString overrideLang;
    QString uiLanguage;
    friend class MdeSettings;
};

#endif // MDESETTINGS_P_H
