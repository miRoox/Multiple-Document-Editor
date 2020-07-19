#include "mdewindow_p.h"
#include "ui_mdewindow.h"
#include "mdisubwindow.h"
#include "generalsettingswidget.h"
#include <generalsettings/generalsettings.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/ieditor.h>
#include <QAction>
#include <QActionGroup>
#include <QMdiArea>
#include <QApplication>
#include <QKeySequence>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QDebug>

MdeWindowPrivate::MdeWindowPrivate(MdeWindow *parent)
    : QObject(parent),
      w(parent),
      ui(new Ui::MdeWindow)
{
    genSettings = nullptr;
    plugManager = nullptr;
    preferrence = new QDialog(w);
    historyGroup = new QActionGroup(w);
}

MdeWindowPrivate::~MdeWindowPrivate()
{
    delete ui;
}

void MdeWindowPrivate::installGeneralSettings()
{
    // default direction
    connect(w,&MdeWindow::openedFile,this,[this](QString fileName){
        lastOperatePath = QFileInfo(fileName).canonicalPath();
    });
    connect(w,&MdeWindow::savedFile,this,[this](QString fileName){
        lastOperatePath = QFileInfo(fileName).canonicalPath();
    });
    //load style sheet
    loadStyleSheet(genSettings->styleSheetFile());
    connect(genSettings,&GeneralSettings::styleSheetFileChanged,
            this,&MdeWindowPrivate::loadStyleSheet);
    //history
    loadHistory(genSettings->history());
    connect(genSettings,&GeneralSettings::historyChange,
            this,&MdeWindowPrivate::loadHistory);
    loadHistoryOption(genSettings->historyOption());
    connect(genSettings,&GeneralSettings::historyOptionChanged,
            this,&MdeWindowPrivate::loadHistoryOption);
}

void MdeWindowPrivate::initBasicConnection()
{
    connect(w,&MdeWindow::openFailed,this,&MdeWindowPrivate::warningOpenFailed);
    connect(w,&MdeWindow::saveFailed,this,&MdeWindowPrivate::warningSaveFailed);
}

void MdeWindowPrivate::initActions()
{
    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionReload->setShortcut(QKeySequence::Refresh);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    ui->actionClose->setShortcut(QKeySequence::Close);
    ui->actionExit->setShortcut(QKeySequence::Quit);
    ui->actionFullScreen->setShortcut(QKeySequence::FullScreen);
    ui->actionNext->setShortcut(QKeySequence::NextChild);
    ui->actionPrevious->setShortcut(QKeySequence::PreviousChild);
    ui->actionPreferences->setShortcut(QKeySequence::Preferences);
    ui->actionAbout->setShortcut(QKeySequence::HelpContents);
    connect(ui->mdiArea,&QMdiArea::subWindowActivated,[this](QMdiSubWindow * sub){
        bool hasSubWindow = sub!=0;
        ui->actionReload->setEnabled(hasSubWindow);
        ui->actionSave->setEnabled(hasSubWindow);
        ui->actionSaveAs->setEnabled(hasSubWindow);
        ui->actionClose->setEnabled(hasSubWindow);
        ui->actionCloseAll->setEnabled(hasSubWindow);
        ui->actionNext->setEnabled(hasSubWindow);
        ui->actionPrevious->setEnabled(hasSubWindow);
    });
    connect(ui->actionNew,&QAction::triggered,w,&MdeWindow::newDoc);
    connect(ui->actionOpen,&QAction::triggered,w,[w = w]{
        w->openWithDialog(/*selectable = false*/);
    });
    connect(ui->actionOpenWith,&QAction::triggered,w,[w = w]{
        w->openWithDialog(/*selectable = */true);
    });
    connect(ui->actionReload,&QAction::triggered, [mdiArea = ui->mdiArea]{
        QMdiSubWindow * active = mdiArea->activeSubWindow();
        if(active) qobject_cast<MdiSubWindow*>(active)->editor()->reload();
    });
    connect(ui->actionClearRecentHistory,&QAction::triggered,
            genSettings,&GeneralSettings::clearHistory);
    connect(ui->actionOpenAllTheHistoryFiles,&QAction::triggered,[this]{
        foreach (QString file, genSettings->history()) {
            w->openFile(file);
        }
    });
    connect(ui->actionSave,&QAction::triggered,w,&MdeWindow::save);
    connect(ui->actionSaveAs,&QAction::triggered,w,&MdeWindow::saveAs);
    connect(ui->actionClose,&QAction::triggered,ui->mdiArea,&QMdiArea::closeActiveSubWindow);
    connect(ui->actionCloseAll,&QAction::triggered,ui->mdiArea,&QMdiArea::closeAllSubWindows);
    connect(ui->actionExit,&QAction::triggered,qApp,&QApplication::closeAllWindows);
    connect(ui->actionStayOnTop,&QAction::toggled,w,[w = w](bool checked){
        if(checked) w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnTopHint);
        else w->setWindowFlags(w->windowFlags() & ~Qt::WindowStaysOnTopHint);
        w->show();
    });
    connect(ui->actionFullScreen,&QAction::triggered,w,[w = w]{
        w->setWindowState(w->windowState() ^ Qt::WindowFullScreen);
    });
    connect(ui->actionMultInst,&QAction::triggered,[]{
        QProcess::startDetached(QApplication::applicationFilePath(),QStringList("--multInst"));
    });
    connect(ui->actionNext,&QAction::triggered,ui->mdiArea,&QMdiArea::activateNextSubWindow);
    connect(ui->actionPrevious,&QAction::triggered,ui->mdiArea,&QMdiArea::activatePreviousSubWindow);
    connect(ui->actionCmdlParam,&QAction::triggered,[]{
        QProcess::startDetached(QApplication::applicationFilePath(),QStringList("-h"));
    });
    connect(ui->actionAbout,&QAction::triggered,w,[w = w]{
        static const char homepage[] = "https://github.com/miRoox/Multiple-Document-Editor";
        QMessageBox::about(w,tr("About ")+qApp->applicationDisplayName(),
                           tr("<html><body>"
                              "<h3 align=\"center\">%1\t%2</h3>"
                              "<p>Based on Qt %3</p>"
                              "<table border=\"0\"><tr>"
                              "<td>Author: </td>"
                              "<td><a href=\"https://github.com/miRoox\">%4</a></td>"
                              "</tr><tr>"
                              "<td>Homepage: </td>"
                              "<td><a href=\"%5\">%5</a></td>"
                              "</tr></table>"
                              "</body></html>").
                           arg(qApp->applicationDisplayName()).
                           arg(qApp->applicationVersion()).
                           arg(QT_VERSION_STR).
                           arg(qApp->organizationName()).
                           arg(homepage)
                           );
    });
    connect(ui->actionAboutQt,&QAction::triggered,qApp,&QApplication::aboutQt);
}

