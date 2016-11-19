#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"
#include "findreplacedialog.h"
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QMimeData>
#include <cstring>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSettings();

    initMenu();

    updateMenus();
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
                this, SLOT(updateMenus()));           //当有活动窗口时更新菜单

    setWindowTitle(tr("多文档编辑器"));

    frDialog = new FindReplaceDialog(this);//查找与替换
    connect(frDialog,SIGNAL(findNext(QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)),
            this,SLOT(findByCursor(QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)));
    connect(frDialog,SIGNAL(count(QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)),
            this,SLOT(counter(QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)));
    connect(frDialog,SIGNAL(replace(QString,QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)),
            this,SLOT(replace(QString,QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)));
    connect(frDialog,SIGNAL(replaceAll(QString,QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)),
            this,SLOT(replaceAll(QString,QString,QTextDocument::FindFlags,FindReplaceDialog::FindModel)));

    // 我们在工具栏上单击鼠标右键时，可以关闭工具栏
    ui->mainToolBar->setWindowTitle(tr("工具栏"));

    // 当多文档区域的内容超出可视区域后，出现滚动条
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    initStatusBar();

    setAcceptDrops(true); //打开主窗口的拖放接收
}

MainWindow::~MainWindow()
{
    delete ui;
}

//活动窗口
MdiChild * MainWindow::activeMdiChild() //活动窗口
{
    // 如果有活动窗口，则将其内的中心部件转换为MdiChild类型
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0; // 没有活动窗口，直接返回0
}

// 查找子窗口
QMdiSubWindow * MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = fileName; //文件被删除但其窗口仍打开时
    if(QFileInfo(fileName).exists()){
        canonicalFilePath = QDir::toNativeSeparators(QFileInfo(fileName).canonicalFilePath());
    }
    // 利用foreach语句遍历子窗口列表，如果其文件路径和要查找的路径相同，则返回该窗口
    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

//打开文件操作
void MainWindow::openFile(QString fileName)
{
    if (!fileName.isEmpty()) // 如果路径不为空，则查看该文件是否已经打开
    {
        QMdiSubWindow *existing = findMdiChild(fileName);
        MdiChild * child;
        if (existing) // 如果已经存在，则将对应的子窗口设置为活动窗口
        {
            child = qobject_cast<MdiChild *>(existing->widget());
            ui->mdiArea->setActiveSubWindow(existing);
        }
        else {
            child = createMdiChild(); // 如果没有打开，则新建子窗口
        }

        if (child->loadFile(fileName,existing)) {
            addToHistory(child->currentFile());
            ui->statusBar->showMessage(tr("打开文件成功"), 2000);
            if(1 == ui->mdiArea->subWindowList().size()) child->showMaximized();
            else child->show();
        }
        else {
            child->close();
        }
    }
}

void MainWindow::initMenu()
{
    //历史记录
    ui->menu_History->removeAction(ui->menu_History->actions().first());
    foreach (QString name, history) {
        QAction * action = new QAction(name,this);
        action->setData(name);
        ui->menu_History->addAction(action);
    }
    ui->menu_History->addSeparator();
    ui->menu_History->addAction(tr("清空历史记录"),this,SLOT(clearHistory()));
    connect(ui->menu_History,SIGNAL(triggered(QAction*)),
            this,SLOT(openFile(QAction*)));

    //自动换行
    ui->action_WrapLine->setChecked(QTextEdit::WidgetWidth==lineWrapMode);

    //选择编码与转换编码
    menuCodec = new QMenu(tr("选择编码"),this);
    menuTranscode = new QMenu(tr("转换编码"),this);
    codecGroup = new QActionGroup(this);
    transcodeGroup = new QActionGroup(this);
    ui->menu_C->addMenu(menuCodec);
    ui->menu_C->addMenu(menuTranscode);
    //遍历可用编码
    QList<QByteArray> codeNames = QTextCodec::availableCodecs();
    foreach (QByteArray name, codeNames) {
        QAction * codecAction = new QAction(name,codecGroup);
        QAction * transcAction = new QAction(name,transcodeGroup);
        codecAction->setData(name);
        codecAction->setCheckable(true);
        if(name=="System") codecAction->setChecked(true);
        transcAction->setData(name);
    }
    menuCodec->addActions(codecGroup->actions());
    menuTranscode->addActions(transcodeGroup->actions());
    connect(codecGroup,SIGNAL(triggered(QAction*)),
             this,SLOT(setCurrentCode(QAction*)));
    connect(transcodeGroup,SIGNAL(triggered(QAction*)),
            this,SLOT(transCurrentCode(QAction*)));
}


