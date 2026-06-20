#pragma once
#include "Path.h"
#include "FileItem.h"
#include "Expected.h"
#include <string>
#include <vector>

namespace ExplorerX::Domain {

struct SearchQuery { 
    std::string Keyword; 
    Path RootPath; 
};
struct SearchResults { std::vector<FileItem> Matches; };

class ISearchEngine {
public:
    virtual ~ISearchEngine() = default;

    virtual Expected<void> IndexDirectory(const Path& path) = 0;
    virtual Expected<SearchResults> Query(const SearchQuery& query) = 0;
};

} // namespace ExplorerX::Domain
