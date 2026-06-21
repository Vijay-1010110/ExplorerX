#pragma once
#include "IFileSystemProvider.h"
#include "ISearchEngine.h"
#include <memory>
#include <string>
#include <vector>

namespace ExplorerX::Core {

enum class AIAction {
    Unknown,
    Copy,
    Move,
    Delete,
    Search
};

struct AIIntent {
    AIAction Action = AIAction::Unknown;
    std::vector<Domain::Path> TargetPaths;
    Domain::Path DestinationPath; // Used for Copy/Move
    std::string SearchQuery;      // Used for Search
};

class AIIntentOrchestrator {
public:
    AIIntentOrchestrator(std::shared_ptr<Domain::IFileSystemProvider> fsProvider,
                         std::shared_ptr<Domain::ISearchEngine> searchEngine);

    // Parses a natural language string and executes the corresponding action asynchronously
    std::future<Domain::Expected<void>> ExecuteNaturalLanguageCommand(const std::string& command);

private:
    AIIntent ParseIntent(const std::string& command);
    std::future<Domain::Expected<void>> ExecuteIntent(const AIIntent& intent);

    std::shared_ptr<Domain::IFileSystemProvider> m_fsProvider;
    std::shared_ptr<Domain::ISearchEngine> m_searchEngine;
};

} // namespace ExplorerX::Core