//初始化状态栏
void MainWindow::initStatusBar()
{
    ui->action_New->setStatusTip(tr("创建一个文件"));
    ui->action_Open->setStatusTip(tr("打开一个已经存在的文件"));
    ui->menu_History->setStatusTip(tr("选择曾经打开过的文件"));
    ui->action_Save->setStatusTip(tr("保存文档到硬盘"));
    ui->action_SaveAs->setStatusTip(tr("以新的名称保存文档"));
    ui->action_Exit->setStatusTip(tr("退出应用程序"));
    ui->action_Undo->setStatusTip(tr("撤销先前的操作"));
    ui->action_Redo->setStatusTip(tr("恢复先前的操作"));
    ui->action_Cut->setStatusTip(tr("剪切选中的内容到剪贴板"));
    ui->action_Copy->setStatusTip(tr("复制选中的内容到剪贴板"));
    ui->action_Paste->setStatusTip(tr("粘贴剪贴板的内容到当前位置"));
    ui->action_Find->setStatusTip(tr("在当前文件中查找"));
    ui->action_Replace->setStatusTip(tr("在当前文件中替换字符或字符串"));
    ui->action_WrapLine->setStatusTip(tr("选择是否自动换行"));
    ui->action_ZoomIn->setStatusTip(tr("放大文本显示的大小"));
    ui->action_ZoomOut->setStatusTip(tr("缩小文本显示的大小"));
    ui->action_Close->setStatusTip(tr("关闭活动窗口"));
    ui->action_CloseAll->setStatusTip(tr("关闭所有窗口"));
    ui->action_Next->setStatusTip(tr("将焦点移动到下一个窗口"));
    ui->action_Previous->setStatusTip(tr("将焦点移动到前一个窗口"));
    ui->action_About->setStatusTip(tr("显示本软件的介绍"));
    ui->action_AboutQt->setStatusTip(tr("显示Qt帮助"));
    ui->statusBar->showMessage(tr("欢迎使用多文档编辑器"));
    menuCodec->setStatusTip(tr("选择当前文件的编码格式"));
    menuTranscode->setStatusTip(tr("将当前文件的编码转化为特定编码格式"));
}

// 写入窗口设置
void MainWindow::writeSettings()
{
    QSettings settings("miroox","MDE");
    settings.setValue("winState",saveState());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("tsize",textSize);
    settings.setValue("lineWrap",lineWrapMode);
    settings.setValue("history",history);
}

// 读取窗口设置
void MainWindow::readSettings()
{
    QSettings settings("miroox", "MDE");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("winState").toByteArray());
    textSize = settings.value("tsize",12).toInt();
    lineWrapMode = static_cast<QTextEdit::LineWrapMode>(settings.value("lineWrap",QTextEdit::NoWrap).toUInt());
    history = settings.value("history",QStringList()).toStringList();
}


