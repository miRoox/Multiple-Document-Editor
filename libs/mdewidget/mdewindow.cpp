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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
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
    p->initPreferrence();
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

void MdeWindow::addToPreferrence(QString name, QWidget *page)
{
    p->addToPreferrence(name,page);
}

void MdeWindow::newDoc()
{
    static quint64 sequence = 1;
    qInfo() << "Attempt to create a new document";
    IEditor * editor = p->plugManager->defaultEditor();
    if(!editor)
        return p->warningNoEditor();
    MdiSubWindow * tab = p->addToSubWindow(editor);
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
        tab = p->addToSubWindow(editor);
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
    tab = p->addToSubWindow(editor);
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
    QMdiSubWindow * active = p->ui->mdiArea->activeSubWindow();
    if(active) {
        auto sub = qobject_cast<MdiSubWindow*>(active);
        QString fileName = sub->editor()->save(false);
        if(!fileName.isEmpty()) {
            emit savedFile(fileName);
            return true;
        }
        else {
            p->warningSaveFailed(sub->windowTitle());
        }
    }
    return false;
}

bool MdeWindow::saveAs()
{
    QMdiSubWindow * active = p->ui->mdiArea->activeSubWindow();
    if(active) {
        auto sub = qobject_cast<MdiSubWindow*>(active);
        QString fileName = sub->editor()->save(true);
        if(!fileName.isEmpty()) {
            setWindowTitle(sub->editor()->title());
            emit savedFile(fileName);
            return true;
        }
        else {
            p->warningSaveFailed(sub->editor()->file().canonicalFilePath());
        }
    }
    return false;
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

void MdeWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else event->ignore();
}

void MdeWindow::dropEvent(QDropEvent *event)
{
    const QMimeData * mimeData = event->mimeData();
    if(mimeData->hasUrls()) {
        foreach (QUrl url, mimeData->urls()) {
            openFile(url.toLocalFile());
        }
    }
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