void MdeWindowPrivate::initPreferrence()
{
    QListWidget * widgetList = new QListWidget(preferrence);
    QStackedWidget * container = new QStackedWidget(preferrence);
    connect(widgetList,&QListWidget::currentRowChanged,
            container,&QStackedWidget::setCurrentIndex);
    connect(widgetList,&QListWidget::currentTextChanged,[this](const QString &name){
        preferrence->setWindowTitle(tr("%1 - Preferrences").arg(name));
    });
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(widgetList);
    layout->addWidget(container);
    layout->setSpacing(10);
    layout->setMargin(20);
    layout->setStretchFactor(widgetList,1);
    layout->setStretchFactor(container,4);
    preferrence->setLayout(layout);
    connect(ui->actionPreferences,&QAction::triggered,preferrence,&QDialog::exec);

    addToPreferrence(tr("General"),new GeneralSettingsWidget(genSettings,preferrence));
}

void MdeWindowPrivate::addToPreferrence(QString name, QWidget *page)
{
    if(!page)
        return;
    auto widgetList = preferrence->findChild<QListWidget *>();
    auto container = preferrence->findChild<QStackedWidget *>();
    widgetList->addItem(name);
    container->addWidget(page);
}

bool MdeWindowPrivate::loadStyleSheet(QString fileName)
{
    if(fileName.isEmpty())
        return true;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)) {
        qWarning() << "Cannot open style sheet file" << fileName;
        QMessageBox::warning(w,tr("Invalid style sheet file"),
                             tr("Cannot open style sheet file %1").arg(fileName));
        return false;
    }
    QTextStream fileText(&file);
    qApp->setStyleSheet(fileText.readAll());
    file.close();
    return true;
}

void MdeWindowPrivate::loadHistory(QStringList history)
{
    foreach (QAction * action, historyGroup->actions()) {
        ui->menuHistory->removeAction(action);
    }
    foreach (QString file, history) {
        QAction * action = ui->menuHistory->addAction(file,[this,file]{
            w->openFile(file);
        });
        action->setActionGroup(historyGroup);
    }
}

