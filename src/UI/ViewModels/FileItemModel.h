#pragma once
#include <QAbstractTableModel>
#include <vector>
#include <QString>

struct DummyFile {
    QString name;
    QString size;
    QString type;
};

class FileItemModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit FileItemModel(QObject *parent = nullptr);
    ~FileItemModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    std::vector<DummyFile> m_files;
};
