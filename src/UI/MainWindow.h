#pragma once
#include <QMainWindow>
#include <QList>
#include <memory>
#include "../Domain/IFileSystemProvider.h"
#include "../Domain/ISearchEngine.h"
#include <QInputDialog>

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
    
    // Navigation slots
    void onBackClicked();
    void onForwardClicked();
    void onUpClicked();
    void onRefreshClicked();
    void onAddressBarReturnPressed();
    
    // Command Bar slots
    void onRename();
    void onShare();
    void onNewFolder();
    void onNewFile();
    void onSortChanged(int index);
    void onViewModeChanged(int index);

private:
    void setupUi();
    void setupActions();
    void navigateTo(const QString& path, bool recordHistory);

    std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> m_provider;
    std::shared_ptr<ExplorerX::Domain::ISearchEngine> m_searchEngine;
    std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> m_thumbOrchestrator;
    std::shared_ptr<ExplorerX::Core::AIIntentOrchestrator> m_aiOrchestrator;
    
    QString m_pendingSyncPath;
    QList<QString> m_history;
    int m_historyIndex = -1;
    QString m_currentPath;
    
    QString m_clipboardPath;
    bool m_clipboardIsCut = false;
    
    QLineEdit* m_searchBox = nullptr;
    QLineEdit* m_aiCommandBox = nullptr;
    QLabel* m_aiStatusLabel = nullptr;
    DirectoryTreeView* m_navTree = nullptr;
    FileGridView* m_fileGrid = nullptr;
    
    // Navigation bar elements
    class QToolButton* m_btnBack = nullptr;
    class QToolButton* m_btnForward = nullptr;
    class QToolButton* m_btnUp = nullptr;
    class QToolButton* m_btnRefresh = nullptr;
    
    // Breadcrumb Address Bar elements
    class QStackedWidget* m_addressStack = nullptr;
    QWidget* m_breadcrumbsContainer = nullptr;
    class QHBoxLayout* m_breadcrumbsLayout = nullptr;
    QLineEdit* m_addressEdit = nullptr;
    class QToolButton* m_btnEditAddress = nullptr;
    
    void updateBreadcrumbs();
    void setAddressEditMode(bool editMode);
};
