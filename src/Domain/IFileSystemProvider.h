#pragma once
#include "Path.h"
#include "FileItem.h"
#include "Expected.h"
#include <vector>

namespace ExplorerX::Domain {

struct CopyRequest { Path Source; Path Destination; bool Overwrite; };
struct MoveRequest { Path Source; Path Destination; };
struct DeleteRequest { Path Target; bool Permanent; };
struct ListingResult { std::vector<FileItem> Items; };

class IFileSystemProvider {
public:
    virtual ~IFileSystemProvider() = default;

    virtual Expected<ListingResult> Enumerate(const Path& path) = 0;
    virtual Expected<void> Copy(const CopyRequest& req) = 0;
    virtual Expected<void> Move(const MoveRequest& req) = 0;
    virtual Expected<void> Delete(const DeleteRequest& req) = 0;
};

} // namespace ExplorerX::Domain
