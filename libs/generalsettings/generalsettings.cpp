#include "generalsettings.h"
#include "generalsettings_p.h"
#include <QDebug>

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

QString GeneralSettings::styleSheetFile() const
{
    return p->styleSheetFile;
}

bool GeneralSettings::autoSave() const
{
    return p->autoSave;
}

int GeneralSettings::autoSaveInterval() const
{
    return p->autoSaveInterval;
}

int GeneralSettings::maxHistory() const
{
    return p->maxHistory;
}

GeneralSettings::HistoryOption GeneralSettings::historyOption() const
{
    return p->historyOption;
}

QStringList GeneralSettings::history() const
{
    return p->history;
}

void GeneralSettings::setAutoLog(bool log)
{
    if(p->autoLog != log) {
        p->autoLog = log;
        emit autoLogChange(log);
    }
}

void GeneralSettings::setOverrideLang(QString locale)
{
    if(p->overrideLang != locale) {
        p->overrideLang = locale;
        emit overrideLangChanged(locale);
    }
}

void GeneralSettings::setUiLanguage(QString locale)
{
    if(p->uiLanguage.isEmpty())
        p->uiLanguage = locale;
}

void GeneralSettings::setDefDirOption(GeneralSettings::DefDirOption option)
{
    if(p->defDirOption != option) {
        p->defDirOption = option;
        emit defDirOptionChanged(option);
    }
}

void GeneralSettings::setDefDir(QString path)
{
    if(p->defDirOption == Custom && p->defDir != path) {
        p->defDir = path;
        emit defDirChanged(path);
    }
}

void GeneralSettings::setStyleSheetFile(QString file)
{
    if(p->styleSheetFile != file) {
        p->styleSheetFile = file;
        emit styleSheetFileChanged(file);
    }
}

void GeneralSettings::setAutoSave(bool autoSave)
{
    if(p->autoSave != autoSave) {
        p->autoSave = autoSave;
        emit autoSaveChanged(autoSave);
    }
}

void GeneralSettings::setAutoSaveInterval(int min)
{
    if(min<=0)
        qWarning() << "Auto-save interval is not positive";
    if(p->autoSaveInterval != min && p->autoSave) {
        p->autoSaveInterval = min;
        emit autoSaveIntervalChanged(min);
    }
}

void GeneralSettings::setMaxHistory(int num)
{
    if(p->maxHistory != num) {
        p->maxHistory = num;
        emit maxHistoryChanged(num);
    }
}

void GeneralSettings::setHistoryOption(GeneralSettings::HistoryOption option)
{
    if(p->historyOption != option) {
        p->historyOption = option;
        emit historyOptionChanged(option);
    }
}

void GeneralSettings::addToHistory(QString fileName)
{
    if(!p->history.startsWith(fileName))
        return;
    p->history.removeOne(fileName);
    p->history.prepend(fileName);
    if(p->history.count() > p->maxHistory)
        p->history.removeLast();
    emit historyChange(p->history);
}

bool GeneralSettings::removeFromHistory(QString fileName)
{
    if(p->history.removeOne(fileName)) {
        emit historyChange(p->history);
        return true;
    }
    return false;
}
