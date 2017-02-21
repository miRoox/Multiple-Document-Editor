#include "generalsettingswidget.h"
#include "ui_generalsettingswidget.h"
#include <generalsettings/generalsettings.h>
#include <QString>
#include <QStringList>
#include <QLocale>
#include <QLibraryInfo>
#include <QTranslator>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QPushButton>

GeneralSettingsWidget::GeneralSettingsWidget(GeneralSettings * settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettingsWidget),
    genSettings(settings),
    manual(false)
{
    ui->setupUi(this);
    loadAvailbaleLanguage();
    initConnections();
    loadSettings();
}

GeneralSettingsWidget::~GeneralSettingsWidget()
{
    delete ui;
}

void GeneralSettingsWidget::loadAvailbaleLanguage()
{
    QTranslator translator;
    QTranslator qtTranslator;
    QStringList uiLanguages = QLocale::system().uiLanguages();
    const QString & appTrPath = qApp->applicationDirPath() + "/../translations";
    const QString & qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    ui->langComboBox->addItem(tr("<System language>"),QString());
    foreach (QString locale, uiLanguages) {
        locale = QLocale(locale).name();
        if(translator.load(QLatin1String("mde_")+locale,appTrPath)) {
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            if(qtTranslator.load(qtTrFile,qtTrPath) ||
               qtTranslator.load(qtTrFile,appTrPath)) {
                ui->langComboBox->addItem(QLocale(locale).nativeLanguageName(),locale);
                continue;
            }
            translator.load(QString()); // unload
        } else if (locale.startsWith(QLatin1String("en"))) {
            ui->langComboBox->addItem(QLocale(locale).nativeLanguageName(),locale);
        }
    }
}

void GeneralSettingsWidget::loadSettings()
{
    manual = false;

    setLanguage(genSettings->overrideLang());
    ui->qssLineEdit->setText(genSettings->styleSheetFile());
    ui->autoLogCheckBox->setChecked(genSettings->autoLog());
    setAutoSave(genSettings->autoSave());
    loadDefDir();
    loadHistoryOption();
    ui->maxHistoryBox->setValue(genSettings->maxHistory());

    manual = true;
}

void GeneralSettingsWidget::initConnections()
{
    //override language
    connect(ui->langComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,GeneralSettingsWidget::warningNeedRestart);
    //style sheet
    connect(ui->qssToolButton,QToolButton::clicked,[this]{
        QString file = QFileDialog::getOpenFileName(this,
                                                    tr("Select style sheet file"),
                                                    "../qss",
                                                    tr("Qt Style Sheet")+"(*.qss)");
        if(!file.isEmpty())
            ui->qssLineEdit->setText(QDir::toNativeSeparators(QFileInfo(file).canonicalFilePath()));
    });
    //auto-log
    connect(ui->autoLogCheckBox,QCheckBox::toggled,
            this,GeneralSettingsWidget::warningNeedRestart);
    //auto-save
    connect(ui->autoSaveCheckBox,QCheckBox::toggled,
            ui->autoSaveLabel,QLabel::setEnabled);
    connect(ui->autoSaveCheckBox,QCheckBox::toggled,
            ui->autoSaveSpinBox,QSpinBox::setEnabled);
    //default directory
    connect(ui->customButton,QRadioButton::toggled,
            ui->defDirLineEdit,QLineEdit::setEnabled);
    connect(ui->customButton,QRadioButton::toggled,
            ui->defDirToolButton,QToolButton::setEnabled);
    connect(ui->defDirToolButton,QToolButton::clicked,[this]{
        QString dir = QFileDialog::getExistingDirectory(this,
                                                        tr("Select directory"),
                                                        ".",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if(!dir.isEmpty())
            ui->defDirLineEdit->setText(QDir::toNativeSeparators(QFileInfo(dir).canonicalPath()));
    });
    //history option
    //max history
    //buttons
    connect(ui->buttonBox->button(QDialogButtonBox::Apply),QPushButton::clicked,
            this,GeneralSettingsWidget::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset),QPushButton::clicked,
            this,GeneralSettingsWidget::loadSettings);
}

void GeneralSettingsWidget::apply()
{
    genSettings->setOverrideLang(ui->langComboBox->currentData().toString());
    genSettings->setStyleSheetFile(ui->qssLineEdit->text());
    genSettings->setAutoLog(ui->autoLogCheckBox->isChecked());

    genSettings->setAutoSave(ui->autoSaveCheckBox->isChecked());
    genSettings->setAutoSaveInterval(ui->autoSaveSpinBox->value());

    if(ui->currentFileButton->isChecked()) {
        genSettings->setDefDirOption(GeneralSettings::CurrentFile);
    } else if (ui->lastOperateButton->isChecked()) {
        genSettings->setDefDirOption(GeneralSettings::LastOperate);
    } else {
        genSettings->setDefDirOption(GeneralSettings::CurrentFile);
        genSettings->setDefDir(ui->defDirLineEdit->text());
    }

    if(ui->recentClosedButton->isChecked()) {
        genSettings->setHistoryOption(GeneralSettings::RecentClosed);
    } else if (ui->recentOpenedButton->isChecked()) {
        genSettings->setHistoryOption(GeneralSettings::RecentOpened);
    } else {
        genSettings->setHistoryOption(GeneralSettings::RecentSaved);
    }

    genSettings->setMaxHistory(ui->maxHistoryBox->value());
}

void GeneralSettingsWidget::warningNeedRestart()
{
    if(manual)
        QMessageBox::warning(this,tr("Need restart"),
                             tr("This option need restart to take effect"));
}

void GeneralSettingsWidget::setLanguage(QString lang)
{
    int index;
    if(lang.isEmpty())
        index = 0;
    else
        index = ui->langComboBox->findData(lang);
    ui->langComboBox->setCurrentIndex(index);
}

void GeneralSettingsWidget::setAutoSave(bool autoSave)
{
    ui->autoSaveCheckBox->setChecked(autoSave);
    if(autoSave) {
        ui->autoSaveSpinBox->setValue(genSettings->autoSaveInterval());
    }
}

void GeneralSettingsWidget::loadDefDir()
{
    GeneralSettings::DefDirOption option = genSettings->defDirOption();
    switch (option) {
    case GeneralSettings::CurrentFile:
        ui->currentFileButton->setChecked(true);
        break;
    case GeneralSettings::LastOperate:
        ui->lastOperateButton->setChecked(true);
        break;
    case GeneralSettings::Custom:
        ui->customButton->setChecked(true);
        ui->defDirLineEdit->setText(genSettings->defDir());
        break;
    }
}

void GeneralSettingsWidget::loadHistoryOption()
{
    GeneralSettings::HistoryOption option = genSettings->historyOption();
    switch (option) {
    case GeneralSettings::RecentClosed:
        ui->recentClosedButton->setChecked(true);
        break;
    case GeneralSettings::RecentOpened:
        ui->recentOpenedButton->setChecked(true);
        break;
    case GeneralSettings::RecentSaved:
        ui->recentSavedButton->setChecked(true);
        break;
    }
}
