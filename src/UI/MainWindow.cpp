#include "MainWindow.h"
#include <QToolBar>
#include <QSplitter>
#include "Views/DirectoryTreeView.h"
#include "Views/FileGridView.h"

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
    DirectoryTreeView *navTree = new DirectoryTreeView(mainSplitter);
    mainSplitter->addWidget(navTree);

    // Right pane (file grid)
    FileGridView *fileGrid = new FileGridView(mainSplitter);
    mainSplitter->addWidget(fileGrid);

    // Initial sizes for splitter (e.g., 25% vs 75%)
    mainSplitter->setSizes({250, 750});

    setCentralWidget(mainSplitter);
}
