#include "FileGridView.h"
#include "../ViewModels/FileItemModel.h"
#include <QHeaderView>

FileGridView::FileGridView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                           std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                           std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                           QWidget *parent)
    : QListView(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)), m_thumbOrchestrator(std::move(thumbOrchestrator)) {
    
    // Icon Grid Mode Configuration
    setViewMode(QListView::IconMode);
    setIconSize(QSize(96, 96));
    setResizeMode(QListView::Adjust);
    setSpacing(10);
    setGridSize(QSize(120, 140)); // Give enough space for icon + text
    setWordWrap(true);
    setUniformItemSizes(true);
    
    // Drag and Drop support
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    
    setupRealModel();
}

FileGridView::~FileGridView() = default;

void FileGridView::setupRealModel() {
    m_model = new FileItemModel(m_provider, m_searchEngine, m_thumbOrchestrator, this);
    setModel(m_model);
    m_model->loadPath("C:\\");
}

void FileGridView::setSearchResults(std::vector<ExplorerX::Domain::FileItem> results) {
    if (m_model) {
        m_model->setSearchResults(std::move(results));
    }
}

void FileGridView::loadPath(const QString& path) {
    if (m_model) {
        m_model->loadPath(path.toStdString());
    }
}
