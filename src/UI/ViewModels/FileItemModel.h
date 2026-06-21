#pragma once
#include <QAbstractTableModel>
#include <vector>
#include <memory>
#include <QString>
#include <QIcon>
#include <unordered_map>
#include <unordered_set>
#include "../Domain/FileItem.h"
#include "../Domain/IFileSystemProvider.h"
#include "../Domain/ISearchEngine.h"
#include "../Core/ThumbnailOrchestrator.h"

class FileItemModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit FileItemModel(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                           std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                           std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                           QObject *parent = nullptr);
    ~FileItemModel() override;

    void loadPath(const std::string& path);
    void performSearch(const QString& query);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::shared_ptr<ExplorerX::Domain::ISearchEngine> m_searchEngine;
    std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> m_thumbOrchestrator;
    std::vector<ExplorerX::Domain::FileItem> m_files;
    
    mutable std::unordered_map<std::string, QIcon> m_iconCache;
    mutable std::unordered_set<std::string> m_pendingThumbnails;
};
