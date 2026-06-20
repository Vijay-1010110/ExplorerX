#include "FileGridView.h"
#include "../ViewModels/FileItemModel.h"
#include <QHeaderView>

FileGridView::FileGridView(QWidget *parent)
    : QTableView(parent) {
    // UI Virtualization: Lock vertical header sizes to avoid recalculation
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(24);
    
    setupDummyModel();
}

FileGridView::~FileGridView() = default;

void FileGridView::setupDummyModel() {
    auto *model = new FileItemModel(this);
    setModel(model);
    
    // Stretch the name column
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}
