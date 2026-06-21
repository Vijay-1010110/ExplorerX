#pragma once

#include "Expected.h"
#include "Path.h"
#include <vector>
#include <cstdint>
#include <future>

#include "../Domain/IThumbnailProvider.h"

namespace ExplorerX::Platform {

class WinThumbnailProvider : public Domain::IThumbnailProvider {
public:
    WinThumbnailProvider() = default;
    ~WinThumbnailProvider() override = default;

    std::future<Domain::Expected<Domain::ThumbnailImage>> GetThumbnailAsync(const Domain::Path& path, int targetSize) override;
};

} // namespace ExplorerX::Platform
