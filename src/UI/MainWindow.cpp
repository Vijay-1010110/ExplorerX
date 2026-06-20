#include "MainWindow.h"
#include <QToolBar>
#include <QSplitter>
#include "Views/DirectoryTreeView.h"
#include "Views/FileGridView.h"
#include <QKeySequence>
#include <spdlog/spdlog.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
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

    // Main splitter for navigation and file grid
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Left pane (navigation tree)
    DirectoryTreeView *navTree = new DirectoryTreeView(mainSplitter);
    mainSplitter->addWidget(navTree);

    // Right pane (file grid)
    FileGridView *fileGrid = new FileGridView(mainSplitter);
    mainSplitter->addWidget(fileGrid);

    // Initial sizes for splitter (e.g., 25% vs 75%)
    mainSplitter->setSizes({250, 750});

    setCentralWidget(mainSplitter);
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
