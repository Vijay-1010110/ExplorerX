#include "DirectoryTreeView.h"
#include <QStandardItemModel>

DirectoryTreeView::DirectoryTreeView(QWidget *parent)
    : QTreeView(parent) {
    setupDummyModel();
}

DirectoryTreeView::~DirectoryTreeView() = default;

void DirectoryTreeView::setupDummyModel() {
    auto *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({QStringLiteral("Directories")});
    
    QStandardItem *rootItem = model->invisibleRootItem();
    
    QStandardItem *cDrive = new QStandardItem(QStringLiteral("C: Drive"));
    QStandardItem *windows = new QStandardItem(QStringLiteral("Windows"));
    QStandardItem *users = new QStandardItem(QStringLiteral("Users"));
    cDrive->appendRow(windows);
    cDrive->appendRow(users);
    
    QStandardItem *dDrive = new QStandardItem(QStringLiteral("D: Drive"));
    QStandardItem *documents = new QStandardItem(QStringLiteral("Documents"));
    dDrive->appendRow(documents);
    
    rootItem->appendRow(cDrive);
    rootItem->appendRow(dDrive);
    
    setModel(model);
    expandAll();
}
