#pragma once
#include "Path.h"
#include <string>
#include <cstdint>
#include <chrono>

namespace ExplorerX::Domain {

struct FileItem {
    Path ItemPath;
    std::string Name;
    uint64_t Size = 0;
    bool IsDirectory = false;
    bool IsHidden = false;
    bool IsSystem = false;
    std::chrono::system_clock::time_point DateCreated;
    std::chrono::system_clock::time_point DateModified;
    std::chrono::system_clock::time_point DateAccessed;
};

} // namespace ExplorerX::Domain
