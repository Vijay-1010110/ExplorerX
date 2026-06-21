#include "MainWindow.h"
#include <QToolBar>
#include <QSplitter>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include "Views/DirectoryTreeView.h"
#include "Views/FileGridView.h"
#include "ViewModels/DirectoryItemModel.h"
#include "ViewModels/FileItemModel.h"
#include <QKeySequence>
#include <spdlog/spdlog.h>
#include <thread>
#include <QToolButton>
#include <QDir>
#include <QFileInfo>
#include <QStackedWidget>
#include <QFrame>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                       std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                       std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                       std::shared_ptr<ExplorerX::Core::AIIntentOrchestrator> aiOrchestrator,
                       QWidget *parent)
    : QMainWindow(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)), m_thumbOrchestrator(std::move(thumbOrchestrator)), m_aiOrchestrator(std::move(aiOrchestrator)) {
    setupUi();
    setupActions();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle(QStringLiteral("ExplorerX"));
    resize(1024, 768);

    // Main widget and layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Top bar containing AI command bar and search box
    QWidget* topBar = new QWidget(centralWidget);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(5, 5, 5, 5);
    
    // Navigation Buttons
    m_btnBack = new QToolButton(topBar);
    m_btnBack->setText("<-");
    m_btnBack->setEnabled(false);
    
    m_btnForward = new QToolButton(topBar);
    m_btnForward->setText("->");
    m_btnForward->setEnabled(false);
    
    m_btnUp = new QToolButton(topBar);
    m_btnUp->setText("^");
    m_btnUp->setEnabled(false);
    
    m_btnRefresh = new QToolButton(topBar);
    m_btnRefresh->setText("R");
    
    topLayout->addWidget(m_btnBack);
    topLayout->addWidget(m_btnForward);
    topLayout->addWidget(m_btnUp);
    topLayout->addWidget(m_btnRefresh);
    
    // Address Bar Stack
    m_addressStack = new QStackedWidget(topBar);
    m_addressStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // 1. Breadcrumbs Container
    QFrame* breadcrumbsFrame = new QFrame(m_addressStack);
    breadcrumbsFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border: 1px solid #CCCCCC; border-radius: 4px; }");
    m_breadcrumbsLayout = new QHBoxLayout(breadcrumbsFrame);
    m_breadcrumbsLayout->setContentsMargins(2, 2, 2, 2);
    m_breadcrumbsLayout->setSpacing(0);
    
    m_breadcrumbsContainer = new QWidget(breadcrumbsFrame);
    QHBoxLayout* innerCrumbLayout = new QHBoxLayout(m_breadcrumbsContainer);
    innerCrumbLayout->setContentsMargins(0, 0, 0, 0);
    innerCrumbLayout->setSpacing(2);
    m_breadcrumbsLayout->addWidget(m_breadcrumbsContainer);
    
    m_breadcrumbsLayout->addStretch(1); // Push breadcrumbs to the left
    
    m_btnEditAddress = new QToolButton(breadcrumbsFrame);
    m_btnEditAddress->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton)); // Or just text
    m_btnEditAddress->setText("Edit");
    m_btnEditAddress->setCursor(Qt::PointingHandCursor);
    m_btnEditAddress->setStyleSheet("QToolButton { border: none; padding: 2px; } QToolButton:hover { background-color: #E0E0E0; }");
    m_breadcrumbsLayout->addWidget(m_btnEditAddress);
    
    m_addressStack->addWidget(breadcrumbsFrame);
    
    // 2. Line Edit
    m_addressEdit = new QLineEdit(m_addressStack);
    m_addressEdit->setStyleSheet("QLineEdit { padding: 4px; border: 1px solid #0078D7; border-radius: 4px; }");
    m_addressStack->addWidget(m_addressEdit);
    
    topLayout->addWidget(m_addressStack, 1);
    
    // AI Command Box (Distinct styling)
    m_aiCommandBox = new QLineEdit(topBar);
    m_aiCommandBox->setPlaceholderText("Ask the AI to do something...");
    m_aiCommandBox->setStyleSheet("QLineEdit { border: 2px solid #0078D7; border-radius: 4px; padding: 4px; font-weight: bold; }");
    topLayout->addWidget(m_aiCommandBox, 1); // stretch factor 1
    
    // Search Box
    m_searchBox = new QLineEdit(topBar);
    m_searchBox->setPlaceholderText("Search...");
    m_searchBox->setMaximumWidth(200);
    topLayout->addWidget(m_searchBox, 0);

    mainLayout->addWidget(topBar);

    // Splitter for navigation and file grid
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);

    // Left pane (navigation tree)
    m_navTree = new DirectoryTreeView(m_provider, mainSplitter);
    mainSplitter->addWidget(m_navTree);

    // Right pane (file grid)
    m_fileGrid = new FileGridView(m_provider, m_searchEngine, m_thumbOrchestrator, mainSplitter);
    mainSplitter->addWidget(m_fileGrid);

    // Wire navigation tree clicks to the file grid
    connect(m_navTree, &DirectoryTreeView::clicked, this, &MainWindow::onDirectorySelected);
    connect(m_searchBox, &QLineEdit::returnPressed, this, &MainWindow::onSearchTriggered);
    connect(m_aiCommandBox, &QLineEdit::returnPressed, this, &MainWindow::onAICommandTriggered);
    connect(m_fileGrid, &FileGridView::doubleClicked, this, &MainWindow::onFileGridDoubleClicked);
    connect(m_navTree->model(), &QAbstractItemModel::rowsInserted, this, &MainWindow::onTreeRowsInserted);
    
    // Wire navigation bar
    connect(m_btnBack, &QToolButton::clicked, this, &MainWindow::onBackClicked);
    connect(m_btnForward, &QToolButton::clicked, this, &MainWindow::onForwardClicked);
    connect(m_btnUp, &QToolButton::clicked, this, &MainWindow::onUpClicked);
    connect(m_btnRefresh, &QToolButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(m_addressEdit, &QLineEdit::returnPressed, this, &MainWindow::onAddressBarReturnPressed);
    connect(m_btnEditAddress, &QToolButton::clicked, [this]() { setAddressEditMode(true); });

    // Initial sizes for splitter (e.g., 25% vs 75%)
    mainSplitter->setSizes({250, 750});

    mainLayout->addWidget(mainSplitter, 1);
    
    // Status Bar label for AI Feedback
    m_aiStatusLabel = new QLabel(this);
    statusBar()->addWidget(m_aiStatusLabel);
    
    setCentralWidget(centralWidget);
}

