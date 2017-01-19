#ifndef MDEWINDOW_H
#define MDEWINDOW_H

#include <QMainWindow>

namespace Ui {
class MdeWindow;
}

class MdeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MdeWindow(QWidget *parent = 0);
    ~MdeWindow();

private:
    Ui::MdeWindow *ui;

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed(MdeWindow *);
};

#endif // MDEWINDOW_H
