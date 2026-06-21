#pragma once
#include <QMainWindow>
#include <memory>
#include "../Domain/IFileSystemProvider.h"
#include "../Domain/ISearchEngine.h"

#include "../Core/ThumbnailOrchestrator.h"
#include "../Core/AIIntentOrchestrator.h"

class QLineEdit;
class QLabel;
class DirectoryTreeView;
class FileGridView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                        std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                        std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                        std::shared_ptr<ExplorerX::Core::AIIntentOrchestrator> aiOrchestrator,
                        QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCopy();
    void onPaste();
    void onCut();
    void onDelete();
    void onSearchTriggered();
    void onDirectorySelected(const QModelIndex& index);
    void onAICommandTriggered();
    void onFileGridDoubleClicked(const QModelIndex& index);
    void onTreeRowsInserted(const QModelIndex& parent, int first, int last);

private:
    void setupUi();
    void setupActions();

    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::shared_ptr<ExplorerX::Domain::ISearchEngine> m_searchEngine;
    std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> m_thumbOrchestrator;
    std::shared_ptr<ExplorerX::Core::AIIntentOrchestrator> m_aiOrchestrator;
    
    QString m_pendingSyncPath;
    
    QLineEdit* m_searchBox = nullptr;
    QLineEdit* m_aiCommandBox = nullptr;
    QLabel* m_aiStatusLabel = nullptr;
    DirectoryTreeView* m_navTree = nullptr;
    FileGridView* m_fileGrid = nullptr;
};
