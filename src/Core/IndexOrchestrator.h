#pragma once
#include "IFileSystemWatcher.h"
#include "ISearchEngine.h"
#include <memory>

namespace ExplorerX::Core {

class IndexOrchestrator {
public:
    IndexOrchestrator(std::shared_ptr<Domain::IFileSystemWatcher> watcher, 
                      std::shared_ptr<Domain::ISearchEngine> searchEngine);

    void Start(const Domain::Path& watchPath);
    void Stop();

private:
    void HandleFileSystemEvent(const Domain::FileSystemEvent& event);

    std::shared_ptr<Domain::IFileSystemWatcher> m_watcher;
    std::shared_ptr<Domain::ISearchEngine> m_searchEngine;
};

} // namespace ExplorerX::Core
