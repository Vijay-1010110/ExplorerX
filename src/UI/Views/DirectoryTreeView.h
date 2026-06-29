#pragma once
#include <QTreeView>

#include <memory>
#include "../Domain/IFileSystemProvider.h"

class DirectoryItemModel;

class DirectoryTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit DirectoryTreeView(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider, QWidget *parent = nullptr);
    ~DirectoryTreeView() override;

private:
    void setupRealModel();
    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    DirectoryItemModel* m_model = nullptr;

protected:
    void scrollContentsBy(int dx, int dy) override;
};
