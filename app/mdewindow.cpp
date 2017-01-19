#include "mdewindow.h"
#include "ui_mdewindow.h"

#include <QCloseEvent>

MdeWindow::MdeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MdeWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    setAttribute(Qt::WA_DeleteOnClose);
}

MdeWindow::~MdeWindow()
{
    delete ui;
}


void MdeWindow::closeEvent(QCloseEvent *event)
{
    emit closed(this);
    event->accept();
}
