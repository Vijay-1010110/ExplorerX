#include "DirectoryTreeView.h"
#include "../ViewModels/DirectoryItemModel.h"

DirectoryTreeView::DirectoryTreeView(QWidget *parent)
    : QTreeView(parent) {
    setUniformRowHeights(true); // UI Virtualization: Optimize row heights
    
    // Drag and Drop support
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);

    setupDummyModel();
}

DirectoryTreeView::~DirectoryTreeView() = default;

void DirectoryTreeView::setupDummyModel() {
    auto *model = new DirectoryItemModel(this);
    setModel(model);
    expandAll();
}
