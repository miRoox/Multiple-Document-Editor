#ifndef MDESETTINGS_H
#define MDESETTINGS_H

#include <QObject>

class MdeSettingsPrivate;

class MdeSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoLog READ autoLog WRITE setAutoLog)
    Q_PROPERTY(QString overrideLang READ overrideLang WRITE setOverrideLang NOTIFY overrideLangChanged)
    Q_PROPERTY(QString uiLanguage READ uiLanguage WRITE setUiLanguage)
public:
    explicit MdeSettings(QObject *parent = 0);
    bool autoLog() const;
    QString overrideLang() const;
    QString uiLanguage() const;

signals:
    void overrideLangChanged(QString/*locale*/);

public slots:
    void setAutoLog(bool log);
    void setOverrideLang(QString locale);
    void setUiLanguage(QString locale);

private:
    MdeSettingsPrivate * p;
};

#endif // MDESETTINGS_H
