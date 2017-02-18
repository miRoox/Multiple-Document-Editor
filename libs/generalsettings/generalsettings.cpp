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

GeneralSettings::DefDirOption GeneralSettings::defDirOption() const
{
    return p->defDirOption;
}

QString GeneralSettings::defDir() const
{
    return p->defDir;
}

void GeneralSettings::setAutoLog(bool log)
{
    p->autoLog = log;
    emit autoLogChange(log);
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

void GeneralSettings::setDefDirOption(GeneralSettings::DefDirOption option)
{
    p->defDirOption = option;
    emit defDirOptionChanged(option);
}

void GeneralSettings::setDefDir(QString path)
{
    if(p->defDirOption==Custom) {
        p->defDir = path;
        emit defDirChanged(path);
    }
}
