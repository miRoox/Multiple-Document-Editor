#include "generalsettings.h"
#include "generalsettings_p.h"

GeneralSettings::GeneralSettings(QObject *parent) : QObject(parent)
{
    p = new GeneralSettingsPrivate(this);
}

bool GeneralSettings::autoLog() const
{
    return p->autoLog;
}

QString GeneralSettings::overrideLang() const
{
    return p->overrideLang;
}

QString GeneralSettings::uiLanguage() const
{
    return p->uiLanguage;
}

void GeneralSettings::setAutoLog(bool log)
{
    p->autoLog = log;
}

void GeneralSettings::setOverrideLang(QString locale)
{
    p->overrideLang = locale;
    emit overrideLangChanged(locale);
}

void GeneralSettings::setUiLanguage(QString locale)
{
    if(p->uiLanguage.isEmpty())
        p->uiLanguage = locale;
}
