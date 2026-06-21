#include "AIIntentOrchestrator.h"
#include <algorithm>
#include <cctype>

namespace ExplorerX::Core {

AIIntentOrchestrator::AIIntentOrchestrator(std::shared_ptr<Domain::IFileSystemProvider> fsProvider,
                                           std::shared_ptr<Domain::ISearchEngine> searchEngine)
    : m_fsProvider(std::move(fsProvider)), m_searchEngine(std::move(searchEngine))
{
}

AIIntent AIIntentOrchestrator::ParseIntent(const std::string& command) {
    AIIntent intent;
    std::string lowerCmd = command;
    std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), 
                   [](unsigned char c){ return std::tolower(c); });

    // Dummy parsing implementation
    if (lowerCmd.find("delete") != std::string::npos) {
        intent.Action = AIAction::Delete;
        intent.TargetPaths.push_back(Domain::Path("C:\\dummy\\delete\\target.txt"));
    } else if (lowerCmd.find("copy") != std::string::npos) {
        intent.Action = AIAction::Copy;
        intent.TargetPaths.push_back(Domain::Path("C:\\dummy\\copy\\source.txt"));
        intent.DestinationPath = Domain::Path("C:\\dummy\\copy\\dest.txt");
    } else if (lowerCmd.find("search") != std::string::npos) {
        intent.Action = AIAction::Search;
        intent.SearchQuery = "dummy_keyword";
    }

    return intent;
}

std::future<Domain::Expected<void>> AIIntentOrchestrator::ExecuteIntent(const AIIntent& intent) {
    switch (intent.Action) {
        case AIAction::Delete:
            if (!intent.TargetPaths.empty()) {
                Domain::DeleteRequest req{ intent.TargetPaths[0], false };
                return m_fsProvider->Delete(req);
            }
            break;
        case AIAction::Copy:
            if (!intent.TargetPaths.empty()) {
                Domain::CopyRequest req{ intent.TargetPaths[0], intent.DestinationPath, false };
                return m_fsProvider->Copy(req);
            }
            break;
        case AIAction::Search:
            if (!intent.SearchQuery.empty()) {
                // We just mock completion since Search returns SearchResults
                return std::async(std::launch::async, []() -> Domain::Expected<void> {
                    return {};
                });
            }
            break;
        default:
            return std::async(std::launch::async, []() -> Domain::Expected<void> {
                return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::InvalidFormat, "Command not recognized" });
            });
    }

    return std::async(std::launch::async, []() -> Domain::Expected<void> { return {}; });
}

std::future<Domain::Expected<void>> AIIntentOrchestrator::ExecuteNaturalLanguageCommand(const std::string& command) {
    AIIntent intent = ParseIntent(command);
    return ExecuteIntent(intent);
}

} // namespace ExplorerX::Core
