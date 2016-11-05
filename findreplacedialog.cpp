#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::showTab(int index)
{
    ui->tabWidget->setCurrentIndex(index);
    show();
}

/** 自动关联的槽 **/

void FindReplaceDialog::on_fNextButton_clicked()
{
    QTextDocument::FindFlags flag;
    if(ui->fUpButton->isChecked()) flag |= QTextDocument::FindBackward;
    if(ui->fCapitalBox->isChecked()) flag |= QTextDocument::FindCaseSensitively;
    if(ui->fWholeBox->isChecked()) flag |= QTextDocument::FindWholeWords;
    FindModel model;
    if(ui->fRegexpButton->isChecked()) model |= RegExp;
    if(ui->fLoopBox->isChecked()) model |= Loop;
    QString pattern = ui->fLineEdit->text();
    emit findNext(pattern,flag,model);
}

void FindReplaceDialog::on_rNextButton_clicked()
{
    QTextDocument::FindFlags flag;
    if(ui->rUpButton->isChecked()) flag |= QTextDocument::FindBackward;
    if(ui->rCapitalBox->isChecked()) flag |= QTextDocument::FindCaseSensitively;
    if(ui->rWholeBox->isChecked()) flag |= QTextDocument::FindWholeWords;
    FindModel model;
    if(ui->rRegexpButton->isChecked()) model |= RegExp;
    if(ui->rLoopBox->isChecked()) model |= Loop;
    QString pattern = ui->rLineEdit->text();
    emit findNext(pattern,flag,model);
}

void FindReplaceDialog::on_fCountButton_clicked()
{
    QTextDocument::FindFlags flag;
    if(ui->fCapitalBox->isChecked()) flag |= QTextDocument::FindCaseSensitively;
    if(ui->fWholeBox->isChecked()) flag |= QTextDocument::FindWholeWords;
    FindModel model;
    if(ui->fRegexpButton->isChecked()) model |= RegExp;
    QString pattern = ui->fLineEdit->text();
    emit count(pattern,flag,model);
}

void FindReplaceDialog::on_fCancelButton_clicked()
{
    close();
}

void FindReplaceDialog::on_rCancelButton_clicked()
{
    close();
}

