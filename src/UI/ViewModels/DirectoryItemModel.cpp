#include "DirectoryItemModel.h"
#include <thread>
#include <spdlog/spdlog.h>
#include <filesystem>

DirectoryItemModel::DirectoryItemModel(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider, QObject *parent) 
    : QAbstractItemModel(parent), m_provider(std::move(provider)) {
    m_rootNode = std::make_unique<RealDirNode>(QStringLiteral("Root"), QStringLiteral(""));
}

DirectoryItemModel::~DirectoryItemModel() = default;

void DirectoryItemModel::loadPath(const std::string& path) {
    if (!m_provider) return;
    
    beginResetModel();
    m_rootNode = std::make_unique<RealDirNode>(QStringLiteral("Root"), QString::fromStdString(path));
    m_rootNode->hasFetchedChildren = false;
    m_rootNode->isFetching = false;
    endResetModel();
    
    fetchMore(QModelIndex());
}

bool DirectoryItemModel::hasChildren(const QModelIndex &parent) const {
    if (parent.column() > 0) return false;
    RealDirNode *parentNode = parent.isValid() ? static_cast<RealDirNode*>(parent.internalPointer()) : m_rootNode.get();
    if (!parentNode->hasFetchedChildren) return true; // Assume true until fetched
    return !parentNode->children.empty();
}

bool DirectoryItemModel::canFetchMore(const QModelIndex &parent) const {
    RealDirNode *parentNode = parent.isValid() ? static_cast<RealDirNode*>(parent.internalPointer()) : m_rootNode.get();
    return !parentNode->hasFetchedChildren && !parentNode->isFetching;
}

void DirectoryItemModel::fetchMore(const QModelIndex &parent) {
    RealDirNode *parentNode = parent.isValid() ? static_cast<RealDirNode*>(parent.internalPointer()) : m_rootNode.get();
    if (parentNode->hasFetchedChildren || parentNode->isFetching) return;
    
    parentNode->isFetching = true;
    std::string path = parentNode->fullPath.toStdString();
    
    QPersistentModelIndex persistentParent(parent);
    
    std::thread([this, persistentParent, provider = m_provider, path]() {
        auto future = provider->Enumerate(ExplorerX::Domain::Path(path));
        auto result = future.get();
        if (result) {
            auto items = result.value().Items;
            QMetaObject::invokeMethod(this, [this, persistentParent, items = std::move(items)]() mutable {
                RealDirNode *node = nullptr;
                if (persistentParent.isValid()) {
                    node = static_cast<RealDirNode*>(persistentParent.internalPointer());
                } else {
                    node = m_rootNode.get();
                }
                
                std::vector<ExplorerX::Domain::FileItem> dirs;
                for (const auto& item : items) {
                    if (item.IsDirectory) dirs.push_back(item);
                }
                
                if (!dirs.empty()) {
                    beginInsertRows(persistentParent, 0, dirs.size() - 1);
                    for (const auto& dir : dirs) {
                        node->children.push_back(
                            std::make_unique<RealDirNode>(
                                QString::fromStdString(dir.Name), 
                                QString::fromStdString(dir.ItemPath.ToString()), 
                                node
                            )
                        );
                    }
                    endInsertRows();
                }
                node->hasFetchedChildren = true;
                node->isFetching = false;
            });
        } else {
            spdlog::error("DirectoryItemModel failed to fetch path: {}", path);
            QMetaObject::invokeMethod(this, [this, persistentParent]() {
                RealDirNode *node = persistentParent.isValid() ? static_cast<RealDirNode*>(persistentParent.internalPointer()) : m_rootNode.get();
                node->hasFetchedChildren = true;
                node->isFetching = false;
            });
        }
    }).detach();
}

QString DirectoryItemModel::filePath(const QModelIndex& index) const {
    if (!index.isValid()) return {};
    auto *node = static_cast<RealDirNode*>(index.internalPointer());
    return node->fullPath;
}

QModelIndex DirectoryItemModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    RealDirNode *parentNode;
    if (!parent.isValid()) {
        parentNode = m_rootNode.get();
    } else {
        parentNode = static_cast<RealDirNode*>(parent.internalPointer());
    }

    if (row < parentNode->children.size()) {
        return createIndex(row, column, parentNode->children[row].get());
    }
    return {};
}

QModelIndex DirectoryItemModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return {};
    }

    auto *childNode = static_cast<RealDirNode*>(index.internalPointer());
    RealDirNode *parentNode = childNode->parent;

    if (parentNode == m_rootNode.get() || parentNode == nullptr) {
        return {};
    }

    RealDirNode *grandParentNode = parentNode->parent;
    if (!grandParentNode) return {};

    int row = 0;
    for (size_t i = 0; i < grandParentNode->children.size(); ++i) {
        if (grandParentNode->children[i].get() == parentNode) {
            row = static_cast<int>(i);
            break;
        }
    }

    return createIndex(row, 0, parentNode);
}

int DirectoryItemModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) {
        return 0;
    }

    RealDirNode *parentNode;
    if (!parent.isValid()) {
        parentNode = m_rootNode.get();
    } else {
        parentNode = static_cast<RealDirNode*>(parent.internalPointer());
    }

    return static_cast<int>(parentNode->children.size());
}

int DirectoryItemModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant DirectoryItemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }

    auto *node = static_cast<RealDirNode*>(index.internalPointer());
    return node->name;
}

QVariant DirectoryItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0) {
        return QStringLiteral("Directories");
    }
    return {};
}

Qt::ItemFlags DirectoryItemModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsDropEnabled;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QStringList DirectoryItemModel::mimeTypes() const {
    return {"text/uri-list"};
}

QMimeData* DirectoryItemModel::mimeData(const QModelIndexList &indexes) const {
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

Qt::DropActions DirectoryItemModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions DirectoryItemModel::supportedDragActions() const {
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

bool DirectoryItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (!data->hasUrls()) return false;

    std::string targetPath;
    if (parent.isValid()) {
        targetPath = filePath(parent).toStdString();
    } else {
        if (!m_rootNode) return false;
        targetPath = m_rootNode->fullPath.toStdString();
    }

    if (targetPath.empty()) return false;

    QList<QUrl> urls = data->urls();
    std::thread([urls, targetPath, action]() {
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                std::string srcPath = url.toLocalFile().toStdString();
                std::filesystem::path srcFsPath(srcPath);
                std::filesystem::path destFsPath(targetPath);
                destFsPath /= srcFsPath.filename();
                
                std::error_code ec;
                if (action == Qt::MoveAction) {
                    std::filesystem::rename(srcFsPath, destFsPath, ec);
                    if (ec) spdlog::error("Move failed: {}", ec.message());
                } else {
                    std::filesystem::copy(srcFsPath, destFsPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);
                    if (ec) spdlog::error("Copy failed: {}", ec.message());
                }
            }
        }
    }).detach();

    return true;
}
