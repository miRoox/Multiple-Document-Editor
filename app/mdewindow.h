#ifndef MDEWINDOW_H
#define MDEWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class PlainTextEditor;

namespace Ui {
class MdeWindow;
}

class MdeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MdeWindow(QWidget *parent = 0);
    ~MdeWindow();
    PlainTextEditor * createPTE();

private:
    Ui::MdeWindow *ui;

    QLabel * summaryLabel;
    QLabel * cursorLabel;

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed(MdeWindow *);

private:
    void initStatusBar();
};

#endif // MDEWINDOW_H
