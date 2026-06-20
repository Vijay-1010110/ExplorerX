#include "FileGridView.h"
#include "../ViewModels/FileItemModel.h"
#include <QHeaderView>

FileGridView::FileGridView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                           std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                           QWidget *parent)
    : QTableView(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)) {
    // UI Virtualization: Lock vertical header sizes to avoid recalculation
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(24);
    
    // Drag and Drop support
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    
    setupRealModel();
    
    // Stretch the name column
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

FileGridView::~FileGridView() = default;

void FileGridView::setupRealModel() {
    m_model = new FileItemModel(m_provider, m_searchEngine, this);
    setModel(m_model);
    m_model->loadPath("C:\\");
}

void FileGridView::performSearch(const QString& query) {
    if (m_model) {
        m_model->performSearch(query);
    }
}
