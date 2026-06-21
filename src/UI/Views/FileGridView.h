#pragma once
#include <QTableView>

#include <memory>
#include "../Domain/IFileSystemProvider.h"
#include "../Domain/ISearchEngine.h"

class FileItemModel;

class FileGridView : public QTableView {
    Q_OBJECT
public:
    explicit FileGridView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                          std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                          QWidget *parent = nullptr);
    ~FileGridView() override;

    void loadPath(const QString& path);
    void performSearch(const QString& query);

private:
    void setupRealModel();
    
    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::shared_ptr<ExplorerX::Domain::ISearchEngine> m_searchEngine;
    FileItemModel* m_model = nullptr;
};
