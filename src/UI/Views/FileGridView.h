#pragma once
#include <QListView>

#include <memory>
#include "../Domain/IFileSystemProvider.h"
#include "../Domain/ISearchEngine.h"
#include "../Core/ThumbnailOrchestrator.h"

#include <QSortFilterProxyModel>

class FileItemModel;

class FileGridView : public QListView {
    Q_OBJECT
public:
    explicit FileGridView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                          std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                          std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                          QWidget *parent = nullptr);
    ~FileGridView() override;

    void loadPath(const QString& path);
    void setSearchResults(std::vector<ExplorerX::Domain::FileItem> results);
    void setLocalFilter(const QString& filterText);

private:
    void setupRealModel();
    
    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::shared_ptr<ExplorerX::Domain::ISearchEngine> m_searchEngine;
    std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> m_thumbOrchestrator;
    FileItemModel* m_model = nullptr;
    QSortFilterProxyModel* m_proxyModel = nullptr;
};
