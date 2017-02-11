#include "mdesettings_p.h"
#include <QSettings>
#include <QDebug>

MdeSettingsPrivate::MdeSettingsPrivate(QObject *parent) : QObject(parent)
{
    loadSettings();
}

MdeSettingsPrivate::~MdeSettingsPrivate()
{
    saveSettings();
}

void MdeSettingsPrivate::loadSettings()
{
    QSettings settings;
    settings.beginGroup("General");
    qInfo() << "Loading general settings..";
    autoLog = settings.value("autoLog").toBool();
    overrideLang = settings.value("overrideLanguage").toString();
    settings.endGroup();
    qInfo() << "General setting is loaded.";
}

void MdeSettingsPrivate::saveSettings()
{
    QSettings settings;
    settings.beginGroup("General");
    qInfo() << "Saving general settings..";
    settings.setValue("autoLog",autoLog);
    settings.setValue("overrideLanguage",overrideLang);
    settings.endGroup();
    qInfo() << "General setting is saved.";
}
