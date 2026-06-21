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
#include <QKeySequence>
#include <spdlog/spdlog.h>
#include <thread>

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
    
    // AI Command Box (Distinct styling)
    m_aiCommandBox = new QLineEdit(topBar);
    m_aiCommandBox->setPlaceholderText("Ask the AI to do something...");
    m_aiCommandBox->setStyleSheet("QLineEdit { border: 2px solid #0078D7; border-radius: 4px; padding: 4px; font-weight: bold; }");
    topLayout->addWidget(m_aiCommandBox, 1); // stretch factor 1
    
    // Search Box
    m_searchBox = new QLineEdit(topBar);
    m_searchBox->setPlaceholderText("Search...");
    m_searchBox->setMaximumWidth(300);
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
        spdlog::info("Directory selected: {}", path.toStdString());
        if (m_fileGrid) {
            m_fileGrid->loadPath(path);
        }
    }
}

void MainWindow::onSearchTriggered() {
    QString query = m_searchBox->text();
    spdlog::info("Search triggered for: {}", query.toStdString());
    if (m_fileGrid) {
        m_fileGrid->performSearch(query);
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
