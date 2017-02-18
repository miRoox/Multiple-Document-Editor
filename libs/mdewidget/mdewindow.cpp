#include "mdewindow.h"
#include "mdewindow_p.h"
#include "ui_mdewindow.h"
#include <generalsettings/generalsettings.h>
#include <extensionsystem/pluginmanager.h>
#include "mdisubwindow.h"
#include <extensionsystem/ieditor.h>
#include <QStringList>
#include <QMdiArea>
#include <QProcess>
#include <QKeySequence>
#include <QFileDialog>
#include <QCloseEvent>
#include <QDebug>

MdeWindow::MdeWindow(GeneralSettings * settings, QWidget *parent) :
    QMainWindow(parent)
{
    p = new MdeWindowPrivate(this);
    p->genSettings = settings;
    p->installGeneralSettings();
    qInfo() << "Set up user interface";
    p->ui->setupUi(this);
    p->initActions();
    p->loadSettings();
    setAcceptDrops(true);
    connect(p->ui->mdiArea,QMdiArea::subWindowActivated,[this](QMdiSubWindow * active){
            if(active) setWindowTitle(active->windowTitle());
            else setWindowTitle("");
    });
}

void MdeWindow::installPluginManager(PluginManager *pm)
{
    if(p->plugManager==nullptr) {
        p->plugManager = pm;
        p->plugManager->setMDE(this);
        connect(p->ui->actionPluginManager,QAction::triggered,
                p->plugManager,PluginManager::execPluginSelectionDialog);
    }
}

MdiSubWindow *MdeWindow::addToSubWindow(IEditor *editor)
{
    MdiSubWindow * tab = new MdiSubWindow(p->ui->mdiArea);
    tab->setEditor(editor);
    p->ui->mdiArea->addSubWindow(tab);
    connect(p->ui->mdiArea,QMdiArea::subWindowActivated,
            tab,MdiSubWindow::slotSubWindowActivated);
    return tab;
}

void MdeWindow::newDoc()
{
    static quint64 sequence = 1;
    qInfo() << "Attempt to create a new document";
    IEditor * editor = p->plugManager->defaultEditor();
    if(!editor)
        return p->warningNoEditor();
    MdiSubWindow * tab = addToSubWindow(editor);
    tab->setEditor(editor);
    editor->newFile();
    tab->setWindowTitle(tr("Untitled %1").arg(sequence++));
    tab->showMaximized();
    p->warningNoEditor(false);
    qInfo() << "Create a new document.";
}

bool MdeWindow::openFile(QString fileName)
{
    if(fileName.isEmpty())
        return false;
    qInfo() << "Attempt to open file:" << fileName;
    MdiSubWindow * tab = p->findSubWindow(fileName);
    if(tab) {
        qInfo() << "The file" << fileName << "has been opened.";
        p->ui->mdiArea->setActiveSubWindow(tab);
    }
    else {
        IEditor * editor = p->plugManager->editor(fileName);
        if(!editor)
            editor = p->plugManager->defaultBrowser();
        if(!editor) {
            p->warningNoEditor();
            return false;
        }
        tab = addToSubWindow(editor);
        if(!editor->loadFile(fileName)) {
            p->warningOpenFailed(fileName);
            return false;
        }
        tab->setWindowTitle(editor->title());
        tab->showMaximized();
        p->warningNoEditor(false);
        qInfo() << "Open file:"
                << QDir::toNativeSeparators(QFileInfo(fileName).canonicalFilePath());
    }
    emit openedFile(QFileInfo(fileName).canonicalFilePath());
    return true;
}

bool MdeWindow::openFileWithSelectedEditor(QString fileName)
{
    if(fileName.isEmpty())
        return false;
    qInfo() << "Attempt to open file:" << fileName;
    IEditor * editor = p->plugManager->selectEditor();
    if(!editor) //cancel selection
        return openFile(fileName);
    MdiSubWindow * tab = p->findSubWindow(fileName);
    if(tab) {
        tab->close();
    }
    tab = addToSubWindow(editor);
    if(!editor->loadFile(fileName)) {
        p->warningOpenFailed(fileName);
        return false;
    }
    tab->setWindowTitle(editor->title());
    tab->showMaximized();
    p->warningNoEditor(false);
    qInfo() << "Open file:"
            << QDir::toNativeSeparators(QFileInfo(fileName).canonicalFilePath());
    emit openedFile(QFileInfo(fileName).canonicalFilePath());
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

void MdeWindow::openWithDialog(bool selectable)
{
    QString filter = p->plugManager->fileNameFilter();
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Select one or more files to open"),
                                                          p->defaultDir(),
                                                          filter);
    if(selectable)
        foreach (QString fileName, fileNames) {
            openFileWithSelectedEditor(fileName);
        }
    else
        foreach (QString fileName, fileNames) {
            openFile(fileName);
        }
}

bool MdeWindow::save()
{
    qInfo() << "Attempt to save current file";
    QMdiSubWindow * active = p->ui->mdiArea->activeSubWindow();
    if(active) {
        auto sub = qobject_cast<MdiSubWindow*>(active);
        QString fileName = sub->editor()->file().canonicalFilePath();
        if(sub->editor()->save()) {
            emit savedFile(fileName);
            return true;
        }
        else {
            p->warningSaveFailed(fileName);
        }
    }
    return false;
}

bool MdeWindow::saveAsFile(QString fileName)
{
    if(fileName.isEmpty())
        return false;
    qInfo() << "Attempt to save current file as" << fileName;
    QMdiSubWindow * active = p->ui->mdiArea->activeSubWindow();
    if(active) {
        auto sub = qobject_cast<MdiSubWindow*>(active);
        if(sub->editor()->saveAs(fileName)) {
            setWindowTitle(sub->editor()->title());
            emit savedFile(fileName);
            return true;
        }
        else {
            p->warningSaveFailed(fileName);
        }
    }
    return false;
}

void MdeWindow::saveAs()
{
    QString filter = p->plugManager->fileNameFilter();
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save as"),
                                                    p->defaultDir(),filter);
    saveAsFile(fileName);
}

QMenu * MdeWindow::menuFile() const
{
    return p->ui->menu_File;
}
QMenu * MdeWindow::menuEdit() const
{
    return p->ui->menu_Edit;
}
QMenu * MdeWindow::menuView() const
{
    return p->ui->menu_View;
}
QMenu * MdeWindow::menuTools() const
{
    return p->ui->menu_Tools;
}
QMenu * MdeWindow::menuSettings() const
{
    return p->ui->menu_Settings;
}
QMenu * MdeWindow::menuHelp() const
{
    return p->ui->menu_Help;
}

QString MdeWindow::currentFile() const
{
    QMdiSubWindow * active = p->ui->mdiArea->activeSubWindow();
    if(active) {
        return qobject_cast<MdiSubWindow*>(active)->editor()->file().canonicalFilePath();
    }
    return QString();
}

void MdeWindow::closeEvent(QCloseEvent *event)
{
    p->ui->mdiArea->closeAllSubWindows();
    if(p->ui->mdiArea->currentSubWindow()) {
        event->ignore();
    }
    else {
        p->saveSettings();
        event->accept();
    }
}
