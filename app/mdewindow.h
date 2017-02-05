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
    QMenu * menuFile();
    QMenu * menuEdit();
    QMenu * menuView();
    QMenu * menuTools();
    QMenu * menuSettings();
    QMenu * menuHelp();

protected:

signals:

public slots:
    MdiSubWindow * addToSubWindow(IEditor * editor);
    void newDoc();
    bool openFile(QString file);
    quint32 openFiles(QString file);

private:
    void initActions();

private:
    Ui::MdeWindow *ui;

    PluginManager * plugManager;

};

#endif // MDEWINDOW_H
