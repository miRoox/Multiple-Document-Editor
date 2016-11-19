#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QFileSystemWatcher>
#include <QTextCodec>

class QFile;

class MdiChild : public QTextEdit
{
    Q_OBJECT

public:
    explicit MdiChild(QWidget *parent = 0);
    void newFile();                         // 新建操作
    bool loadFile(const QString &fileName,bool re); // 加载文件
    bool save();                            // 保存操作
    bool saveAs();                          // 另存为操作
    bool saveFile(const QString &fileName); // 保存文件
    QString userFriendlyCurrentFile();      // 提取文件名
    QString currentFile();                   // 返回当前文件路径
    void setTextSize(int size);           // 设置文本字体大小
    void setModified(bool is);              // 保持未修改

    QTextCodec * textCodec() const {return codec;}//文本编码
    bool hasBom() { return bom; }                //是否有BOM头

protected:
    void closeEvent(QCloseEvent *event);    // 关闭事件
    void contextMenuEvent(QContextMenuEvent *e); //右键菜单事件
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE; //调整大小事件

public slots:
    void setCodec(QTextCodec * codec);      //设置编码方式
    void setBom(bool bom);                  //设置BOM
    void transCodec(QTextCodec * codec);    //转换编码

private slots:
    void documentWasModified();             // 文档被更改时，显示更改状态标志
    void changeMessage(const QString &fileName); //文件被外部更改时
    void highlightCurrentLine();            //光标移动时

signals:
    void closeWindow(QString);             //向主窗口发射的关闭此窗口信号

private:
    bool maybeSave();                       // 是否需要保存
    void setCurrentFile(const QString &fileName);  // 设置当前文件
    QString curFile;                        // 保存当前文件路径
    bool isUntitled;                        // 作为当前文件是否被保存到硬盘上的标志
    QFileSystemWatcher fWatcher;            //监听文件是否被修改
    QTextCodec * codec;                     //文件编码
    bool bom;                               //是否添加BOM
};

#endif // MDICHILD_H
