#include "AIIntentOrchestrator.h"

namespace ExplorerX::Core {

AIIntentOrchestrator::AIIntentOrchestrator(std::shared_ptr<Domain::IFileSystemProvider> fsProvider,
                                           std::shared_ptr<Domain::ISearchEngine> searchEngine,
                                           std::shared_ptr<Domain::ILLMClient> llmClient)
    : m_fsProvider(std::move(fsProvider)), m_searchEngine(std::move(searchEngine)), m_llmClient(std::move(llmClient))
{
}

std::future<Domain::Expected<void>> AIIntentOrchestrator::ExecuteIntent(const Domain::AIIntent& intent) {
    switch (intent.Action) {
        case Domain::AIAction::Delete:
            if (!intent.TargetPaths.empty()) {
                Domain::DeleteRequest req{ intent.TargetPaths[0], false };
                return m_fsProvider->Delete(req);
            }
            break;
        case Domain::AIAction::Copy:
            if (!intent.TargetPaths.empty()) {
                Domain::CopyRequest req{ intent.TargetPaths[0], intent.DestinationPath, false };
                return m_fsProvider->Copy(req);
            }
            break;
        case Domain::AIAction::Search:
            if (!intent.SearchQuery.empty()) {
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
    return std::async(std::launch::async, [this, command]() -> Domain::Expected<void> {
        auto intentResult = m_llmClient->ParseIntent(command).get();
        if (!intentResult.has_value()) {
            return Domain::MakeUnexpected(intentResult.error());
        }
        return ExecuteIntent(intentResult.value()).get();
    });
}

} // namespace ExplorerX::Core
