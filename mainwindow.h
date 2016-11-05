#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

#include "findreplacedialog.h"

class QMdiSubWindow;
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

private slots:
    void updateMenus();
    MdiChild * createMdiChild();
    void closeMdiChild(QString fileName);
    void setActiveSubWindow(QWidget * window);
    void showTextRowAndCol();
    bool findInDocument(QString pattern,
                        QTextDocument::FindFlags options,
                        FindReplaceDialog::FindModel model);
    int counter(QString pattern,
                QTextDocument::FindFlags options,
                FindReplaceDialog::FindModel model);

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

    void on_action_Tile_triggered();

    void on_action_Cascade_triggered();

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

private:
    Ui::MainWindow *ui;
    QTextEdit::LineWrapMode lineWrapMode;
    int textSize;

    void readSettings();
    void writeSettings();
    void initStatusBar();
    void openFile(QString fileName);
    MdiChild * activeMdiChild();
    QMdiSubWindow * findMdiChild(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    FindReplaceDialog * frDialog;
    static const QStringList codecList /*= {"GB2312","GBK","GB18030",
                                          "Big5","Big5-HKSCS",
                                          "UTF-8",
                                          "UTF-16","UTF-16BE","UTF-16LE",
                                          "UTF-32","UTF-32BE","UTF-32LE",0}*/;
};

#endif // MAINWINDOW_H
