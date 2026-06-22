#include "FileItemModel.h"
#include <thread>
#include <spdlog/spdlog.h>
#include <QApplication>
#include <QStyle>
#include <filesystem>
#include <QMimeData>
#include <QUrl>
#include <QImage>
#include <QPainter>

FileItemModel::FileItemModel(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                             std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                             std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                             QObject *parent)
    : QAbstractTableModel(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)), m_thumbOrchestrator(std::move(thumbOrchestrator)) {
}

FileItemModel::~FileItemModel() = default;

void FileItemModel::loadPath(const std::string& path) {
    if (!m_provider) return;
    m_currentLoadedPath = path;

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

void FileItemModel::setSearchResults(std::vector<ExplorerX::Domain::FileItem> results) {
    beginResetModel();
    m_files = std::move(results);
    endResetModel();
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
                auto future = orchestrator->GetThumbnailAsync(path, 256);
                auto result = future.get();
                if (result) {
                    auto thumbnail = result.value();
                    if (!thumbnail.Data.empty()) {
                        QImage img;
                        img.loadFromData(thumbnail.Data.data(), thumbnail.Data.size());
                        if (!img.isNull()) {
                            // Scale up to 256x256, preserving aspect ratio and smoothing pixels
                            QImage scaledImg = img.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                            
                            // Draw onto a perfect 256x256 square bounding box
                            QImage squareImg(256, 256, QImage::Format_ARGB32);
                            squareImg.fill(Qt::transparent);
                            QPainter painter(&squareImg);
                            painter.drawImage((256 - scaledImg.width()) / 2, (256 - scaledImg.height()) / 2, scaledImg);
                            painter.end();
                            
                            auto* self = const_cast<FileItemModel*>(this);
                            QMetaObject::invokeMethod(self, [self, pIndex, pathStr, squareImg]() {
                                self->m_iconCache[pathStr] = QIcon(QPixmap::fromImage(squareImg));
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
        return QApplication::style()->standardIcon(file.IsDirectory ? QStyle::SP_DirIcon : QStyle::SP_FileIcon);
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
    Qt::ItemFlags defaultFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
    if (isDirectory(index)) {
        defaultFlags |= Qt::ItemIsDropEnabled;
    }
    return defaultFlags;
}

QStringList FileItemModel::mimeTypes() const {
    return {"text/uri-list"};
}

QMimeData* FileItemModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urls;
    for (const QModelIndex &index : indexes) {
        if (index.isValid() && index.column() == 0) {
            urls.append(QUrl::fromLocalFile(filePath(index)));
        }
    }
    mimeData->setUrls(urls);
    return mimeData;
}

Qt::DropActions FileItemModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

bool FileItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (!data->hasUrls()) return false;

    std::string targetPath;
    if (parent.isValid() && isDirectory(parent)) {
        targetPath = filePath(parent).toStdString();
    } else {
        targetPath = m_currentLoadedPath;
    }

    if (targetPath.empty()) return false;

    QList<QUrl> urls = data->urls();
    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider = m_provider;
    std::thread([provider, urls, targetPath, action]() {
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                std::string srcPath = url.toLocalFile().toStdString();
                std::filesystem::path srcFsPath(srcPath);
                std::filesystem::path destFsPath(targetPath);
                destFsPath /= srcFsPath.filename();
                
                if (action == Qt::CopyAction) {
                    provider->Copy(ExplorerX::Domain::CopyRequest{ExplorerX::Domain::Path(srcPath), ExplorerX::Domain::Path(destFsPath.string()), false});
                } else if (action == Qt::MoveAction) {
                    provider->Move(ExplorerX::Domain::MoveRequest{ExplorerX::Domain::Path(srcPath), ExplorerX::Domain::Path(destFsPath.string())});
                }
            }
        }
    }).detach();

    return true;
}
