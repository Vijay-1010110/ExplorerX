#include <QApplication>
#include <memory>
#include "MainWindow.h"
#include "../Platform/WinFileSystemProvider.h"
#include "../Platform/WinFileSystemWatcher.h"
#include "../Infrastructure/Index/SqliteSearchEngine.h"
#include "../Core/IndexOrchestrator.h"

#include "../Platform/WinThumbnailProvider.h"
#include "../Platform/LLMClient.h"
#include "../Core/ThumbnailOrchestrator.h"
#include "../Core/AIIntentOrchestrator.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    auto provider = std::make_shared<ExplorerX::Platform::WinFileSystemProvider>();
    auto watcher = std::make_shared<ExplorerX::Platform::WinFileSystemWatcher>();
    auto searchEngine = std::make_shared<ExplorerX::Infrastructure::Index::SqliteSearchEngine>("explorerx_index.db");
    
    // Thumbnails
    auto thumbProvider = std::make_shared<ExplorerX::Platform::WinThumbnailProvider>();
    auto thumbOrchestrator = std::make_shared<ExplorerX::Core::ThumbnailOrchestrator>(thumbProvider);
    
    // AI Intent Orchestrator
    auto llmClient = std::make_shared<ExplorerX::Platform::LLMClient>();
    auto aiOrchestrator = std::make_shared<ExplorerX::Core::AIIntentOrchestrator>(provider, searchEngine, llmClient);
    
    // Start indexer orchestration
    auto orchestrator = std::make_shared<ExplorerX::Core::IndexOrchestrator>(watcher, searchEngine);
    orchestrator->Start(ExplorerX::Domain::Path("C:\\"));
    
    MainWindow window(provider, searchEngine, thumbOrchestrator, aiOrchestrator);
    window.show();
    
    return app.exec();
}
