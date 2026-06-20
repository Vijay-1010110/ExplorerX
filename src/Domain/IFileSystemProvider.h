#pragma once
#include "Path.h"
#include "FileItem.h"
#include "Expected.h"
#include <vector>
#include <future>
#include <functional>
#include <atomic>

namespace ExplorerX::Domain {

struct CopyRequest { Path Source; Path Destination; bool Overwrite; };
struct MoveRequest { Path Source; Path Destination; };
struct DeleteRequest { Path Target; bool Permanent; };
struct ListingResult { std::vector<FileItem> Items; };

struct ProgressContext {
    std::function<void(uint64_t bytesTransferred, uint64_t totalBytes, std::string_view currentFile)> OnProgress;
    std::atomic<bool>* IsCancelled = nullptr;
};

class IFileSystemProvider {
public:
    virtual ~IFileSystemProvider() = default;

    virtual std::future<Expected<ListingResult>> Enumerate(const Path& path) = 0;
    virtual std::future<Expected<FileItem>> GetMetadata(const Path& path) = 0;
    virtual std::future<Expected<void>> Copy(const CopyRequest& req, const ProgressContext& progress = {}) = 0;
    virtual std::future<Expected<void>> Move(const MoveRequest& req, const ProgressContext& progress = {}) = 0;
    virtual std::future<Expected<void>> Delete(const DeleteRequest& req, const ProgressContext& progress = {}) = 0;
};

} // namespace ExplorerX::Domain
