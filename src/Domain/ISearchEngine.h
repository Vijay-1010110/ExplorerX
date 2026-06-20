#pragma once
#include "Path.h"
#include "FileItem.h"
#include "Expected.h"
#include <string>
#include <vector>
#include <future>

namespace ExplorerX::Domain {

struct SearchQuery { 
    std::string Keyword; 
    Path RootPath; 
};
struct SearchResults { std::vector<FileItem> Matches; };

class ISearchEngine {
public:
    virtual ~ISearchEngine() = default;

    virtual std::future<Expected<void>> IndexDirectory(const Path& path) = 0;
    virtual std::future<Expected<void>> IndexFile(const Path& path) = 0;
    virtual std::future<Expected<void>> RemoveEntry(const Path& path) = 0;
    virtual std::future<Expected<SearchResults>> Query(const SearchQuery& query) = 0;
};

} // namespace ExplorerX::Domain
