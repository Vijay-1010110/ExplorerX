#pragma once

#include "IFileSystemProvider.h"
#include "Expected.h"

namespace ExplorerX::Platform {

class WinFileSystemProvider : public Domain::IFileSystemProvider {
public:
    WinFileSystemProvider() = default;
    ~WinFileSystemProvider() override = default;

    std::future<Domain::Expected<Domain::ListingResult>> Enumerate(const Domain::Path& path) override;
    std::future<Domain::Expected<Domain::FileItem>> GetMetadata(const Domain::Path& path) override;
    std::future<Domain::Expected<void>> Copy(const Domain::CopyRequest& req, const Domain::ProgressContext& progress = {}) override;
    std::future<Domain::Expected<void>> Move(const Domain::MoveRequest& req, const Domain::ProgressContext& progress = {}) override;
    std::future<Domain::Expected<void>> Delete(const Domain::DeleteRequest& req, const Domain::ProgressContext& progress = {}) override;
};

} // namespace ExplorerX::Platform
