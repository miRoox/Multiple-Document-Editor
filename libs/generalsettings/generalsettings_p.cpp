#include "generalsettings_p.h"
#include <QSettings>
#include <QDebug>

GeneralSettingsPrivate::GeneralSettingsPrivate(QObject *parent) : QObject(parent)
{
    loadSettings();
}

GeneralSettingsPrivate::~GeneralSettingsPrivate()
{
    saveSettings();
}

void GeneralSettingsPrivate::loadSettings()
{
    QSettings settings;
    settings.beginGroup("General");
    qInfo() << "Loading general settings..";
    autoLog = settings.value("autoLog").toBool();
    overrideLang = settings.value("overrideLanguage").toString();
    settings.endGroup();
    qInfo() << "General setting is loaded.";
}

void GeneralSettingsPrivate::saveSettings()
{
    QSettings settings;
    settings.beginGroup("General");
    qInfo() << "Saving general settings..";
    settings.setValue("autoLog",autoLog);
    settings.setValue("overrideLanguage",overrideLang);
    settings.endGroup();
    qInfo() << "General setting is saved.";
}
