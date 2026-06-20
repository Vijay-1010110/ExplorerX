#include "IndexOrchestrator.h"

namespace ExplorerX::Core {

IndexOrchestrator::IndexOrchestrator(std::shared_ptr<Domain::IFileSystemWatcher> watcher, 
                                     std::shared_ptr<Domain::ISearchEngine> searchEngine)
    : m_watcher(std::move(watcher)), m_searchEngine(std::move(searchEngine)) 
{
}

void IndexOrchestrator::Start(const Domain::Path& watchPath) {
    m_watcher->StartWatching(watchPath, [this](const Domain::FileSystemEvent& event) {
        HandleFileSystemEvent(event);
    });
}

void IndexOrchestrator::Stop() {
    m_watcher->StopWatching();
}

void IndexOrchestrator::HandleFileSystemEvent(const Domain::FileSystemEvent& event) {
    switch (event.Type) {
        case Domain::FileSystemEventType::Created:
        case Domain::FileSystemEventType::Modified:
            // Delegate to the search engine to index the new or updated file
            m_searchEngine->IndexFile(event.TargetPath);
            break;
        case Domain::FileSystemEventType::Deleted:
            // Remove the deleted file from the index
            m_searchEngine->RemoveEntry(event.TargetPath);
            break;
        case Domain::FileSystemEventType::Renamed:
            // A rename is effectively a delete of the old path and an index of the new path
            m_searchEngine->RemoveEntry(event.OldPath);
            m_searchEngine->IndexFile(event.TargetPath);
            break;
    }
}

} // namespace ExplorerX::Core
