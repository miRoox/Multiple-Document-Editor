#include "pluginmanager_p.h"
#include "ipluginbase.h"
#include "ieditor.h"
#include <mdewidget/mdewindow.h>
#include <QSettings>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QVBoxLayout>
#include <QMenu>
#include <QDialog>
#include <QLineEdit>
#include <QTableView>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QIcon>
#include <QDebug>

PluginManagerPrivate::PluginManagerPrivate(PluginManager *parent)
    : QObject(parent),
      p(parent),
      plugins(),
      editors(),
      disabledPlugins(),
      mapper(),
      suffixDesc()
{
    win = 0;
    coreSettings = 0;
    pluginSelectDialog = 0;
}

void PluginManagerPrivate::loadSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    qInfo() << "Plugin manager: loading settings..";
    //disabled plugins
    settings.beginGroup("disabled");
    qInfo() << "loading disabled plugins list..";
    foreach (QString index, settings.allKeys()) {
        PluginSpec spec =  specFromVariantHash(settings.value(index).toHash());
        disabledPlugins.insert(spec);
    }
    settings.endGroup();
    //editor mapper
    settings.beginGroup("mapper");
    qInfo() << "loading file-editor mapper..";
    foreach (QString suffix, settings.allKeys()) {
        PluginSpec spec = specFromVariantHash(settings.value(suffix).toHash());
        if(disabledPlugins.contains(spec))
            continue;
        mapper.insert(suffix,spec);
    }
    settings.endGroup();
    settings.endGroup();
    qInfo() << "Plugin manager: settings are loaded.";
}

void PluginManagerPrivate::saveSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    qInfo() << "Plugin manager: saving settings..";
    //disabled plugins
    settings.beginGroup("disabled");
    settings.remove("");
    qInfo() << "saving disabled plugins list..";
    int i = 0;
    foreach (PluginSpec spec, disabledPlugins.toList()) {
        settings.setValue(QString("%1").arg(i++),variantHashFromSpec(spec));
    }
    settings.endGroup();
    //editor mapper
    settings.beginGroup("mapper");
    settings.remove("");
    qInfo() << "saving file-editor mapper..";
    foreach (QString suffix, mapper.keys()) {
        settings.setValue(suffix,variantHashFromSpec(mapper.value(suffix)));
    }
    settings.endGroup();
    settings.endGroup();
    qInfo() << "Plugin manager: settings are saved.";
}

void PluginManagerPrivate::initViewer()
{
    //Plugin selection dialog
    pluginSelectDialog = new QDialog(win);
    pluginSelectDialog->setWindowTitle(tr("Plugin manager"));

    QTableView * view = new QTableView(pluginSelectDialog);
    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel;
    auto sourceModel = createPluginSpecModel(plugins.keys());
    proxyModel->setSourceModel(sourceModel);
    view->setModel(proxyModel);
    view->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    for (int column = 0; column < proxyModel->columnCount(); ++column)
        view->resizeColumnToContents(column);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHeaderView * headerView = view->horizontalHeader();
    headerView->setSectionsClickable(true);
    headerView->setSortIndicatorShown(true);
    //auto change the sort indicator when section click
//    connect(headerView,QHeaderView::sortIndicatorChanged,[](int logicalIndex, Qt::SortOrder order){
//        qDebug() << "sort indicator change" << logicalIndex << static_cast<int>(order);
//    });
    connect(headerView,QHeaderView::sortIndicatorChanged,
            proxyModel,QSortFilterProxyModel::sort);

    QLineEdit * filter = new QLineEdit(pluginSelectDialog);
    filter->setPlaceholderText(tr("Filter"));
    connect(filter,QLineEdit::textChanged,
            proxyModel,static_cast<void(QSortFilterProxyModel::*)(const QString &)>(&QSortFilterProxyModel::setFilterRegExp));

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
                                                        pluginSelectDialog);
    connect(buttonBox->button(QDialogButtonBox::Cancel),QPushButton::clicked,
            pluginSelectDialog,QDialog::close);
    connect(buttonBox->button(QDialogButtonBox::Ok),QPushButton::clicked,[=]{
        for (int row = 0; row < proxyModel->rowCount(); ++row) {
            PluginSpec spec = specFromIndex(sourceModel,sourceModel->index(row,0));
            if(sourceModel->item(row)->checkState()==Qt::Checked) {
                if(!this->disabledPlugins.remove(spec))
                    qWarning() << "Plugin" << spec << "has not been disabled";
            }
            else {
                this->disabledPlugins.insert(spec);
            }
            QMessageBox::information(pluginSelectDialog,tr("Need to restart"),
                                     tr("Need to restart to take effect"),
                                     QMessageBox::Ok);
        }
        pluginSelectDialog->close();
    });

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(filter);
    layout->addWidget(view);
    layout->addWidget(buttonBox);
    layout->setSpacing(5);
    layout->setMargin(10);
    pluginSelectDialog->setLayout(layout);

}

