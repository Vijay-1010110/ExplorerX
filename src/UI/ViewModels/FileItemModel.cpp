#include "FileItemModel.h"
#include <thread>
#include <spdlog/spdlog.h>

FileItemModel::FileItemModel(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                             std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                             std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                             QObject *parent)
    : QAbstractTableModel(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)), m_thumbOrchestrator(std::move(thumbOrchestrator)) {
}

FileItemModel::~FileItemModel() = default;

void FileItemModel::loadPath(const std::string& path) {
    if (!m_provider) return;

    std::thread([this, provider = m_provider, path]() {
        auto future = provider->Enumerate(ExplorerX::Domain::Path(path));
        auto result = future.get();
        if (result) {
            auto items = result.value().Items;
            QMetaObject::invokeMethod(this, [this, items = std::move(items)]() mutable {
                beginResetModel();
                m_files = std::move(items);
                endResetModel();
            });
        } else {
            spdlog::error("Failed to load path: {}", path);
        }
    }).detach();
}

bool FileItemModel::isDirectory(const QModelIndex& index) const {
    if (!index.isValid() || index.row() >= m_files.size()) return false;
    return m_files[index.row()].IsDirectory;
}

QString FileItemModel::filePath(const QModelIndex& index) const {
    if (!index.isValid() || index.row() >= m_files.size()) return {};
    return QString::fromStdString(m_files[index.row()].ItemPath.ToString());
}

void FileItemModel::performSearch(const QString& query) {
    if (!m_searchEngine) return;
    std::string q = query.toStdString();
    
    std::thread([this, engine = m_searchEngine, q]() {
        ExplorerX::Domain::SearchQuery searchQ{q, ExplorerX::Domain::Path("C:\\")};
        auto future = engine->Query(searchQ);
        auto result = future.get();
        if (result) {
            auto items = result.value().Matches;
            QMetaObject::invokeMethod(this, [this, items = std::move(items)]() mutable {
                beginResetModel();
                m_files = std::move(items);
                endResetModel();
            });
        } else {
            spdlog::error("Failed to perform search: {}", q);
        }
    }).detach();
}

int FileItemModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_files.size());
}

int FileItemModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return 3;
}

QVariant FileItemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    const auto& file = m_files[index.row()];
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return QString::fromStdString(file.Name);
            case 1: return QString::number(file.Size) + " bytes"; // Formatting can be improved later
            case 2: return file.IsDirectory ? QStringLiteral("Folder") : QStringLiteral("File");
            default: return {};
        }
    } else if (role == Qt::DecorationRole && index.column() == 0) {
        std::string pathStr = file.ItemPath.ToString();
        
        if (m_iconCache.find(pathStr) != m_iconCache.end()) {
            return m_iconCache[pathStr];
        }
        
        if (m_thumbOrchestrator && m_pendingThumbnails.find(pathStr) == m_pendingThumbnails.end()) {
            m_pendingThumbnails.insert(pathStr);
            QPersistentModelIndex pIndex(index);
            
            std::thread([this, pIndex, orchestrator = m_thumbOrchestrator, path = file.ItemPath, pathStr]() {
                auto future = orchestrator->GetThumbnailAsync(path, 32);
                auto result = future.get();
                if (result) {
                    auto thumbnail = result.value();
                    if (!thumbnail.Data.empty()) {
                        QImage img;
                        img.loadFromData(thumbnail.Data.data(), thumbnail.Data.size());
                        if (!img.isNull()) {
                            auto* self = const_cast<FileItemModel*>(this);
                            QMetaObject::invokeMethod(self, [self, pIndex, pathStr, img]() {
                                self->m_iconCache[pathStr] = QIcon(QPixmap::fromImage(img));
                                self->m_pendingThumbnails.erase(pathStr);
                                if (pIndex.isValid()) {
                                    emit self->dataChanged(pIndex, pIndex, {Qt::DecorationRole});
                                }
                            });
                            return;
                        }
                    }
                }
                
                // Fallback or error
                auto* self = const_cast<FileItemModel*>(this);
                QMetaObject::invokeMethod(self, [self, pathStr]() {
                    self->m_pendingThumbnails.erase(pathStr);
                });
            }).detach();
        }
        
        // Return a generic fallback while loading or if it failed
        return file.IsDirectory ? QIcon::fromTheme("folder") : QIcon::fromTheme("text-x-generic");
    }

    return {};
}

QVariant FileItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return {};
    }

    switch (section) {
        case 0: return QStringLiteral("Name");
        case 1: return QStringLiteral("Size");
        case 2: return QStringLiteral("Type");
        default: return {};
    }
}

Qt::ItemFlags FileItemModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsDropEnabled;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}
