#include "DirectoryTreeView.h"
#include "../ViewModels/DirectoryItemModel.h"

DirectoryTreeView::DirectoryTreeView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider, QWidget *parent)
    : QTreeView(parent), m_provider(std::move(provider)) {
    setUniformRowHeights(true); // UI Virtualization: Optimize row heights
    
    // Drag and Drop support
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);

    setupRealModel();
}

DirectoryTreeView::~DirectoryTreeView() = default;

void DirectoryTreeView::setupRealModel() {
    m_model = new DirectoryItemModel(m_provider, this);
    setModel(m_model);
    m_model->loadPath("C:\\"); // Default root
    expandAll();
}
