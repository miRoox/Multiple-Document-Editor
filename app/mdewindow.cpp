#include "mdewindow.h"
#include "ui_mdewindow.h"
#include "pluginmanager.h"
#include "mdisubwindow.h"
#include "ieditor.h"
#include <QStringList>
#include <QLabel>
#include <QStatusBar>
#include <QMdiArea>
#include <QProcess>
#include <QKeySequence>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MdeWindow::MdeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MdeWindow)
{
    plugManager = 0;
    qInfo() << "Set up User Interface";
    ui->setupUi(this);
    initActions();
    loadSettings();
    setAcceptDrops(true);
    connect(ui->mdiArea,QMdiArea::subWindowActivated,[this](QMdiSubWindow * active){
            if(active) setWindowTitle(active->windowTitle());
            else setWindowTitle("");
    });
}

MdeWindow::~MdeWindow()
{
    delete ui;
}

void MdeWindow::setPluginManager(PluginManager *pm)
{
    plugManager = pm;
}


MdiSubWindow *MdeWindow::addToSubWindow(IEditor *editor)
{
    MdiSubWindow * tab = new MdiSubWindow(ui->mdiArea);
    tab->setEditor(editor);
    ui->mdiArea->addSubWindow(tab);
    connect(ui->mdiArea,QMdiArea::subWindowActivated,
            tab,MdiSubWindow::slotSubWindowActivated);
    return tab;
}

void MdeWindow::newDoc()
{
    static quint64 sequence = 1;
    qInfo() << "Attempt to create a new document";
    IEditor * editor = plugManager->defaultEditor();
    if(!editor)
        return warningNoEditor();
    MdiSubWindow * tab = addToSubWindow(editor);
    tab->setEditor(editor);
    editor->newFile();
    tab->setWindowTitle(tr("Untitled %1").arg(sequence++));
    tab->show();
    warningNoEditor(false);
    qInfo() << "Create a new document.";
}

bool MdeWindow::openFile(QString fileName)
{
    if(fileName.isEmpty())
        return false;
    qInfo() << "Attempt to open file:" << fileName;
    MdiSubWindow * tab = findSubWindow(fileName);
    if(tab) {
        ui->mdiArea->setActiveSubWindow(tab);
    }
    else {
        IEditor * editor = plugManager->editor(fileName);
        if(!editor)
            editor = plugManager->defaultBrowser();
        if(!editor) {
            warningNoEditor();
            return false;
        }
        tab = addToSubWindow(editor);
        editor->loadFile(fileName);
        tab->setWindowTitle(editor->title());
        tab->show();
        warningNoEditor(false);
    }
    qInfo() << "Open file:"
            << QDir::toNativeSeparators(QFileInfo(fileName).canonicalFilePath());
    return true;
}

quint32 MdeWindow::openFilesRecursively(QString fileName)
{
    quint32 count = 0;
    QStringList filter(QFileInfo(fileName).fileName());
    QString path = QFileInfo(fileName).absolutePath();
    QDir dir(path);
    qInfo() << "Attempt to open" << fileName << "recursively";
    if(filter.first().isEmpty()) {
        filter[0] = QString("*");
    }
    foreach (QString file, dir.entryList(filter,QDir::Files)) {
        if(openFile(dir.absoluteFilePath(file)))
            ++count;
    }
    if(filter.first().startsWith('*'))
        foreach (QString sdir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            dir.cd(sdir);
            count += openFilesRecursively(dir.path() + "/" + filter.first());
            dir.cdUp();
        }
    return count;
}

void MdeWindow::openWithDialog()
{
    QString filter = plugManager->fileNameFilter();
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Select one or more files to open"),
                                                          ".",
                                                          filter);
    foreach (QString fileName, fileNames) {
        openFile(fileName);
    }
}

QMenu * MdeWindow::menuFile() const
{
    return ui->menu_File;
}
QMenu * MdeWindow::menuEdit() const
{
    return ui->menu_Edit;
}
QMenu * MdeWindow::menuView() const
{
    return ui->menu_View;
}
QMenu * MdeWindow::menuTools() const
{
    return ui->menu_Tools;
}
QMenu * MdeWindow::menuSettings() const
{
    return ui->menu_Settings;
}
QMenu * MdeWindow::menuHelp() const
{
    return ui->menu_Help;
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
    connect(ui->actionNew,QAction::triggered,this,newDoc);
    connect(ui->actionOpen,QAction::triggered,this,openWithDialog);
    connect(ui->actionReload,QAction::triggered, [&]{
        QMdiSubWindow * active = ui->mdiArea->activeSubWindow();
        if(active) qobject_cast<MdiSubWindow*>(active)->editor()->reload();
    });
    connect(ui->actionClose,QAction::triggered,ui->mdiArea,QMdiArea::closeActiveSubWindow);
    connect(ui->actionCloseAll,QAction::triggered,ui->mdiArea,QMdiArea::closeAllSubWindows);
    connect(ui->actionExit,QAction::triggered,qApp,QApplication::closeAllWindows);
    connect(ui->actionStayOnTop,QAction::toggled,[this](bool checked){
        if(checked) setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        show();
    });
    connect(ui->actionFullScreen,QAction::triggered,[this]{
        setWindowState(windowState() ^ Qt::WindowFullScreen);
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

void MdeWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    qInfo() << "Main window: loading settings..";
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
    qInfo() << "Main window: settings are loaded.";
}

void MdeWindow::saveSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    qInfo() << "Main window: saving settings..";
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state",saveState());
    settings.endGroup();
    qInfo() << "Main window: settings are saved.";
}

MdiSubWindow * MdeWindow::findSubWindow(QString fileName)
{
    QFileInfo file(fileName);
    if(file.exists()) {
        QMdiSubWindow * sub;
        foreach (sub, ui->mdiArea->subWindowList()) {
            auto tab = qobject_cast<MdiSubWindow*>(sub);
            if(file==tab->editor()->file()) {
                qInfo() << "The file" << fileName << "has been opened.";
                return tab;
            }
        }
    }
    return 0;
}

void MdeWindow::warningNoEditor(bool noEditor)
{
    static bool showed = false;
    if(noEditor) {
        qWarning() << "No editor available!";
        if(!showed) {
            QMessageBox::warning(this,tr("No editor availabe!"),
                                 tr("The operation cannot be performed "
                                    "because there is no editor availbale."));
            showed = true;
        }
    }
    else {
        showed = false;
    }
}

void MdeWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if(ui->mdiArea->currentSubWindow()) {
        event->ignore();
    }
    else {
        saveSettings();
        event->accept();
    }
}