void MainWindow::onDirectorySelected(const QModelIndex& index) {
    if (!index.isValid()) return;
    auto* model = qobject_cast<DirectoryItemModel*>(m_navTree->model());
    if (model) {
        QString path = model->filePath(index);
        if (path != m_currentPath) {
            navigateTo(path, true);
        }
    }
}

void MainWindow::onSearchTriggered() {
    QString query = m_searchBox->text();
    if (query.isEmpty()) return;
    
    spdlog::info("Search triggered for: {}", query.toStdString());
    m_aiStatusLabel->setText("Searching...");
    m_searchBox->setEnabled(false);
    
    if (m_searchEngine && m_fileGrid) {
        QString searchPath = "C:\\";
        if (m_navTree && m_navTree->selectionModel()) {
            QModelIndex currentIndex = m_navTree->selectionModel()->currentIndex();
            if (currentIndex.isValid()) {
                auto* dirModel = qobject_cast<DirectoryItemModel*>(m_navTree->model());
                if (dirModel) {
                    searchPath = dirModel->filePath(currentIndex);
                }
            }
        }
        
        std::thread([this, queryStr = query.toStdString(), pathStr = searchPath.toStdString()]() {
            ExplorerX::Domain::SearchQuery searchQ{queryStr, ExplorerX::Domain::Path(pathStr)};
            auto future = m_searchEngine->Query(searchQ);
            auto result = future.get();
            
            QMetaObject::invokeMethod(this, [this, result]() mutable {
                m_searchBox->setEnabled(true);
                if (result) {
                    m_fileGrid->setSearchResults(std::move(result.value().Matches));
                    m_aiStatusLabel->setText("Search complete.");
                } else {
                    m_aiStatusLabel->setText("Search failed.");
                }
            });
        }).detach();
    }
}

