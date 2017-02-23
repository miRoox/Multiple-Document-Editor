#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include "generalsettings_global.h"

#include <QObject>

class GeneralSettingsPrivate;

class GENERALSETTINGSSHARED_EXPORT GeneralSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoLog READ autoLog WRITE setAutoLog NOTIFY autoLogChange)
    Q_PROPERTY(QString overrideLang READ overrideLang WRITE setOverrideLang NOTIFY overrideLangChanged)
    Q_PROPERTY(QString uiLanguage READ uiLanguage WRITE setUiLanguage)
    Q_PROPERTY(DefDirOption defDirOption READ defDirOption WRITE setDefDirOption NOTIFY defDirOptionChanged)
    Q_PROPERTY(QString defDir READ defDir WRITE setDefDir NOTIFY defDirChanged)//defDirOption==Custom
    Q_PROPERTY(QString styleSheetFile READ styleSheetFile WRITE setStyleSheetFile NOTIFY styleSheetFileChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(int autoSaveInterval READ autoSaveInterval WRITE setAutoSaveInterval NOTIFY autoSaveIntervalChanged)
    Q_PROPERTY(int maxHistory READ maxHistory WRITE setMaxHistory NOTIFY maxHistoryChanged)
    Q_PROPERTY(HistoryOption historyOption READ historyOption WRITE setHistoryOption NOTIFY historyOptionChanged)
    Q_PROPERTY(bool reload READ reload WRITE setReload NOTIFY reloadChanged)

public:
    explicit GeneralSettings(QObject *parent = 0);

    enum DefDirOption {
        CurrentFile,
        LastOperate,
        Custom
    };
    Q_ENUM(DefDirOption)

    enum HistoryOption {
        RecentClosed,
        RecentOpened,
        RecentSaved
    };
    Q_ENUM(HistoryOption)

    bool autoLog() const;
    QString overrideLang() const;
    QString uiLanguage() const;
    DefDirOption defDirOption() const;
    QString defDir() const;
    QString styleSheetFile() const;
    bool autoSave() const;
    int autoSaveInterval() const;
    int maxHistory() const;
    HistoryOption historyOption() const;
    QStringList history() const;
    bool reload() const;

signals:
    void autoLogChange(bool);
    void overrideLangChanged(QString/*locale*/);
    void defDirOptionChanged(DefDirOption);
    void defDirChanged(QString/*path*/);
    void styleSheetFileChanged(QString/*file*/);
    void autoSaveChanged(bool);
    void autoSaveIntervalChanged(int/*min*/);
    void maxHistoryChanged(int/*max*/);
    void historyOptionChanged(HistoryOption);
    void historyChange(QStringList/*history*/);
    void reloadChanged(bool);

public slots:
    void setAutoLog(bool log);
    void setOverrideLang(QString locale);
    void setUiLanguage(QString locale);//only once
    void setDefDirOption(DefDirOption option);
    void setDefDir(QString path);
    void setStyleSheetFile(QString file);
    void setAutoSave(bool autoSave);
    void setAutoSaveInterval(int min);
    void setMaxHistory(int num);
    void setHistoryOption(HistoryOption option);
    void addToHistory(QString fileName);
    bool removeFromHistory(QString fileName);
    void clearHistory();
    void setReload(bool re);

private:
    GeneralSettingsPrivate * p;
};

#endif // GENERALSETTINGS_H