//在文档中查找
bool MainWindow::findInDocuent(QTextCursor &cur, QString pattern, QTextDocument::FindFlags options, FindReplaceDialog::FindModel model)
{
    QTextDocument * document = activeMdiChild()->document();
    bool isFind;
    if(model & FindReplaceDialog::RegExp)
    {
        QRegExp regexp(pattern);
        if(!regexp.isValid())
        {
            QMessageBox::warning(frDialog,tr("正则表达式错误！"),
                                 tr("原因: %1").arg(regexp.errorString()));
            return false;
        }
        regexp.setCaseSensitivity((options & QTextDocument::FindCaseSensitively)?
                                      Qt::CaseSensitive : Qt::CaseInsensitive);
        cur = document->find(regexp,cur,options);
        isFind = cur.isNull()==false;
        /*if(!isFind && (model & FindReplaceDialog::Loop))
        {
            if(options & QTextDocument::FindBackward)
                cur.movePosition(QTextCursor::End);
            else
                cur.movePosition(QTextCursor::Start);
            isFind = document->find(regexp,cur,options).isNull()==false;
        }*/
    }
    else {
        cur = document->find(pattern,cur,options);
        isFind = cur.isNull()==false;
        /*if(!isFind && (model & FindReplaceDialog::Loop))
        {
            if(options & QTextDocument::FindBackward)
                cur.movePosition(QTextCursor::End);
            else
                cur.movePosition(QTextCursor::Start);
            isFind = document->find(pattern,cur,options).isNull()==false;
        }*/
    }
    return isFind;
}

//将文件路径添加到历史
void MainWindow::addToHistory(QString fileName)
{
    int index = history.indexOf(fileName);
    if(index<0){
        history.prepend(fileName);
        QAction * action = new QAction(fileName,this);
        action->setData(fileName);
        ui->menu_History->insertAction(ui->menu_History->actions().first(),
                                       action);
    }
    else {
        history.move(index,0);
        QAction * action = ui->menu_History->actions().value(index);
        ui->menu_History->removeAction(action);
        ui->menu_History->insertAction(ui->menu_History->actions().first(),
                                       action);
    }
}


/** 事件处理函数 **/

// 关闭事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();// 先执行多文档区域的关闭操作
    if(ui->mdiArea->currentSubWindow()){
        event->ignore();// 如果还有窗口没有关闭，则忽略该事件
    }
    else {
        writeSettings();// 在关闭前写入窗口设置
        event->accept();
    }
}

//拖入事件
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())// 数据中是否包含URL，如果是则接收动作，否则忽略该事件
        event->acceptProposedAction();
    else event->ignore();
}

//放下事件
void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData * mimeData = event->mimeData();// 获取MIME数据
    if(mimeData->hasUrls()) // 如果数据中包含URL
    {
        QList<QUrl> urlList = mimeData->urls();// 获取URL列表
        QString fileName = urlList.at(0).toLocalFile();// 将其中第一个URL表示为本地文件路径
        openFile(fileName);
    }
}

/** 反应槽 **/

//更新菜单
void MainWindow::updateMenus()
{
    MdiChild * child = activeMdiChild();
    bool hasMdiChild = (child != 0); //是否有活动窗口
    ui->action_Save->setEnabled(hasMdiChild);    //设置各个动作是否可用
    ui->action_SaveAs->setEnabled(hasMdiChild);
    ui->action_Paste->setEnabled(hasMdiChild);
    ui->action_Find->setEnabled(hasMdiChild);
    ui->action_Replace->setEnabled(hasMdiChild);
    ui->action_WrapLine->setEnabled(hasMdiChild);
    ui->action_ZoomIn->setEnabled(hasMdiChild);
    ui->action_ZoomOut->setEnabled(hasMdiChild);
    ui->action_Close->setEnabled(hasMdiChild);
    ui->action_CloseAll->setEnabled(hasMdiChild);
    ui->action_Next->setEnabled(hasMdiChild);
    ui->action_Previous->setEnabled(hasMdiChild);
    ui->action_Bom->setEnabled(hasMdiChild);
    menuCodec->setEnabled(hasMdiChild);
    menuTranscode->setEnabled(hasMdiChild);

    bool hasSelection = (hasMdiChild
                         && child->textCursor().hasSelection());

    // 有活动窗口且有被选择的文本，剪切复制才可用
    ui->action_Cut->setEnabled(hasSelection);
    ui->action_Copy->setEnabled(hasSelection);

    // 有活动窗口且文档有可撤销操作
    ui->action_Undo->setEnabled(hasMdiChild
                          && child->document()->isUndoAvailable());

    // 有活动窗口且文档有可恢复操作
    ui->action_Redo->setEnabled(hasMdiChild
                          && child->document()->isRedoAvailable());

    // 有活动窗口且根据活动窗口的信息更新
    ui->action_Bom->setChecked(hasMdiChild
                               && child->hasBom());

    if(hasMdiChild){
        if(codecGroup->checkedAction()->data().toByteArray()!=child->textCodec()->name()){
            QList<QAction*> acts =  codecGroup->actions();
            QByteArray codeName = child->textCodec()->name();
            foreach (QAction * act, acts) {
                if(act->data().toByteArray()==codeName)
                    act->setChecked(true);
            }
        }
    }
}

