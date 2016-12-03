#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QVariant;
class QWidget;
QT_END_NAMESPACE

class PluginInferface
{
public:
    virtual ~PluginInferface() {}
    virtual QString name() const = 0;
    virtual QStringList operators() const = 0;
    virtual QStringList descriptions(QString opName) const = 0;
    virtual bool getData(QString desc, QVariant data) = 0;
    virtual QStringList apply(QWidget * parent, QString opName) = 0;
};
QT_BEGIN_NAMESPACE

#define PluginInferface_iid "org.miroox.mde-1.1-PluginInferface"
Q_DECLARE_INTERFACE(PluginInferface, PluginInferface_iid)

QT_END_NAMESPACE
#endif // PLUGININTERFACE_H
