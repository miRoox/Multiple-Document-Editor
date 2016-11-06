#include "mdichild.h"

#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPushButton>
#include <QMenu>
#include <QTextCodec>
#include <QDebug>

MdiChild::MdiChild(QWidget *parent) :
    QTextEdit(parent)
{
    // 这样可以在子窗口关闭时销毁这个类的对象
    setAttribute(Qt::WA_DeleteOnClose);

    // 初始isUntitled为true
    isUntitled = true;

    codec = QTextCodec::codecForLocale();

    connect(&fWatcher,SIGNAL(fileChanged(QString)),this,SLOT(changeMessage(QString)));
    connect(&fWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(changeMessage(QString)));
}

// 新建文件操作
void MdiChild::newFile()
{
    static unsigned int sequenceNum = 1;// 设置窗口编号
    isUntitled = true;
    // 将当前文件命名为未命名文档加编号，编号先使用再加1
    curFile = tr("未命名文档 %1").arg(sequenceNum++);
    // 设置窗口标题，使用[*]可以在文档被更改后在文件名称后显示”*“号
    setWindowTitle(curFile + "[*]");
    // 当文档被更改时发射contentsChanged()信号，
    // 执行我们的documentWasModified()槽函数
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));
}

// 加载文件
bool MdiChild::loadFile(const QString &fileName,bool re)
{
    QFile file(fileName);
    // 只读方式打开文件，出错则提示，并返回false
    if(! file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),
                             tr("无法读取文件 %1:\n%2").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    codec = in.codec();
    in.setCodec(codec);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);// 设置鼠标状态为等待状态
    setPlainText(in.readAll());// 读取文件的全部文本内容，并添加到编辑器中
    QGuiApplication::restoreOverrideCursor();// 恢复鼠标状态
    if(!re)
    {
        setCurrentFile(fileName);
        connect(document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));
    }
    else {
        setModified(false);
    }
    file.close();
    return true;
}

// 提取文件名
QString MdiChild::userFriendlyCurrentFile()
{
    return QFileInfo(curFile).fileName(); // 从文件路径中提取文件名
}

// 保存操作
bool MdiChild::save()
{
    if (isUntitled) { // 如果文件未被保存过，则执行另存为操作
        return saveAs();
    }
    else {
        return saveFile(curFile); //否则直接保存文件
    }
}

// 另存为操作
bool MdiChild::saveAs()
{
    // 获取文件路径，如果为空，则返回false
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"),curFile,
                                                    tr("文本文件(*.txt);;所有文件(*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName); // 否则保存文件
}

// 保存文件
bool MdiChild::saveFile(const QString &fileName)
{
    fWatcher.removePath(curFile);
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法写入文件 %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setCodec(codec);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText(); // 以纯文本文件写入
    QGuiApplication::restoreOverrideCursor();
    file.close();
    setCurrentFile(fileName);
    return true;
}

//设置是否有修改
void MdiChild::setModified(bool is)
{
    document()->setModified(is);
    documentWasModified();
}

//设置字体大小
void MdiChild::setTextSize(int size)
{
    QFont f = document()->defaultFont();
    f.setPointSize(size);
    document()->setDefaultFont(f);
}

/** 私有方法 **/

// 设置当前文件
void MdiChild::setCurrentFile(const QString &fileName)
{
    // canonicalFilePath()可以除去路径中的符号链接，“.”和“..”等符号
    curFile = QFileInfo(fileName).canonicalFilePath();
    if(fWatcher.files().isEmpty()) fWatcher.addPath(fileName);
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    // 设置窗口标题，userFriendlyCurrentFile()返回文件名
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

// 是否需要保存
bool MdiChild::maybeSave()
{
    if(document()->isModified())
    {
        QMessageBox box;
        box.setWindowTitle(tr("多文档编辑器"));
        box.setText(tr("是否保存对 \"%1\" 的更改？")
                    .arg(userFriendlyCurrentFile()));
        box.setIcon(QMessageBox::Warning);
        QPushButton * yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        QPushButton * noBtn = box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        box.addButton(tr("取消(&C)"),QMessageBox::RejectRole);
        box.exec();
        if(box.clickedButton() == yesBtn){
            return save();
        }
        else if (box.clickedButton() == noBtn) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return true;
    }
}

/** 共有槽 **/

void MdiChild::setCodec(QTextCodec *codec)
{
    this->codec = codec;
    loadFile(curFile,true);
}

/** 私有槽 **/


//文档被更改时，窗口显示更改状态标志
void MdiChild::documentWasModified()
{
    // 根据文档的isModified()函数的返回值，判断我们编辑器内容是否被更改了
    // 如果被更改了，就要在设置了[*]号的地方显示“*”号，这里我们会在窗口标题中显示
    setWindowModified(document()->isModified());
}

//文件被外部修改时
void MdiChild::changeMessage(const QString &fileName)
{
    if(QFileInfo(fileName).exists())
    {
        QMessageBox box;
        box.setWindowTitle(tr("多文档编辑器"));
        box.setText(tr("文档 \"%1\" 可能被外部编辑器修改\n是否重新载入？")
                    .arg(fileName));
        box.setIcon(QMessageBox::Warning);
        QPushButton * yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        box.exec();
        if(box.clickedButton()==yesBtn)
        {
            loadFile(fileName,true);
        }
        else {
            setModified(true);
        }
    }
    else {
        QMessageBox box;
        box.setWindowTitle(tr("多文档编辑器"));
        box.setText(tr("文档 \"%1\" 已不存在\n是否关闭？")
                    .arg(fileName));
        box.setIcon(QMessageBox::Warning);
        QPushButton * yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        box.exec();
        if(box.clickedButton()==yesBtn)
        {
            setModified(false);
            emit closeThis(fileName);
        }
        else {
            setModified(true);
        }
    }
}

/** 事件处理函数 **/

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) // 如果maybeSave()函数返回true，则关闭窗口
    {
        event->accept();
    }
    else {   // 否则忽略该事件
        event->ignore();
    }
}

void MdiChild::contextMenuEvent(QContextMenuEvent *e) // 右键菜单事件
{
    QMenu *menu = new QMenu; // 创建菜单，并向其中添加动作
    QAction *undo = menu->addAction(tr("撤销(&U)"),this,
                                    SLOT(undo()),QKeySequence::Undo);
    undo->setEnabled(document()->isUndoAvailable());
    QAction *redo = menu->addAction(tr("恢复(&R)"),this,
                                    SLOT(redo()),QKeySequence::Redo);
    redo->setEnabled(document()->isRedoAvailable());
    menu->addSeparator();
    QAction *cut = menu->addAction(tr("剪切(&T)"),this,
                                   SLOT(cut()),QKeySequence::Cut);
    cut->setEnabled(textCursor().hasSelection());
    QAction *copy = menu->addAction(tr("复制(&C)"),this,
                                    SLOT(copy()),QKeySequence::Copy);
    copy->setEnabled(textCursor().hasSelection());
    menu->addAction(tr("粘贴(&P)"),this,SLOT(paste()),QKeySequence::Paste);
    QAction *clear = menu->addAction(tr("清空"),this,SLOT(clear()));
    clear->setEnabled(!document()->isEmpty());
    menu->addSeparator();
    QAction *select = menu->addAction(tr("全选"),this,
                                SLOT(selectAll()),QKeySequence::SelectAll);
    select->setEnabled(!document()->isEmpty());
    menu->exec(e->globalPos()); // 获取鼠标的位置，然后在这个位置显示菜单
    delete menu; // 最后销毁这个菜单
}