// 创建子窗口部件
MdiChild * MainWindow::createMdiChild()
{
    MdiChild * child = new MdiChild;

    //向多文档区域添加子窗口，child为中心部件
    ui->mdiArea->addSubWindow(child);
    connect(child,SIGNAL(closeWindow(QString)),this,SLOT(closeMdiChild(QString)),Qt::QueuedConnection);

    // 根据QTextEdit类的是否可以复制信号设置剪切复制动作是否可用
    connect(child,SIGNAL(copyAvailable(bool)),ui->action_Cut,
            SLOT(setEnabled(bool)));
    connect(child,SIGNAL(copyAvailable(bool)),ui->action_Copy,
            SLOT(setEnabled(bool)));

    // 根据QTextDocument类的是否可以撤销恢复信号设置撤销恢复动作是否可用
    connect(child->document(),SIGNAL(undoAvailable(bool)),
            ui->action_Undo,SLOT(setEnabled(bool)));
    connect(child->document(),SIGNAL(redoAvailable(bool)),
            ui->action_Redo,SLOT(setEnabled(bool)));

    // 每当编辑器中的光标位置改变，就重新显示行号和列号
    connect(child,SIGNAL(cursorPositionChanged()),this,SLOT(showTextInfo()));

    //自动换行？
    child->setLineWrapMode(lineWrapMode);
    //文字大小
    child->setTextSize(textSize);

    //关闭子窗口的拖放接收
    child->setAcceptDrops(false);

    return child;
}

// 设置活动子窗口
void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window) // 如果传递了窗口部件，则将其设置为活动窗口
        return;
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

// 显示文本的行号和列号
void MainWindow::showTextInfo()
{
    // 如果有活动窗口，则显示其中光标所在的位置
    if(activeMdiChild()){
        // 获取的行号和列号都是从0开始的
        int rowNum = activeMdiChild()->textCursor().blockNumber()+1;
        int colNum = activeMdiChild()->textCursor().positionInBlock()+1;

        ui->statusBar->showMessage(tr("%1行 %2列")
                                   .arg(rowNum).arg(colNum));
    }
}

//关闭指定的子窗口
void MainWindow::closeMdiChild(QString fileName)
{
    QMdiSubWindow * child = findMdiChild(fileName);
    if(0!=child)
    {
        ui->mdiArea->setActiveSubWindow(child);
        ui->mdiArea->closeActiveSubWindow();
    }
}

//打开文件
void MainWindow::openFile(QAction *action)
{
    if(!action->data().isNull()){
        QString fileName = action->data().toString();
        if(!QFileInfo(fileName).exists()){
            if(QMessageBox::Yes==QMessageBox::warning(this,tr("路径不存在！"),
                                 tr("是否删除该记录"),
                                 QMessageBox::Yes,QMessageBox::No))
            {
                history.removeOne(fileName);
                ui->menu_History->removeAction(action);
            }
            return;
        }
        openFile(fileName);
    }
}

