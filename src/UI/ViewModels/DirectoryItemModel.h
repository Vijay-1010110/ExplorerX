#pragma once
#include <QAbstractItemModel>
#include <vector>
#include <memory>
#include <QString>

struct DummyNode {
    QString name;
    DummyNode* parent = nullptr;
    std::vector<std::unique_ptr<DummyNode>> children;

    DummyNode(const QString& n, DummyNode* p = nullptr) : name(n), parent(p) {}
    ~DummyNode() = default;
};

class DirectoryItemModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit DirectoryItemModel(QObject *parent = nullptr);
    ~DirectoryItemModel() override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::unique_ptr<DummyNode> m_rootNode;
};
