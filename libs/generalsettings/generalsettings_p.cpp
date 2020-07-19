#include "generalsettings_p.h"
#include <QSettings>
#include <QDebug>

GeneralSettingsPrivate::GeneralSettingsPrivate(GeneralSettings *parent)
    : QObject(parent),
      p(parent)
{
    loadSettings();
    connect(parent,&GeneralSettings::maxHistoryChanged,
            this,&GeneralSettingsPrivate::checkHistory);
    connect(this,&GeneralSettingsPrivate::historyChange,
            parent,&GeneralSettings::historyChange);
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
    autoLog = settings.value("autoLog",false).toBool();
    overrideLang = settings.value("overrideLanguage",QString()).toString();
    defDirOption = static_cast<GeneralSettings::DefDirOption>(settings.value("defDirOption",0).toInt());
    defDir = settings.value("defDir",QString(".")).toString();
    styleSheetFile = settings.value("styleSheetFile",QString()).toString();
    autoSave = settings.value("autoSave",false).toBool();
    autoSaveInterval = settings.value("autoSaveInterval",5).toInt();
    maxHistory = settings.value("maxHistory",10).toInt();
    historyOption = static_cast<GeneralSettings::HistoryOption>(settings.value("historyOption",0).toInt());
    history = settings.value("history",QStringList()).toStringList();
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
    settings.setValue("defDirOption",static_cast<int>(defDirOption));
    settings.setValue("defDir",defDir);
    settings.setValue("styleSheetFile",styleSheetFile);
    settings.setValue("autoSave",autoSave);
    settings.setValue("autoSaveInterval",autoSaveInterval);
    settings.setValue("maxHistory",maxHistory);
    settings.setValue("historyOption",static_cast<int>(historyOption));
    settings.setValue("history",history);
    settings.endGroup();
    qInfo() << "General setting is saved.";
}

void GeneralSettingsPrivate::checkHistory()
{
    if(maxHistory<=0)
        return;
    if(history.count() > maxHistory) {
        while (history.count() > maxHistory) {
            history.removeLast();
        }
        emit historyChange(history);
    }
}