void MdeWindowPrivate::loadHistoryOption(GeneralSettings::HistoryOption option)
{

    switch (option) {
    case GeneralSettings::RecentClosed:
        disconnect(w,&MdeWindow::openedFile,
                   genSettings,&GeneralSettings::addToHistory);
        disconnect(w,&MdeWindow::savedFile,
                   genSettings,&GeneralSettings::addToHistory);
        connect(w,&MdeWindow::closedFile,
                genSettings,&GeneralSettings::addToHistory);
        connect(w,&MdeWindow::openedFile,
                genSettings,&GeneralSettings::removeFromHistory);
        break;
    case GeneralSettings::RecentOpened:
        disconnect(w,&MdeWindow::closedFile,
                   genSettings,&GeneralSettings::addToHistory);
        disconnect(w,&MdeWindow::savedFile,
                   genSettings,&GeneralSettings::addToHistory);
        disconnect(w,&MdeWindow::openedFile,
                   genSettings,&GeneralSettings::removeFromHistory);
        connect(w,&MdeWindow::openedFile,
                genSettings,&GeneralSettings::addToHistory);
        break;
    case GeneralSettings::RecentSaved:
        disconnect(w,&MdeWindow::closedFile,
                   genSettings,&GeneralSettings::addToHistory);
        disconnect(w,&MdeWindow::openedFile,
                   genSettings,&GeneralSettings::addToHistory);
        disconnect(w,&MdeWindow::openedFile,
                   genSettings,&GeneralSettings::removeFromHistory);
        connect(w,&MdeWindow::savedFile,
                genSettings,&GeneralSettings::addToHistory);
        break;
    }
}

void MdeWindowPrivate::loadSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    qInfo() << "Main window: loading settings..";
    if(!w->restoreGeometry(settings.value("geometry").toByteArray()))
        qWarning() << "Restore main window's geometry failure!";
    if(!w->restoreState(settings.value("state").toByteArray()))
        qWarning() << "Restore main window's state failure!";
    settings.endGroup();
    qInfo() << "Main window: settings are loaded.";
}

void MdeWindowPrivate::saveSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    qInfo() << "Main window: saving settings..";
    settings.setValue("geometry", w->saveGeometry());
    settings.setValue("state",w->saveState());
    settings.endGroup();
    qInfo() << "Main window: settings are saved.";
}

MdiSubWindow *MdeWindowPrivate::addToSubWindow(IEditor *editor)
{
    MdiSubWindow * tab = new MdiSubWindow(ui->mdiArea);
    tab->setEditor(editor);
    ui->mdiArea->addSubWindow(tab);
    connect(ui->mdiArea,&QMdiArea::subWindowActivated,
            tab,&MdiSubWindow::slotSubWindowActivated);
    return tab;
}

MdiSubWindow *MdeWindowPrivate::findSubWindow(QString fileName)
{
    QFileInfo file(fileName);
    if(file.exists()) {
        QMdiSubWindow * sub;
        foreach (sub, ui->mdiArea->subWindowList()) {
            auto tab = qobject_cast<MdiSubWindow*>(sub);
            if(file==tab->editor()->file()) {
                return tab;
            }
        }
    }
    return nullptr;
}

QString MdeWindowPrivate::defaultDir()
{
    auto option = genSettings->defDirOption();
    switch (option) {
    case GeneralSettings::CurrentFile:
        return QFileInfo(w->currentFile()).absolutePath();
    case GeneralSettings::LastOperate:
        return lastOperatePath;
    case GeneralSettings::Custom:
        return genSettings->defDir();
    }
    return QString(".");
}

void MdeWindowPrivate::warningNoEditor(bool noEditor)
{
    static bool showed = false;
    if(noEditor) {
        qWarning() << "No editor available!";
        if(!showed) {
            QMessageBox::warning(w,tr("No editor availabe!"),
                                 tr("The operation cannot be performed "
                                    "because there is no editor availbale."));
            showed = true;
        }
    }
    else {
        showed = false;
    }
}

void MdeWindowPrivate::warningOpenFailed(QString fileName)
{
    qWarning() << "Cannot open" << fileName;
    if(genSettings->history().contains(fileName)) {
        if(QMessageBox::Yes==QMessageBox::warning(w,tr("Cannot open file %1").arg(fileName),
                                                  tr("Delete the record?"),
                                                  QMessageBox::Yes,QMessageBox::No)) {
            genSettings->removeFromHistory(fileName);
        }
    }
    else {
        QMessageBox::warning(w,tr("Open file failed!"),
                             tr("Cannot open %1 ").arg(fileName));
    }
}

void MdeWindowPrivate::warningSaveFailed(QString fileName)
{
    qWarning() << "Cannot save" << fileName;
    QMessageBox::warning(w,tr("Save file failed!"),
                         tr("Cannot save %1 ").arg(fileName));
}