void PluginManagerPrivate::checkDisabled()
{
    disabledPlugins &= QSet<PluginSpec>::fromList(plugins.keys());
}

void PluginManagerPrivate::checkMapper()
{
    foreach (QString suffix, mapper.keys()) {
        if(!editors.contains(mapper.value(suffix)))
            mapper.remove(suffix);
    }
    if(!mapper.contains(DEFEDITSUFX)) { //default editor
        if(mapper.contains("txt"))
            p->setDefaultEditor(mapper.value("txt"));
    }
}

QList<QStandardItem *> PluginManagerPrivate::createPluginSpecItem(const PluginManagerPrivate::PluginSpec &spec)
{
    QList<QStandardItem *> items;
    auto nameItem = new QStandardItem(spec.value(PLUGINMETADATA_NAME));
    nameItem->setCheckable(true);
    if(disabledPlugins.contains(spec))
        nameItem->setCheckState(Qt::Unchecked);
    else
        nameItem->setCheckState(Qt::Checked);
    items.append(nameItem);
    if(disabledPlugins.contains(spec))
        items.append(new QStandardItem(tr("disabled")));
    else if (editors.contains(spec))
        items.append(new QStandardItem(tr("editor")));
    else
        items.append(new QStandardItem(tr("utilities")));
    items.append(new QStandardItem(spec.value(PLUGINMETADATA_VER)));
    items.append(new QStandardItem(spec.value(PLUGINMETADATA_VENDOR)));
    items.append(new QStandardItem(spec.value(PLUGINMETADATA_PLATFORM)));
    return items;
}

QStandardItemModel *PluginManagerPrivate::createPluginSpecModel(const QList<PluginManagerPrivate::PluginSpec> &specs)
{
    QStandardItemModel * model = new QStandardItemModel;
    QStringList headers;
    headers << tr(PLUGINMETADATA_NAME)
            << tr("category")
            << tr(PLUGINMETADATA_VER)
            << tr(PLUGINMETADATA_VENDOR)
            << tr(PLUGINMETADATA_PLATFORM);
    model->setHorizontalHeaderLabels(headers);
    foreach (PluginSpec spec, specs) {
        model->appendRow(createPluginSpecItem(spec));
    }
    return model;
}

PluginManagerPrivate::PluginSpec PluginManagerPrivate::specFromIndex(const QStandardItemModel *model, QModelIndex index)
{
    const int row = index.row();
    PluginSpec spec;
    spec.insert(PLUGINMETADATA_NAME,model->item(row,0)->data(Qt::DisplayRole).toString());
    spec.insert(PLUGINMETADATA_VER,model->item(row,2)->data(Qt::DisplayRole).toString());
    spec.insert(PLUGINMETADATA_VENDOR,model->item(row,3)->data(Qt::DisplayRole).toString());
    spec.insert(PLUGINMETADATA_PLATFORM,model->item(row,4)->data(Qt::DisplayRole).toString());
    return spec;
}

PluginManagerPrivate::PluginSpec PluginManagerPrivate::specFromVariantHash(const QVariantHash &data)
{
    PluginSpec spec;
    spec.insert(PLUGINMETADATA_NAME,data.value(PLUGINMETADATA_NAME).toString());
    spec.insert(PLUGINMETADATA_VER,data.value(PLUGINMETADATA_VER).toString());
    spec.insert(PLUGINMETADATA_VENDOR,data.value(PLUGINMETADATA_VENDOR).toString());
    spec.insert(PLUGINMETADATA_PLATFORM,data.value(PLUGINMETADATA_PLATFORM).toString());
    return spec;
}

QVariantHash PluginManagerPrivate::variantHashFromSpec(const PluginManagerPrivate::PluginSpec &spec)
{
    QVariantHash data;
    data.insert(PLUGINMETADATA_NAME,spec.value(PLUGINMETADATA_NAME));
    data.insert(PLUGINMETADATA_VER,spec.value(PLUGINMETADATA_VER));
    data.insert(PLUGINMETADATA_VENDOR,spec.value(PLUGINMETADATA_VENDOR));
    data.insert(PLUGINMETADATA_PLATFORM,spec.value(PLUGINMETADATA_PLATFORM));
    return data;
}
