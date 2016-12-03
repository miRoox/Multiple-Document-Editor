#ifndef HTMLVIEWER_H
#define HTMLVIEWER_H

#include <plugininterface.h>

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QWidget>

class HtmlViewer :
        public QObject,
        public PluginInferface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.miroox.mde-1.1-PluginInferface" FILE "htmlviewer.json")
    Q_INTERFACES(PluginInferface)

public:
    QString name() const Q_DECL_OVERRIDE;
    QStringList operators() const Q_DECL_OVERRIDE;
    QStringList descriptions(QString opName) const Q_DECL_OVERRIDE;
    bool getData(QString desc, QVariant data) Q_DECL_OVERRIDE;
    QStringList apply(QWidget * parent, QString opName) Q_DECL_OVERRIDE;

private:
    QString content;
    QString file;
};

#endif // HTMLVIEWER_H