void MainWindow::onAICommandTriggered() {
    QString command = m_aiCommandBox->text();
    if (command.isEmpty()) return;
    
    spdlog::info("AI Command triggered: {}", command.toStdString());
    
    // Show feedback
    m_aiStatusLabel->setText("AI is processing...");
    m_aiCommandBox->setEnabled(false);
    
    if (m_aiOrchestrator) {
        std::thread([this, commandStr = command.toStdString()]() {
            auto future = m_aiOrchestrator->ExecuteNaturalLanguageCommand(commandStr);
            auto result = future.get();
            
            QMetaObject::invokeMethod(this, [this, result]() {
                m_aiCommandBox->setEnabled(true);
                m_aiCommandBox->clear();
                
                if (result.has_value()) {
                    m_aiStatusLabel->setText("AI processing complete.");
                } else {
                    m_aiStatusLabel->setText(QString("AI Error: %1").arg(QString::fromStdString(result.error().Message)));
                }
            });
        }).detach();
    }
}

void MainWindow::onFileGridDoubleClicked(const QModelIndex& index) {
    if (!index.isValid()) return;
    
    auto* fileModel = qobject_cast<FileItemModel*>(m_fileGrid->model());
    if (fileModel && fileModel->isDirectory(index)) {
        QString path = fileModel->filePath(index);
        navigateTo(path, true);
    }
}

