#pragma once

#include "IFileSystemProvider.h"
#include "Expected.h"

namespace ExplorerX::Platform {

class WinFileSystemProvider : public Domain::IFileSystemProvider {
public:
    WinFileSystemProvider() = default;
    ~WinFileSystemProvider() override = default;

    std::future<Domain::Expected<Domain::ListingResult>> Enumerate(const Domain::Path& path) override;
    Domain::Expected<void> Copy(const Domain::CopyRequest& req) override;
    Domain::Expected<void> Move(const Domain::MoveRequest& req) override;
    Domain::Expected<void> Delete(const Domain::DeleteRequest& req) override;
};

} // namespace ExplorerX::Platform
