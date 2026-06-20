#include <QApplication>
#include <memory>
#include "MainWindow.h"
#include "../Platform/WinFileSystemProvider.h"
#include "../Platform/WinFileSystemWatcher.h"
#include "../Infrastructure/Index/SqliteSearchEngine.h"
#include "../Core/IndexOrchestrator.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    auto provider = std::make_shared<ExplorerX::Platform::WinFileSystemProvider>();
    auto watcher = std::make_shared<ExplorerX::Platform::WinFileSystemWatcher>();
    auto searchEngine = std::make_shared<ExplorerX::Infrastructure::Index::SqliteSearchEngine>("explorerx_index.db");
    
    // Start indexer orchestration
    auto orchestrator = std::make_shared<ExplorerX::Core::IndexOrchestrator>(watcher, searchEngine);
    orchestrator->Start(ExplorerX::Domain::Path("C:\\"));
    
    MainWindow window(provider, searchEngine);
    window.show();
    
    return app.exec();
}
