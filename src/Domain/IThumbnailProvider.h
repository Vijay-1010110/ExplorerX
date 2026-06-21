#pragma once
#include "Path.h"
#include "Expected.h"
#include <vector>
#include <cstdint>
#include <future>

namespace ExplorerX::Domain {

struct ThumbnailImage {
    std::vector<uint8_t> Data;
    int Width = 0;
    int Height = 0;
};

class IThumbnailProvider {
public:
    virtual ~IThumbnailProvider() = default;

    // Asynchronously fetches thumbnail image bytes for a given path
    virtual std::future<Expected<ThumbnailImage>> GetThumbnailAsync(const Path& path, int targetSize) = 0;
};

} // namespace ExplorerX::Domain
