#pragma once
#include <QAbstractItemModel>
#include <vector>
#include <memory>
#include <QString>
#include "../Domain/IFileSystemProvider.h"

struct RealDirNode {
    QString name;
    QString fullPath;
    RealDirNode* parent = nullptr;
    std::vector<std::unique_ptr<RealDirNode>> children;

    RealDirNode(const QString& n, const QString& p, RealDirNode* pParent = nullptr) 
        : name(n), fullPath(p), parent(pParent) {}
    ~RealDirNode() = default;
};

class DirectoryItemModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit DirectoryItemModel(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider, QObject *parent = nullptr);
    ~DirectoryItemModel() override;

    void loadPath(const std::string& path);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::unique_ptr<RealDirNode> m_rootNode;
};