//按规则在编辑器查找下一个
bool MainWindow::findByCursor(QString pattern, QTextDocument::FindFlags options, FindReplaceDialog::FindModel model)
{
    MdiChild * child = activeMdiChild();
    bool isFind;
    if(model & FindReplaceDialog::RegExp)
    {
        QRegExp regexp(pattern);
        if(!regexp.isValid())
        {
            QMessageBox::warning(frDialog,tr("正则表达式错误！"),
                                 tr("原因: %1").arg(regexp.errorString()));
            return false;
        }
        regexp.setCaseSensitivity((options & QTextDocument::FindCaseSensitively)?
                                      Qt::CaseSensitive : Qt::CaseInsensitive);
        isFind = child->find(regexp,options);
        if(!isFind && (model & FindReplaceDialog::Loop))
        {
            if(options & QTextDocument::FindBackward)
                child->moveCursor(QTextCursor::End);
            else
                child->moveCursor(QTextCursor::Start);
            isFind = child->find(regexp,options);
        }
    }
    else {
        isFind = child->find(pattern,options);
        if(!isFind && (model & FindReplaceDialog::Loop))
        {
            if(options & QTextDocument::FindBackward)
                child->moveCursor(QTextCursor::End);
            else
                child->moveCursor(QTextCursor::Start);
            isFind = child->find(pattern,options);
        }
    }
    if(!isFind) ui->statusBar->showMessage(tr("找不到下一个"),10000);
    return isFind;
}

//按规则计数
int MainWindow::counter(QString pattern, QTextDocument::FindFlags options, FindReplaceDialog::FindModel model)
{
    MdiChild * child = activeMdiChild();
    if(model & FindReplaceDialog::RegExp)
    {
        QRegExp regexp(pattern);
        if(!regexp.isValid())
        {
            QMessageBox::warning(frDialog,tr("正则表达式错误！"),
                                 tr("原因: %1").arg(regexp.errorString()));
            return -1;
        }
    }
    ui->statusBar->showMessage(tr("查找：正在计数..."));
    int count=0;
    for(QTextCursor cur(child->document());
        findInDocuent(cur,pattern,options,model);
        ++count);
    ui->statusBar->showMessage(tr("查找：共找到 %1 个").arg(count));
    return count;
}

//替换
bool MainWindow::replace(QString to, QString pattern, QTextDocument::FindFlags options, FindReplaceDialog::FindModel model)
{
    MdiChild * child = activeMdiChild();
    QString select = child->textCursor().selectedText();
    Qt::CaseSensitivity cs = (options & QTextDocument::FindCaseSensitively)?
                Qt::CaseSensitive : Qt::CaseInsensitive;
    if(model & FindReplaceDialog::RegExp)
    {
        QRegularExpression regexp(pattern);
        if(!regexp.isValid())
        {
            QMessageBox::warning(frDialog,tr("正则表达式错误！"),
                                 tr("原因: %1").arg(regexp.errorString()));
            return false;
        }
        QRegularExpression::PatternOptions patternOption = regexp.patternOptions();
        if(!(options & QTextDocument::FindCaseSensitively))
            regexp.setPatternOptions(patternOption | QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;//确保所选文本与要替换的相符
        if(0==select.indexOf(regexp,0,&match)){
            if(0==select.compare(match.captured(),cs)){
                child->textCursor().insertText(to);
            }
            else {
                return findByCursor(pattern,options,model);
            }
        }
        else {
            return findByCursor(pattern,options,model);
        }
    }
    else {
        if(0==select.compare(pattern,cs))//确保所选文本与要替换的相符
        {
            child->textCursor().insertText(to);
        }
        else {
            return findByCursor(pattern,options,model);
        }
    }
    return true;
}

//替换全部
int MainWindow::replaceAll(QString to, QString pattern, QTextDocument::FindFlags options, FindReplaceDialog::FindModel model)
{
    MdiChild * child = activeMdiChild();
    if(model & FindReplaceDialog::RegExp)
    {
        QRegExp regexp(pattern);
        if(!regexp.isValid())
        {
            QMessageBox::warning(frDialog,tr("正则表达式错误！"),
                                 tr("原因: %1").arg(regexp.errorString()));
            return -1;
        }
    }
    ui->statusBar->showMessage(tr("替换：正在替换..."));
    int count=0;
    for(QTextCursor cur(child->document());
        findInDocuent(cur,pattern,options,model);
        ++count){
        cur.insertText(to);
    }
    ui->statusBar->showMessage(tr("替换：共替换了 %1 个").arg(count));
    return count;
}

//设置当前文件的编码方式
void MainWindow::setCurrentCode(QAction *act)
{
    if(0!=activeMdiChild()){
        QByteArray name = act->data().toByteArray();
        if(0==std::strncmp(name,"System",strlen("System")+1))
        {
            activeMdiChild()->setCodec(QTextCodec::codecForLocale());
        }
        else {
            QTextCodec * code = QTextCodec::codecForName(name);
            if(code!=0) activeMdiChild()->setCodec(code);
        }
    }
}

//将当前文件的编码方式转换为
void MainWindow::transCurrentCode(QAction *act)
{
    if(0!=activeMdiChild()){
        QByteArray name = act->data().toByteArray();
        QTextCodec * code = QTextCodec::codecForName(name);
        if(code!=0) activeMdiChild()->transCodec(code);
    }
}

//清空文件历史
void MainWindow::clearHistory()
{
    for (;!history.isEmpty();history.removeFirst()){
        QAction * act = ui->menu_History->actions().first();
        ui->menu_History->removeAction(act);
    }
}

/** 自动关联的槽 **/

void MainWindow::on_action_New_triggered()
{
    MdiChild * child = createMdiChild();
    child->newFile();
    if(1 == ui->mdiArea->subWindowList().size()) child->showMaximized();
    else child->show();
}

void MainWindow::on_action_Open_triggered()
{
    // 获取文件路径
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开文件"),".",
                                                    tr("文本文件(*.txt);;任意文件(*)"));
    openFile(fileName);
}

