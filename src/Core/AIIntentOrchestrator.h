#pragma once
#include "IFileSystemProvider.h"
#include "ISearchEngine.h"
#include <memory>
#include <string>
#include <vector>

#include "../Domain/ILLMClient.h"

namespace ExplorerX::Core {

class AIIntentOrchestrator {
public:
    AIIntentOrchestrator(std::shared_ptr<Domain::IFileSystemProvider> fsProvider,
                         std::shared_ptr<Domain::ISearchEngine> searchEngine,
                         std::shared_ptr<Domain::ILLMClient> llmClient);

    // Parses a natural language string and executes the corresponding action asynchronously
    std::future<Domain::Expected<void>> ExecuteNaturalLanguageCommand(const std::string& command);

private:
    std::future<Domain::Expected<void>> ExecuteIntent(const Domain::AIIntent& intent);

    std::shared_ptr<Domain::IFileSystemProvider> m_fsProvider;
    std::shared_ptr<Domain::ISearchEngine> m_searchEngine;
    std::shared_ptr<Domain::ILLMClient> m_llmClient;
};

} // namespace ExplorerX::Core
