#include "FileGridView.h"
#include "../ViewModels/FileItemModel.h"
#include <QHeaderView>
#include <QWheelEvent>
#include <QKeyEvent>

FileGridView::FileGridView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                           std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                           std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                           QWidget *parent)
    : QListView(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)), m_thumbOrchestrator(std::move(thumbOrchestrator)) {
    
    // Icon Grid Mode Configuration
    setViewMode(QListView::IconMode);
    setIconSize(QSize(m_zoomLevel, m_zoomLevel));
    setResizeMode(QListView::Adjust);
    setSpacing(10);
    setGridSize(QSize(m_zoomLevel + 24, m_zoomLevel + 44)); // Give enough space for icon + text
    setWordWrap(true);
    setUniformItemSizes(true);
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent, false);
    viewport()->setAutoFillBackground(false);
    
    // Selection configuration
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionRectVisible(true);
    
    // Drag and Drop support
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos) { emit contextMenuRequested(pos, indexAt(pos)); });
    
    setupRealModel();
}

FileGridView::~FileGridView() = default;

void FileGridView::setupRealModel() {
    m_model = new FileItemModel(m_provider, m_searchEngine, m_thumbOrchestrator, this);
    
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(0);
    
    setModel(m_proxyModel);
    
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

void FileGridView::setLocalFilter(const QString& filterText) {
    if (m_proxyModel) {
        m_proxyModel->setFilterWildcard("*" + filterText + "*");
    }
}

void FileGridView::setZoom(int size) {
    if (size < 32) size = 32;
    if (size > 256) size = 256;
    m_zoomLevel = size;
    setIconSize(QSize(m_zoomLevel, m_zoomLevel));
    setGridSize(QSize(m_zoomLevel + 24, m_zoomLevel + 44));
}

void FileGridView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            setZoom(m_zoomLevel + 16);
        } else if (event->angleDelta().y() < 0) {
            setZoom(m_zoomLevel - 16);
        }
        event->accept();
    } else {
        QListView::wheelEvent(event);
    }
}

void FileGridView::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
            setZoom(m_zoomLevel + 16);
        } else if (event->key() == Qt::Key_Minus) {
            setZoom(m_zoomLevel - 16);
        } else {
            QListView::keyPressEvent(event);
        }
    } else {
        QListView::keyPressEvent(event);
    }
}

void FileGridView::scrollContentsBy(int dx, int dy) {
    // Block Qt's internal scroll-blitting algorithm completely.
    // By disabling updates, QWidget::scroll() returns immediately without blitting.
    viewport()->setUpdatesEnabled(false);
    
    QListView::scrollContentsBy(dx, dy);
    
    // Re-enable updates and force a clean, full-viewport repaint (FullViewportUpdate equivalent).
    viewport()->setUpdatesEnabled(true);
    viewport()->update();
}
