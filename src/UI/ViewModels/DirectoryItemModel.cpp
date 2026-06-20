#include "DirectoryItemModel.h"
#include <thread>
#include <spdlog/spdlog.h>

DirectoryItemModel::DirectoryItemModel(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider, QObject *parent) 
    : QAbstractItemModel(parent), m_provider(std::move(provider)) {
    m_rootNode = std::make_unique<RealDirNode>(QStringLiteral("Root"), QStringLiteral(""));
}

DirectoryItemModel::~DirectoryItemModel() = default;

void DirectoryItemModel::loadPath(const std::string& path) {
    if (!m_provider) return;
    
    std::thread([this, provider = m_provider, path]() {
        auto future = provider->Enumerate(ExplorerX::Domain::Path(path));
        auto result = future.get();
        if (result) {
            auto items = result.value().Items;
            QMetaObject::invokeMethod(this, [this, path, items = std::move(items)]() mutable {
                beginResetModel();
                m_rootNode->children.clear();
                for (const auto& item : items) {
                    if (item.IsDirectory) {
                        m_rootNode->children.push_back(
                            std::make_unique<RealDirNode>(
                                QString::fromStdString(item.Name), 
                                QString::fromStdString(item.ItemPath.ToString()), 
                                m_rootNode.get()
                            )
                        );
                    }
                }
                endResetModel();
            });
        } else {
            spdlog::error("DirectoryItemModel failed to load path: {}", path);
        }
    }).detach();
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
