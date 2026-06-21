#pragma once
#include "IThumbnailProvider.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <string>

namespace ExplorerX::Core {

class ThumbnailOrchestrator {
public:
    explicit ThumbnailOrchestrator(std::shared_ptr<Domain::IThumbnailProvider> provider);

    std::future<Domain::Expected<Domain::ThumbnailImage>> GetThumbnailAsync(const Domain::Path& path, int targetSize);

private:
    std::shared_ptr<Domain::IThumbnailProvider> m_provider;
    
    std::mutex m_cacheMutex;
    // Cache key maps to the ThumbnailImage
    std::unordered_map<std::string, Domain::ThumbnailImage> m_cache;
};

} // namespace ExplorerX::Core
