#pragma once

#include "Expected.h"
#include "Path.h"
#include <vector>
#include <cstdint>
#include <future>

namespace ExplorerX::Platform {

class WinThumbnailProvider {
public:
    WinThumbnailProvider() = default;
    ~WinThumbnailProvider() = default;

    // Asynchronously extracts a thumbnail or icon for the given path, returning PNG bytes.
    // size: requested thumbnail/icon dimension (e.g., 256 for a 256x256 image).
    std::future<Domain::Expected<std::vector<uint8_t>>> GetThumbnail(const Domain::Path& path, int size = 256);
};

} // namespace ExplorerX::Platform
