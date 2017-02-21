#ifndef GENERALSETTINGSWIDGET_H
#define GENERALSETTINGSWIDGET_H

#include <QWidget>

class GeneralSettings;

namespace Ui {
class GeneralSettingsWidget;
}

class GeneralSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsWidget(GeneralSettings *settings, QWidget *parent = 0);
    ~GeneralSettingsWidget();

private:
    void loadAvailbaleLanguage();
    void loadSettings();
    void initConnections();

    void setAutoSave(bool autoSave);
    void loadDefDir();
    void loadHistoryOption();

private slots:
    void apply();
    void warningNeedRestart();
    void setLanguage(QString lang);

private:
    Ui::GeneralSettingsWidget *ui;
    GeneralSettings * genSettings;
    bool manual;
};

#endif // GENERALSETTINGSWIDGET_H
