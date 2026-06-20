#include "DirectoryTreeView.h"
#include "../ViewModels/DirectoryItemModel.h"

DirectoryTreeView::DirectoryTreeView(QWidget *parent)
    : QTreeView(parent) {
    setUniformRowHeights(true); // UI Virtualization: Optimize row heights
    setupDummyModel();
}

DirectoryTreeView::~DirectoryTreeView() = default;

void DirectoryTreeView::setupDummyModel() {
    auto *model = new DirectoryItemModel(this);
    setModel(model);
    expandAll();
}
