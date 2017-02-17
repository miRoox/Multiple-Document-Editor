#include "mdewindow_p.h"
#include "ui_mdewindow.h"
#include "mdisubwindow.h"
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/ieditor.h>
#include <QAction>
#include <QMdiArea>
#include <QApplication>
#include <QKeySequence>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

MdeWindowPrivate::MdeWindowPrivate(MdeWindow *parent)
    : QObject(parent),
      w(parent),
      ui(new Ui::MdeWindow)
{
    plugManager = 0;
}

MdeWindowPrivate::~MdeWindowPrivate()
{
    delete ui;
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
    connect(ui->mdiArea,QMdiArea::subWindowActivated,[&](QMdiSubWindow * sub){
        bool hasSubWindow = sub!=0;
        ui->actionReload->setEnabled(hasSubWindow);
        ui->actionSave->setEnabled(hasSubWindow);
        ui->actionSaveAs->setEnabled(hasSubWindow);
        ui->actionClose->setEnabled(hasSubWindow);
        ui->actionCloseAll->setEnabled(hasSubWindow);
        ui->actionNext->setEnabled(hasSubWindow);
        ui->actionPrevious->setEnabled(hasSubWindow);
    });
    connect(ui->actionNew,QAction::triggered,w,MdeWindow::newDoc);
    connect(ui->actionOpen,QAction::triggered,w,[this]{
        w->openWithDialog(/*selectable = false*/);
    });
    connect(ui->actionOpenWith,QAction::triggered,w,[this]{
        w->openWithDialog(/*selectable = */true);
    });
    connect(ui->actionReload,QAction::triggered, [this]{
        QMdiSubWindow * active = ui->mdiArea->activeSubWindow();
        if(active) qobject_cast<MdiSubWindow*>(active)->editor()->reload();
    });
    connect(ui->actionClose,QAction::triggered,ui->mdiArea,QMdiArea::closeActiveSubWindow);
    connect(ui->actionCloseAll,QAction::triggered,ui->mdiArea,QMdiArea::closeAllSubWindows);
    connect(ui->actionExit,QAction::triggered,qApp,QApplication::closeAllWindows);
    connect(ui->actionStayOnTop,QAction::toggled,[this](bool checked){
        if(checked) w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnTopHint);
        else w->setWindowFlags(w->windowFlags() & ~Qt::WindowStaysOnTopHint);
        w->show();
    });
    connect(ui->actionFullScreen,QAction::triggered,[this]{
        w->setWindowState(w->windowState() ^ Qt::WindowFullScreen);
    });
    connect(ui->actionMultInst,QAction::triggered,[]{
        QProcess::startDetached(QApplication::applicationFilePath(),QStringList("--multInst"));
    });
    connect(ui->actionNext,QAction::triggered,ui->mdiArea,QMdiArea::activateNextSubWindow);
    connect(ui->actionPrevious,QAction::triggered,ui->mdiArea,QMdiArea::activatePreviousSubWindow);
    connect(ui->actionCmdlParam,QAction::triggered,[]{
        QProcess::startDetached(QApplication::applicationFilePath(),QStringList("-h"));
    });
    connect(ui->actionAboutQt,QAction::triggered,qApp,QApplication::aboutQt);
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
    return 0;
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
