#include "htmlviewer.h"
#include <QTextBrowser>
#include <QFileInfo>
#include <QMessageBox>

QString HtmlViewer::name() const
{
    return tr("HTML 预览器");
}

QStringList HtmlViewer::operators() const
{
    return QStringList() << tr("预览当前文件")
                         << tr("关于");
}

QStringList HtmlViewer::descriptions(QString opName) const
{
    if(opName==tr("预览当前文件"))
    {
        return QStringList() << "currentText"
                             << "currentFile";
    }
    else if (opName==tr("关于"))
    {
        return QStringList();
    }
    return QStringList();
}

bool HtmlViewer::getData(QString desc, QVariant data)
{
    if(desc=="currentText")
    {
        content = data.toString();
        return true;
    }
    else if (desc=="currentFile")
    {
        file = data.toString();
        return true;
    }
    return false;
}

QStringList HtmlViewer::apply(QWidget * parent ,QString opName)
{
    if(opName==tr("预览当前文件"))
    {
        if(file.isNull())
        {
            QMessageBox::warning(parent,tr("文件不存在！"),
                                 tr("没有可用的文件。"),QMessageBox::Ok);
        }
        else
        {
            QTextBrowser *viewer = new QTextBrowser(parent);
            viewer->setWindowFlags(Qt::Window);
            viewer->resize(600,480);
            viewer->setWindowTitle(QFileInfo(file).fileName() + " - HTML Viewer");
            viewer->setOpenLinks(false);
            viewer->setOpenExternalLinks(true);
            viewer->setSearchPaths(QStringList(file));
            viewer->setWordWrapMode(QTextOption::NoWrap);
            viewer->setHtml(content);
            if(!viewer->documentTitle().isEmpty())
                viewer->setWindowTitle(viewer->documentTitle() + " - HTML Viewer");
            viewer->show();
        }
    }
    else if (opName==tr("关于"))
        QMessageBox::about(parent,tr("关于本插件"),
                           tr("插件名称：HTML Viewer\n"
                              "插件功能：将HTML文件内容以富文本的方式呈现出来"));
    return QStringList();
}
