#include "mdewindow.h"
#include "ui_mdewindow.h"
#include "plaintexteditor.h"

#include <QLabel>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QCloseEvent>

MdeWindow::MdeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MdeWindow)
{
    summaryLabel = new QLabel(this);
    cursorLabel = new QLabel(this);
    ui->setupUi(this);
    initStatusBar();
    setAcceptDrops(true);
}

MdeWindow::~MdeWindow()
{
    delete ui;
}

PlainTextEditor * MdeWindow::createPTE()
{
    PlainTextEditor * pte = new PlainTextEditor(this);
    QMdiSubWindow * tab = ui->mdiArea->addSubWindow(pte);
    connect(pte,QObject::destroyed,
            tab,QMdiSubWindow::close);
    return pte;
}

void MdeWindow::closeEvent(QCloseEvent *event)
{
    emit closed(this);
    event->accept();
}

void MdeWindow::initStatusBar()
{
    summaryLabel->setText("length:- lines:-\t");
    cursorLabel->setText("Col:- Ln:-\t");
    ui->statusbar->addPermanentWidget(summaryLabel);
    ui->statusbar->addPermanentWidget(cursorLabel);
}
