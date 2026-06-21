#pragma once
#include <QMainWindow>
#include <memory>
#include "../Domain/IFileSystemProvider.h"
#include "../Domain/ISearchEngine.h"

class QLineEdit;
class DirectoryTreeView;
class FileGridView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                        std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                        QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCopy();
    void onPaste();
    void onCut();
    void onDelete();
    void onSearchTriggered();
    void onDirectorySelected(const QModelIndex& index);

private:
    void setupUi();
    void setupActions();

    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::shared_ptr<ExplorerX::Domain::ISearchEngine> m_searchEngine;
    
    QLineEdit* m_searchBox = nullptr;
    DirectoryTreeView* m_navTree = nullptr;
    FileGridView* m_fileGrid = nullptr;
};
