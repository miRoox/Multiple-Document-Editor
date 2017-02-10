#ifndef MDEWINDOW_H
#define MDEWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QMenu;
QT_END_NAMESPACE

class IEditor;
class PluginManager;
class MdiSubWindow;

namespace Ui {
class MdeWindow;
}

class MdeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MdeWindow(QWidget *parent = 0);
    ~MdeWindow();
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

public slots:
    MdiSubWindow * addToSubWindow(IEditor * editor);
    void newDoc();
    bool openFile(QString fileName);
    quint32 openFilesRecursively(QString fileName);
    void openWithDialog();

private:
    void initActions();
    void loadSettings();
    void saveSettings();
    MdiSubWindow * findSubWindow(QString fileName);
    void warningNoEditor(bool noEditor = true);

private:
    Ui::MdeWindow *ui;

    PluginManager * plugManager;

};

#endif // MDEWINDOW_H
