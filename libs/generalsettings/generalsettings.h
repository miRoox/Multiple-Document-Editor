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

public:
    explicit GeneralSettings(QObject *parent = 0);

    enum DefDirOption {
        CurrentFile,
        LastOperate,
        Custom
    };
    Q_ENUM(DefDirOption)

    bool autoLog() const;
    QString overrideLang() const;
    QString uiLanguage() const;
    DefDirOption defDirOption() const;
    QString defDir() const;

signals:
    void autoLogChange(bool);
    void overrideLangChanged(QString/*locale*/);
    void defDirOptionChanged(DefDirOption);
    void defDirChanged(QString/*path*/);

public slots:
    void setAutoLog(bool log);
    void setOverrideLang(QString locale);
    void setUiLanguage(QString locale);//only once
    void setDefDirOption(DefDirOption option);
    void setDefDir(QString path);

private:
    GeneralSettingsPrivate * p;
};

#endif // GENERALSETTINGS_H
