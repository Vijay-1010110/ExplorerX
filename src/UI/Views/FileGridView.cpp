#include "FileGridView.h"
#include "../ViewModels/FileItemModel.h"
#include <QHeaderView>

FileGridView::FileGridView(QWidget *parent)
    : QTableView(parent) {
    // UI Virtualization: Lock vertical header sizes to avoid recalculation
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(24);
    
    // Drag and Drop support
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    
    setupDummyModel();
}

FileGridView::~FileGridView() = default;

void FileGridView::setupDummyModel() {
    auto *model = new FileItemModel(this);
    setModel(model);
    
    // Stretch the name column
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}
