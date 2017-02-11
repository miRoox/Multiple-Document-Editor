#include "mdesettings.h"
#include "mdesettings_p.h"

MdeSettings::MdeSettings(QObject *parent) : QObject(parent)
{
    p = new MdeSettingsPrivate(this);
}

bool MdeSettings::autoLog() const
{
    return p->autoLog;
}

QString MdeSettings::overrideLang() const
{
    return p->overrideLang;
}

QString MdeSettings::uiLanguage() const
{
    return p->uiLanguage;
}

void MdeSettings::setAutoLog(bool log)
{
    p->autoLog = log;
}

void MdeSettings::setOverrideLang(QString locale)
{
    p->overrideLang = locale;
    emit overrideLangChanged(locale);
}

void MdeSettings::setUiLanguage(QString locale)
{
    if(p->uiLanguage.isEmpty())
        p->uiLanguage = locale;
}

