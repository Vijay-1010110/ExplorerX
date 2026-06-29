#pragma once
#include <QAbstractTableModel>
#include <vector>
#include <memory>
#include <QString>
#include <QIcon>
#include <QMimeData>
#include <QUrl>
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
    void setSearchResults(std::vector<ExplorerX::Domain::FileItem> results);

    bool isDirectory(const QModelIndex& index) const;
    QString filePath(const QModelIndex& index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList &indexes) const override;
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::shared_ptr<ExplorerX::Domain::ISearchEngine> m_searchEngine;
    std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> m_thumbOrchestrator;
    std::vector<ExplorerX::Domain::FileItem> m_files;
    std::string m_currentLoadedPath;
    
    mutable std::unordered_map<std::string, QIcon> m_iconCache;
    mutable std::unordered_set<std::string> m_pendingThumbnails;
};
