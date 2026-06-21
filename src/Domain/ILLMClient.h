#pragma once
#include "Path.h"
#include "Expected.h"
#include <vector>
#include <string>
#include <future>

namespace ExplorerX::Domain {

enum class AIAction {
    Unknown,
    Copy,
    Move,
    Delete,
    Search
};

struct AIIntent {
    AIAction Action = AIAction::Unknown;
    std::vector<Path> TargetPaths;
    Path DestinationPath;
    std::string SearchQuery;
};

class ILLMClient {
public:
    virtual ~ILLMClient() = default;
    virtual std::future<Expected<AIIntent>> ParseIntent(const std::string& prompt) = 0;
};

} // namespace ExplorerX::Domain
