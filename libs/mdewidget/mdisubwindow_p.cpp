#include "mdisubwindow_p.h"
#include <extensionsystem/ieditor.h>
#include <QFileInfo>
#include <QMessageBox>
#include <QCheckBox>

MdiSubWindowPrivate::MdiSubWindowPrivate(MdiSubWindow *parent)
    : QObject(parent),
      w(parent)
{
    editor = nullptr;
    genSettings = nullptr;
}

MdiSubWindowPrivate::~MdiSubWindowPrivate()
{

}

void MdiSubWindowPrivate::save()
{
    static bool nolonger = false;
    static int choice = QMessageBox::Cancel;
    if(editor->file().exists()) {
        editor->save();
    }
    else if (!nolonger) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle(tr("The document has not been saved yet"));
        msgBox.setText(tr("The document %1 has not been saved yet\n"
                          "Do you want to save it?").arg(w->windowTitle()));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        QCheckBox * checkBox = new QCheckBox(tr("No longer remind me"));
        msgBox.setCheckBox(checkBox);
        choice = msgBox.exec();
        if(choice == QMessageBox::Save) {
            editor->saveAs();
        }
        if (checkBox->isChecked()) {
            nolonger = true;
        }
    }
    else if (choice == QMessageBox::Save) {
        editor->saveAs();
    }
}
