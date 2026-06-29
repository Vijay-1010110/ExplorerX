#include "AIIntentOrchestrator.h"

namespace ExplorerX::Core {

AIIntentOrchestrator::AIIntentOrchestrator(std::shared_ptr<Domain::IFileSystemProvider> fsProvider,
                                           std::shared_ptr<Domain::ISearchEngine> searchEngine,
                                           std::shared_ptr<Domain::ILLMClient> llmClient,
                                           QObject* parent)
    : QObject(parent), m_fsProvider(std::move(fsProvider)), m_searchEngine(std::move(searchEngine)), m_llmClient(std::move(llmClient))
{
}

std::future<Domain::Expected<void>> AIIntentOrchestrator::ExecuteIntent(const Domain::AIIntent& intent) {
    return std::async(std::launch::async, [this, intent]() -> Domain::Expected<void> {
        switch (intent.Action) {
            case Domain::AIAction::Delete: {
                for (const auto& target : intent.TargetPaths) {
                    Domain::DeleteRequest req{ target, false };
                    auto result = m_fsProvider->Delete(req).get();
                    if (!result.has_value()) return Domain::MakeUnexpected(result.error());
                }
                break;
            }
            case Domain::AIAction::Copy: {
                for (const auto& target : intent.TargetPaths) {
                    Domain::CopyRequest req{ target, intent.DestinationPath, false };
                    auto result = m_fsProvider->Copy(req).get();
                    if (!result.has_value()) return Domain::MakeUnexpected(result.error());
                }
                break;
            }
            case Domain::AIAction::Move: {
                for (const auto& target : intent.TargetPaths) {
                    Domain::MoveRequest req{ target, intent.DestinationPath };
                    auto result = m_fsProvider->Move(req).get();
                    if (!result.has_value()) return Domain::MakeUnexpected(result.error());
                }
                break;
            }
            case Domain::AIAction::Search: {
                if (!intent.SearchQuery.empty()) {
                    Domain::SearchQuery sq{ intent.SearchQuery, Domain::Path("") };
                    if (!intent.TargetPaths.empty()) {
                        sq.RootPath = intent.TargetPaths[0];
                    }
                    auto result = m_searchEngine->Query(sq).get();
                    if (result.has_value()) {
                        emit SearchResultsReady(result.value().Matches);
                    } else {
                        return Domain::MakeUnexpected(result.error());
                    }
                }
                break;
            }
            default:
                return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::InvalidFormat, "Command not recognized" });
        }
        return {};
    });
}

std::future<Domain::Expected<void>> AIIntentOrchestrator::ExecuteNaturalLanguageCommand(const std::string& command, const std::string& currentContextPath) {
    return std::async(std::launch::async, [this, command, currentContextPath]() -> Domain::Expected<void> {
        auto intentResult = m_llmClient->ParseIntent(command).get();
        if (!intentResult.has_value()) {
            return Domain::MakeUnexpected(intentResult.error());
        }
        
        auto intent = intentResult.value();
        if (intent.TargetPaths.empty() && !currentContextPath.empty()) {
            intent.TargetPaths.push_back(Domain::Path(currentContextPath));
        }
        
        return ExecuteIntent(intent).get();
    });
}

} // namespace ExplorerX::Core
