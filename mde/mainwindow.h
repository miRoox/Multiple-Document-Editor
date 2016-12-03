#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

#include "findreplacedialog.h"
#include "plugininterface.h"

QT_BEGIN_NAMESPACE
class QMdiSubWindow;
class QMenu;
class QAction;
class QActionGroup;
QT_END_NAMESPACE

class MdiChild;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFile(QString fileName);
    unsigned int loadPlugins();

private slots:
    void updateMenus();
    MdiChild * createMdiChild();
    void openFile(QAction * action);
    void closeMdiChild(QString fileName);
    void setActiveSubWindow(QWidget * window);
    void showTextInfo();
    bool findByCursor(QString pattern,
                      QTextDocument::FindFlags options,
                      FindReplaceDialog::FindModel model);
    int counter(QString pattern,
                QTextDocument::FindFlags options,
                FindReplaceDialog::FindModel model);
    bool replace(QString to,
                 QString pattern,
                 QTextDocument::FindFlags options,
                 FindReplaceDialog::FindModel model);
    int replaceAll(QString to,
                   QString pattern,
                   QTextDocument::FindFlags options,
                   FindReplaceDialog::FindModel model);
    void setCurrentCode(QAction * act);
    void transCurrentCode(QAction * act);
    void clearHistory();
    void pluginAct();

    void on_action_New_triggered();

    void on_action_Open_triggered();

    void on_action_Save_triggered();

    void on_action_SaveAs_triggered();

    void on_action_Undo_triggered();

    void on_action_Redo_triggered();

    void on_action_Cut_triggered();

    void on_action_Copy_triggered();

    void on_action_Paste_triggered();

    void on_action_Close_triggered();

    void on_action_CloseAll_triggered();

    void on_action_Next_triggered();

    void on_action_Previous_triggered();

    void on_action_About_triggered();

    void on_action_Exit_triggered();

    void on_action_WrapLine_toggled(bool checked);

    void on_action_ZoomIn_triggered();

    void on_action_ZoomOut_triggered();

    void on_action_AboutQt_triggered();

    void on_action_Find_triggered();

    void on_action_Replace_triggered();

    void on_action_Bom_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QTextEdit::LineWrapMode lineWrapMode;
    int textSize;
    QStringList history;
    QString recentDir;

    void readSettings();
    void writeSettings();
    void initStatusBar();
    void initMenu();
    MdiChild * activeMdiChild();
    QMdiSubWindow * findMdiChild(const QString &fileName);
    bool findInDocuent(QTextCursor &cur,
                       QString pattern,
                       QTextDocument::FindFlags options,
                       FindReplaceDialog::FindModel model);
    void addToHistory(QString fileName);

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    FindReplaceDialog * frDialog;
    QMenu * menuCodec;
    QMenu * menuTranscode;
    QActionGroup * codecGroup;
    QActionGroup * transcodeGroup;
    QList<PluginInferface *> plugins;

};

#endif // MAINWINDOW_H
