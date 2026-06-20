#include "DirectoryItemModel.h"

DirectoryItemModel::DirectoryItemModel(QObject *parent) : QAbstractItemModel(parent) {
    m_rootNode = std::make_unique<DummyNode>(QStringLiteral("Root"));
    
    auto cDrive = std::make_unique<DummyNode>(QStringLiteral("C: Drive"), m_rootNode.get());
    cDrive->children.push_back(std::make_unique<DummyNode>(QStringLiteral("Windows"), cDrive.get()));
    cDrive->children.push_back(std::make_unique<DummyNode>(QStringLiteral("Users"), cDrive.get()));
    
    auto dDrive = std::make_unique<DummyNode>(QStringLiteral("D: Drive"), m_rootNode.get());
    dDrive->children.push_back(std::make_unique<DummyNode>(QStringLiteral("Documents"), dDrive.get()));
    
    m_rootNode->children.push_back(std::move(cDrive));
    m_rootNode->children.push_back(std::move(dDrive));
}

DirectoryItemModel::~DirectoryItemModel() = default;

QModelIndex DirectoryItemModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    DummyNode *parentNode;
    if (!parent.isValid()) {
        parentNode = m_rootNode.get();
    } else {
        parentNode = static_cast<DummyNode*>(parent.internalPointer());
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

    auto *childNode = static_cast<DummyNode*>(index.internalPointer());
    DummyNode *parentNode = childNode->parent;

    if (parentNode == m_rootNode.get() || parentNode == nullptr) {
        return {};
    }

    DummyNode *grandParentNode = parentNode->parent;
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

    DummyNode *parentNode;
    if (!parent.isValid()) {
        parentNode = m_rootNode.get();
    } else {
        parentNode = static_cast<DummyNode*>(parent.internalPointer());
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

    auto *node = static_cast<DummyNode*>(index.internalPointer());
    return node->name;
}

QVariant DirectoryItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0) {
        return QStringLiteral("Directories");
    }
    return {};
}
