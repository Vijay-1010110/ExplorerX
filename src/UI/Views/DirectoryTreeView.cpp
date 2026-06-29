#include "DirectoryTreeView.h"
#include "../ViewModels/DirectoryItemModel.h"

DirectoryTreeView::DirectoryTreeView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider, QWidget *parent)
    : QTreeView(parent), m_provider(std::move(provider)) {
    setUniformRowHeights(true); // UI Virtualization: Optimize row heights
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent, false);
    viewport()->setAutoFillBackground(false);
    
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

void DirectoryTreeView::scrollContentsBy(int dx, int dy) {
    // 100% guarantee that Qt does NOT blit the viewport when scrolling, 
    // even if the QSS engine attempts to force WA_OpaquePaintEvent back to true.
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent, false);
    viewport()->setAutoFillBackground(false);
    
    QTreeView::scrollContentsBy(dx, dy);
}
