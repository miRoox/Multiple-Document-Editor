#ifndef MDEWINDOW_H
#define MDEWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class MdeWindowPrivate;
class IEditor;
class MdiSubWindow;
class PluginManager;


class MdeWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QMenu* menuFile READ menuFile CONSTANT)
    Q_PROPERTY(QMenu* menuEdit READ menuEdit CONSTANT)
    Q_PROPERTY(QMenu* menuView READ menuView CONSTANT)
    Q_PROPERTY(QMenu* menuTools READ menuTools CONSTANT)
    Q_PROPERTY(QMenu* menuSettings READ menuSettings CONSTANT)
    Q_PROPERTY(QMenu* menuHelp READ menuHelp CONSTANT)

public:
    explicit MdeWindow(QWidget *parent = 0);
    void setPluginManager(PluginManager * pm);
    QMenu * menuFile() const;
    QMenu * menuEdit() const;
    QMenu * menuView() const;
    QMenu * menuTools() const;
    QMenu * menuSettings() const;
    QMenu * menuHelp() const;

protected:
    void closeEvent(QCloseEvent * event) override;

signals:
    void openedFile(QString/*fileName*/);//emit when open file successfully, besides opened file

public slots:
    MdiSubWindow * addToSubWindow(IEditor * editor);
    void newDoc();
    bool openFile(QString fileName);
    bool openFileWithSelectedEditor(QString fileName);
    quint32 openFilesRecursively(QString fileName);
    void openWithDialog(bool selectable = false);

private:
    MdeWindowPrivate * p;
};

#endif // MDEWINDOW_H
