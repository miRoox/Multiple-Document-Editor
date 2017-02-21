#ifndef MDEWINDOW_H
#define MDEWINDOW_H

#include <mdewidget/mdewidget_global.h>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class MdeWindowPrivate;
class MdiSubWindow;
class GeneralSettings;
class PluginManager;


class MDEWIDGETSHARED_EXPORT MdeWindow
        : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QMenu* menuFile READ menuFile CONSTANT)
    Q_PROPERTY(QMenu* menuEdit READ menuEdit CONSTANT)
    Q_PROPERTY(QMenu* menuView READ menuView CONSTANT)
    Q_PROPERTY(QMenu* menuTools READ menuTools CONSTANT)
    Q_PROPERTY(QMenu* menuSettings READ menuSettings CONSTANT)
    Q_PROPERTY(QMenu* menuHelp READ menuHelp CONSTANT)

public:
    explicit MdeWindow(GeneralSettings *settings, QWidget *parent = 0);
    void installPluginManager(PluginManager * pm);//only once
    QMenu * menuFile() const;
    QMenu * menuEdit() const;
    QMenu * menuView() const;
    QMenu * menuTools() const;
    QMenu * menuSettings() const;
    QMenu * menuHelp() const;
    QString currentFile() const;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent * event) override;

signals:
    void openedFile(QString/*fileName*/);//emit when open file successfully, besides opened file
    void savedFile(QString/*fileName*/);//emit when save file successfully

public slots:
    void addToPreferrence(QString name, QWidget * page);
    void newDoc();
    bool openFile(QString fileName);
    bool openFileWithSelectedEditor(QString fileName);
    quint32 openFilesRecursively(QString fileName);
    void openWithDialog(bool selectable = false);
    bool save();
    bool saveAs();

private:
    MdeWindowPrivate * p;
};

#endif // MDEWINDOW_H
