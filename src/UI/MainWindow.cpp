#include "MainWindow.h"
#include <QToolBar>
#include <QSplitter>
#include <QTableView>
#include <QTreeView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUi();
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
    QTreeView *navTree = new QTreeView(mainSplitter);
    mainSplitter->addWidget(navTree);

    // Right pane (file grid)
    QTableView *fileGrid = new QTableView(mainSplitter);
    mainSplitter->addWidget(fileGrid);

    // Initial sizes for splitter (e.g., 25% vs 75%)
    mainSplitter->setSizes({250, 750});

    setCentralWidget(mainSplitter);
}
