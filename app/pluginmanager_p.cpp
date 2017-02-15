#include "pluginmanager_p.h"
#include "ipluginbase.h"
#include "ieditor.h"
#include "mdewindow.h"
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
}

void PluginManagerPrivate::loadSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    qInfo() << "Plugin manager: loading settings..";
    //disabled plugins
    int size = settings.beginReadArray("disabled");
    qInfo() << "loading disabled plugins list..";
    for(int i=0; i<size; ++i) {
        settings.setArrayIndex(i);
        PluginSpec spec =  specFromVariantHash(settings.value("spec").toHash());
        disabledPlugins.insert(spec);
    }
    settings.endArray();
    //editor mapper
    size = settings.beginReadArray("mapper");
    qInfo() << "loading file-editor mapper..";
    for(int i=0; i<size; ++i) {
        settings.setArrayIndex(i);
        PluginSpec spec = specFromVariantHash(settings.value("spec").toHash());
        if(disabledPlugins.contains(spec))
            continue;
        QString suffix = settings.value("suffix").toString();
        mapper.insert(suffix,spec);
    }
    settings.endArray();
    settings.endGroup();
    qInfo() << "Plugin manager: settings are loaded.";
}

void PluginManagerPrivate::saveSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");
    qInfo() << "Plugin manager: saving settings..";
    //disabled plugins
    settings.beginWriteArray("disabled");
    qInfo() << "saving disabled plugins list..";
    int i = 0;
    foreach (PluginSpec spec, disabledPlugins.toList()) {
        settings.setArrayIndex(i++);
        settings.setValue("spec",variantHashFromSpec(spec));
    }
    settings.endArray();
    //editor mapper
    settings.beginWriteArray("mapper");
    qInfo() << "saving file-editor mapper..";
    i = 0;
    foreach (QString suffix, mapper.keys()) {
        settings.setArrayIndex(i++);
        settings.setValue("suffix",suffix);
        settings.setValue("spec",variantHashFromSpec(mapper.value(suffix)));
    }
    settings.endArray();
    settings.endGroup();
    qInfo() << "Plugin manager: settings are saved.";
}

void PluginManagerPrivate::initViewer()
{
    QDialog * dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    QTableView * view = new QTableView(dialog);
    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(createPluginSpecModel(plugins.keys()));
    view->setModel(proxyModel);
    view->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    for (int column = 0; column < proxyModel->columnCount(); ++column)
        view->resizeColumnToContents(column);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHeaderView * headerView = view->horizontalHeader();
    headerView->setSectionsClickable(true);
    headerView->setSortIndicatorShown(true);
    connect(headerView,QHeaderView::sectionClicked,[headerView](int index){
        if(headerView->sortIndicatorOrder()==Qt::AscendingOrder)
            headerView->setSortIndicator(index,Qt::DescendingOrder);
        else
            headerView->setSortIndicator(index,Qt::AscendingOrder);
    });
    connect(headerView,QHeaderView::sortIndicatorChanged,
            proxyModel,QSortFilterProxyModel::sort);

    QLineEdit * filter = new QLineEdit(dialog);
    filter->setPlaceholderText(tr("Filter"));
    connect(filter,QLineEdit::textChanged,
            proxyModel,static_cast<void(QSortFilterProxyModel::*)(const QString &)>(&QSortFilterProxyModel::setFilterRegExp));

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Close,dialog);
    QPushButton * enableButton = buttonBox->addButton(tr("Enable"),QDialogButtonBox::ActionRole);
    QPushButton * disableButton = buttonBox->addButton(tr("Disable"),QDialogButtonBox::ActionRole);
    connect(enableButton,QPushButton::clicked,view,[=]{
        QModelIndexList rows = view->selectionModel()->selectedRows();
        auto prxModel = qobject_cast<QSortFilterProxyModel *>(view->model());
        if(!prxModel) qWarning() << "prxModel convert failed";
        auto model = qobject_cast<QStandardItemModel*>(prxModel->sourceModel());
        if(!model) qWarning() << "model convert failed";
        foreach (QModelIndex index, rows) {
            PluginSpec spec = specFromIndex(model,index);
            if(disabledPlugins.remove(spec))
                model->setData(index,QIcon(":/myImage/images/enable.png"),Qt::DecorationRole);
        }
    });
    connect(disableButton,QPushButton::clicked,view,[=]{
        QModelIndexList rows = view->selectionModel()->selectedRows();
        auto prxModel = qobject_cast<QSortFilterProxyModel *>(view->model());
        if(!prxModel) qWarning() << "prxModel convert failed";
        auto model = qobject_cast<QStandardItemModel*>(prxModel->sourceModel());
        if(!model) qWarning() << "model convert failed";
        foreach (QModelIndex index, rows) {
            PluginSpec spec = specFromIndex(model,index);
            disabledPlugins.insert(spec);
            model->setData(index,QIcon(":/myImage/images/disable.png"),Qt::DecorationRole);
        }
    });

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(filter);
    layout->addWidget(view);
    layout->addWidget(buttonBox);
    layout->setSpacing(5);
    layout->setMargin(10);
    dialog->setLayout(layout);

    auto managerView = win->menuSettings()->addAction(tr("Plugin Manager"));
    connect(managerView,QAction::triggered,dialog,QDialog::exec);
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
    if(disabledPlugins.contains(spec))
        items.append(new QStandardItem(QIcon(":/myImage/images/disable.png"),
                                       spec.value(PLUGINMETADATA_NAME)));
    else
        items.append(new QStandardItem(QIcon(":/myImage/images/enable.png"),
                                       spec.value(PLUGINMETADATA_NAME)));
    items.append(new QStandardItem(spec.value(SPECKEY_CATEGORY)));
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
            << tr(SPECKEY_CATEGORY)
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
    spec.insert(SPECKEY_CATEGORY,model->item(row,1)->data(Qt::DisplayRole).toString());
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
