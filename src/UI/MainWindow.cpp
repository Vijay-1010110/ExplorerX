#include "MainWindow.h"
#include <QToolBar>
#include <QSplitter>
#include <QLineEdit>
#include "Views/DirectoryTreeView.h"
#include "Views/FileGridView.h"
#include "ViewModels/DirectoryItemModel.h"
#include <QKeySequence>
#include <spdlog/spdlog.h>

MainWindow::MainWindow(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                       std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                       std::shared_ptr<ExplorerX::Core::ThumbnailOrchestrator> thumbOrchestrator,
                       QWidget *parent)
    : QMainWindow(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)), m_thumbOrchestrator(std::move(thumbOrchestrator)) {
    setupUi();
    setupActions();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle(QStringLiteral("ExplorerX"));
    resize(1024, 768);

    // Top toolbar (address bar)
    QToolBar *addressBar = addToolBar(QStringLiteral("Address Bar"));
    addressBar->setMovable(false);

    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText(QStringLiteral("Search..."));
    addressBar->addWidget(m_searchBox);
    connect(m_searchBox, &QLineEdit::returnPressed, this, &MainWindow::onSearchTriggered);

    // Main splitter for navigation and file grid
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Left pane (navigation tree)
    m_navTree = new DirectoryTreeView(m_provider, mainSplitter);
    mainSplitter->addWidget(m_navTree);

    // Right pane (file grid)
    m_fileGrid = new FileGridView(m_provider, m_searchEngine, m_thumbOrchestrator, mainSplitter);
    mainSplitter->addWidget(m_fileGrid);

    // Wire navigation tree clicks to the file grid
    connect(m_navTree, &DirectoryTreeView::clicked, this, &MainWindow::onDirectorySelected);

    // Initial sizes for splitter (e.g., 25% vs 75%)
    mainSplitter->setSizes({250, 750});

    setCentralWidget(mainSplitter);
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
