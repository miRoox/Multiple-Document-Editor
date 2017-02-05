#include "mdewindow.h"
#include "ui_mdewindow.h"
#include "pluginmanager.h"
#include "mdisubwindow.h"
#include "ieditor.h"
#include <QStringList>
#include <QLabel>
#include <QStatusBar>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QProcess>
#include <QKeySequence>

MdeWindow::MdeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MdeWindow)
{
    plugManager = new PluginManager(this);
    ui->setupUi(this);
    initActions();
    plugManager->extraInitialize();
    setAcceptDrops(true);
}

MdeWindow::~MdeWindow()
{
    delete ui;
}


MdiSubWindow *MdeWindow::addToSubWindow(IEditor *editor)
{
    QWidget * widget = editor->widget();
    widget->setAttribute(Qt::WA_DeleteOnClose);
    MdiSubWindow * tab = new MdiSubWindow(ui->mdiArea);
    tab->setWidget(widget);
    ui->mdiArea->addSubWindow(tab);
    connect(ui->mdiArea,QMdiArea::subWindowActivated,tab,
            [&tab,&editor](QMdiSubWindow * active){
        if(active==tab) editor->activate();
        else editor->deactivate();
    });//lambda make it easier
    return tab;
}

void MdeWindow::newDoc()
{
    static quint64 sequence = 1;
    IEditor * editor = plugManager->defaultEditor();
    if(!editor)
        return;
    MdiSubWindow * tab = addToSubWindow(editor);
    tab->setWindowTitle(tr("Untitled %1").arg(sequence++));
    editor->extraInitialize(tab);
    editor->newFile();
}

void MdeWindow::initActions()
{
    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionReload->setShortcut(QKeySequence::Refresh);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    ui->actionClose->setShortcut(QKeySequence::Close);
    ui->actionExit->setShortcut(QKeySequence::Quit);
    /*ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionCut->setShortcut(QKeySequence::Cut);
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionPaste->setShortcut(QKeySequence::Paste);*/
    ui->actionNext->setShortcut(QKeySequence::NextChild);
    ui->actionPrevious->setShortcut(QKeySequence::PreviousChild);
    ui->actionPreferences->setShortcut(QKeySequence::Preferences);
    ui->actionAbout->setShortcut(QKeySequence::HelpContents);
    connect(ui->actionClose,QAction::triggered,ui->mdiArea,QMdiArea::closeActiveSubWindow);
    connect(ui->actionCloseAll,QAction::triggered,ui->mdiArea,QMdiArea::closeAllSubWindows);
    connect(ui->actionExit,QAction::triggered,qApp,QApplication::closeAllWindows);
    connect(ui->actionNext,QAction::triggered,ui->mdiArea,QMdiArea::activateNextSubWindow);
    connect(ui->actionPrevious,QAction::triggered,ui->mdiArea,QMdiArea::activatePreviousSubWindow);
    connect(ui->actionCmdlParam,QAction::triggered,
            [](){QProcess::startDetached(QApplication::applicationFilePath(),QStringList("-h"));});
    connect(ui->actionAboutQt,QAction::triggered,qApp,QApplication::aboutQt);
}

QMenu * MdeWindow::menuFile()
{
    return ui->menu_File;
}
QMenu * MdeWindow::menuEdit()
{
    return ui->menu_Edit;
}
QMenu * MdeWindow::menuView()
{
    return ui->menu_View;
}
QMenu * MdeWindow::menuTools()
{
    return ui->menu_Tools;
}
QMenu * MdeWindow::menuSettings()
{
    return ui->menu_Settings;
}
QMenu * MdeWindow::menuHelp()
{
    return ui->menu_Help;
}