#include "ThumbnailOrchestrator.h"

namespace ExplorerX::Core {

ThumbnailOrchestrator::ThumbnailOrchestrator(std::shared_ptr<Domain::IThumbnailProvider> provider)
    : m_provider(std::move(provider)) {}

std::future<Domain::Expected<Domain::ThumbnailImage>> ThumbnailOrchestrator::GetThumbnailAsync(const Domain::Path& path, int targetSize) {
    std::string cacheKey = path.ToString() + "_" + std::to_string(targetSize);

    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        auto it = m_cache.find(cacheKey);
        if (it != m_cache.end()) {
            std::promise<Domain::Expected<Domain::ThumbnailImage>> p;
            p.set_value(it->second);
            return p.get_future();
        }
    }

    return std::async(std::launch::async, [this, path, targetSize, cacheKey]() -> Domain::Expected<Domain::ThumbnailImage> {
        auto future = m_provider->GetThumbnailAsync(path, targetSize);
        auto result = future.get();
        
        if (result) {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_cache[cacheKey] = result.value();
        }
        
        return result;
    });
}

} // namespace ExplorerX::Core
