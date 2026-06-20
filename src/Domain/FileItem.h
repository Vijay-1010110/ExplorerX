#pragma once
#include "Path.h"
#include <string>
#include <cstdint>

namespace ExplorerX::Domain {

struct FileItem {
    Path ItemPath;
    std::string Name;
    uint64_t Size = 0;
    bool IsDirectory = false;
    // other metadata...
};

} // namespace ExplorerX::Domain
