#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include "generalsettings_global.h"

#include <QObject>

class GeneralSettingsPrivate;

class GENERALSETTINGSSHARED_EXPORT GeneralSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoLog READ autoLog WRITE setAutoLog)
    Q_PROPERTY(QString overrideLang READ overrideLang WRITE setOverrideLang NOTIFY overrideLangChanged)
    Q_PROPERTY(QString uiLanguage READ uiLanguage WRITE setUiLanguage)
public:
    explicit GeneralSettings(QObject *parent = 0);
    bool autoLog() const;
    QString overrideLang() const;
    QString uiLanguage() const;

signals:
    void overrideLangChanged(QString/*locale*/);

public slots:
    void setAutoLog(bool log);//only once
    void setOverrideLang(QString locale);
    void setUiLanguage(QString locale);//only once

private:
    GeneralSettingsPrivate * p;
};

#endif // GENERALSETTINGS_H
