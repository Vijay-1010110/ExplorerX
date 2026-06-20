#include "MainWindow.h"
#include <QToolBar>
#include <QDockWidget>
#include <QWidget>
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

    // Left dock widget (navigation tree)
    QDockWidget *navDock = new QDockWidget(QStringLiteral("Navigation"), this);
    navDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QTreeView *navTree = new QTreeView(navDock);
    navDock->setWidget(navTree);
    addDockWidget(Qt::LeftDockWidgetArea, navDock);

    // Central widget (file grid)
    QTableView *fileGrid = new QTableView(this);
    setCentralWidget(fileGrid);
}
