#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QFlags>
#include <QTextDocument>

namespace Ui {
class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = 0);
    ~FindReplaceDialog();

    enum FindFlag { RegExp = 1, Loop = 2};
    typedef QFlags<FindFlag> FindModel;

    void showTab(int index);

signals:
    void findNext(QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel);
    void count(QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel);
    void replace(QString,QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel);
    void replaceAll(QString,QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel);

private slots:
    void on_fNextButton_clicked();

    void on_fCancelButton_clicked();

    void on_rNextButton_clicked();

    void on_rCancelButton_clicked();

    void on_fCountButton_clicked();

    void on_rReplaceButton_clicked();

    void on_rReplaceAllButton_clicked();

private:
    Ui::FindReplaceDialog *ui;
};

#endif // FINDREPLACEDIALOG_H