void MainWindow::on_action_Save_triggered()
{
    MdiChild *child = activeMdiChild();
    if(child && child->save()){
        ui->statusBar->showMessage(tr("文件保存成功"),2000);
        addToHistory(child->currentFile());
    }
}

void MainWindow::on_action_SaveAs_triggered()
{
    MdiChild *child = activeMdiChild();
    if(child && child->saveAs()){
        ui->statusBar->showMessage(tr("文件保存成功"),2000);
        addToHistory(child->currentFile());
    }
}

void MainWindow::on_action_Undo_triggered()
{
    if(activeMdiChild()) activeMdiChild()->undo();
}
void MainWindow::on_action_Redo_triggered()
{
    if(activeMdiChild()) activeMdiChild()->redo();
}

void MainWindow::on_action_Cut_triggered()
{
    if(activeMdiChild()) activeMdiChild()->cut();
}

void MainWindow::on_action_Copy_triggered()
{
    if(activeMdiChild()) activeMdiChild()->copy();
}

void MainWindow::on_action_Paste_triggered()
{
    if(activeMdiChild()) activeMdiChild()->paste();
}

void MainWindow::on_action_Close_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_action_CloseAll_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_action_Next_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_action_Previous_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this,tr("关于本软件"),
                       tr("源码地址：<html><body><a href=\"https://github.com/miRoox/Multiple-Document-Editor\">GitHub</a></body></html>"));
}

void MainWindow::on_action_Exit_triggered()
{
    qApp->closeAllWindows();
}

void MainWindow::on_action_WrapLine_toggled(bool checked)
{
    lineWrapMode = checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap;
    foreach(QMdiSubWindow * window,ui->mdiArea->subWindowList()){
        MdiChild * child = qobject_cast<MdiChild *>(window->widget());
        child->setLineWrapMode(lineWrapMode);
    }
}

void MainWindow::on_action_ZoomIn_triggered()
{
    textSize++;
    foreach(QMdiSubWindow * window,ui->mdiArea->subWindowList()){
        MdiChild * child = qobject_cast<MdiChild *>(window->widget());
        child->setTextSize(textSize);
    }
}

void MainWindow::on_action_ZoomOut_triggered()
{
    if(textSize>1) textSize--;
    foreach(QMdiSubWindow * window,ui->mdiArea->subWindowList()){
        MdiChild * child = qobject_cast<MdiChild *>(window->widget());
        child->setTextSize(textSize);
    }
}

void MainWindow::on_action_AboutQt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_action_Find_triggered()
{
    frDialog->showTab(0);
}

void MainWindow::on_action_Replace_triggered()
{
    frDialog->showTab(1);
}

void MainWindow::on_action_Bom_toggled(bool checked)
{
    if(activeMdiChild()!=0)
        if(activeMdiChild()->hasBom()!=checked)
            activeMdiChild()->setBom(checked);
}