void MainWindow::navigateTo(const QString& path, bool recordHistory) {
    if (path.isEmpty()) return;
    
    spdlog::info("Navigating to: {}", path.toStdString());
    
    // Update state
    m_currentPath = QDir::toNativeSeparators(path);
    m_addressEdit->setText(m_currentPath);
    updateBreadcrumbs();
    setAddressEditMode(false);
    
    if (recordHistory) {
        // Truncate forward history
        if (m_historyIndex < m_history.size() - 1) {
            m_history.erase(m_history.begin() + m_historyIndex + 1, m_history.end());
        }
        
        // Push to history
        if (m_history.isEmpty() || m_history.last() != m_currentPath) {
            m_history.append(m_currentPath);
            m_historyIndex = m_history.size() - 1;
        }
    }
    
    // Update button states
    if (m_btnBack) m_btnBack->setEnabled(m_historyIndex > 0);
    if (m_btnForward) m_btnForward->setEnabled(m_historyIndex < m_history.size() - 1);
    
    // Disable Up button if at root
    if (m_btnUp) {
        QDir dir(m_currentPath);
        m_btnUp->setEnabled(dir.cdUp());
    }
    
    // Load path in views
    if (m_fileGrid) {
        m_fileGrid->loadPath(m_currentPath);
    }
    
    // Sync tree view selection
    m_pendingSyncPath = m_currentPath;
    auto* dirModel = qobject_cast<DirectoryItemModel*>(m_navTree->model());
    if (dirModel) {
        QModelIndex currentTreeIndex = m_navTree->selectionModel()->currentIndex();
        if (!currentTreeIndex.isValid()) {
            currentTreeIndex = dirModel->index(0, 0, QModelIndex()); 
        }
        
        m_navTree->expand(currentTreeIndex);
        
        bool found = false;
        int rows = dirModel->rowCount(currentTreeIndex);
        for (int i = 0; i < rows; ++i) {
            QModelIndex child = dirModel->index(i, 0, currentTreeIndex);
            if (dirModel->filePath(child) == m_currentPath) {
                m_navTree->selectionModel()->setCurrentIndex(child, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                m_navTree->scrollTo(child);
                m_navTree->expand(child);
                m_pendingSyncPath.clear();
                found = true;
                break;
            }
        }
        
        if (!found && dirModel->canFetchMore(currentTreeIndex)) {
            dirModel->fetchMore(currentTreeIndex);
        }
    }
}

void MainWindow::onBackClicked() {
    if (m_historyIndex > 0) {
        m_historyIndex--;
        navigateTo(m_history[m_historyIndex], false);
    }
}

void MainWindow::onForwardClicked() {
    if (m_historyIndex < m_history.size() - 1) {
        m_historyIndex++;
        navigateTo(m_history[m_historyIndex], false);
    }
}

void MainWindow::onUpClicked() {
    QDir dir(m_currentPath);
    if (dir.cdUp()) {
        navigateTo(dir.absolutePath(), true);
    }
}

void MainWindow::onRefreshClicked() {
    if (!m_currentPath.isEmpty()) {
        navigateTo(m_currentPath, false);
    }
}

void MainWindow::onAddressBarReturnPressed() {
    if (!m_addressEdit) return;
    QString path = m_addressEdit->text();
    if (!path.isEmpty()) {
        navigateTo(path, true);
    }
}

void MainWindow::setAddressEditMode(bool editMode) {
    if (!m_addressStack) return;
    if (editMode) {
        m_addressStack->setCurrentWidget(m_addressEdit);
        m_addressEdit->setFocus();
        m_addressEdit->selectAll();
    } else {
        m_addressStack->setCurrentWidget(m_addressStack->widget(0)); // The frame
    }
}

void MainWindow::updateBreadcrumbs() {
    if (!m_breadcrumbsContainer) return;
    
    // Clear existing
    QLayout* layout = m_breadcrumbsContainer->layout();
    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        layout = new QHBoxLayout(m_breadcrumbsContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(2);
    }
    
    // Split path into parts
    QString normalizedPath = QDir::fromNativeSeparators(m_currentPath);
    QStringList parts = normalizedPath.split('/', Qt::SkipEmptyParts);
    
    if (parts.isEmpty()) return;
    
    QString builtPath = "";
    
    for (int i = 0; i < parts.size(); ++i) {
        QString part = parts[i];
        
        // Build the absolute path up to this part
        if (i == 0 && part.endsWith(":")) {
            // Drive letter
            builtPath = part + "\\";
        } else {
            if (!builtPath.endsWith("\\") && !builtPath.endsWith("/")) {
                builtPath += "\\";
            }
            builtPath += part;
        }
        
        QToolButton* btn = new QToolButton(m_breadcrumbsContainer);
        btn->setText(part);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("QToolButton { border: none; padding: 4px; font-weight: bold; color: #333333; } QToolButton:hover { background-color: #E0E0E0; border-radius: 2px; }");
        
        // Add a separator chevron if not the last item
        if (i < parts.size() - 1) {
            QLabel* sep = new QLabel(">", m_breadcrumbsContainer);
            sep->setStyleSheet("color: #888888; margin: 0 2px;");
            
            layout->addWidget(btn);
            layout->addWidget(sep);
        } else {
            layout->addWidget(btn);
        }
        
        // Wire up the button
        connect(btn, &QToolButton::clicked, this, [this, targetPath = builtPath]() {
            navigateTo(targetPath, true);
        });
    }
}

void MainWindow::onTreeRowsInserted(const QModelIndex& parent, int first, int last) {
    if (m_pendingSyncPath.isEmpty()) return;
    
    auto* dirModel = qobject_cast<DirectoryItemModel*>(m_navTree->model());
    if (!dirModel) return;
    
    for (int i = first; i <= last; ++i) {
        QModelIndex child = dirModel->index(i, 0, parent);
        if (dirModel->filePath(child) == m_pendingSyncPath) {
            m_navTree->selectionModel()->setCurrentIndex(child, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            m_navTree->scrollTo(child);
            m_navTree->expand(child);
            m_pendingSyncPath.clear();
            break;
        }
    }
}

void MainWindow::setupActions() {
    auto *copyAction = new QAction(QStringLiteral("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MainWindow::onCopy);
    addAction(copyAction);

    auto *pasteAction = new QAction(QStringLiteral("Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::onPaste);
    addAction(pasteAction);

    auto *cutAction = new QAction(QStringLiteral("Cut"), this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &MainWindow::onCut);
    addAction(cutAction);

    auto *deleteAction = new QAction(QStringLiteral("Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDelete);
    addAction(deleteAction);
}

void MainWindow::onCopy() { spdlog::info("Copy action triggered"); }
void MainWindow::onPaste() { spdlog::info("Paste action triggered"); }
void MainWindow::onCut() { spdlog::info("Cut action triggered"); }
void MainWindow::onDelete() { spdlog::info("Delete action triggered"); }
