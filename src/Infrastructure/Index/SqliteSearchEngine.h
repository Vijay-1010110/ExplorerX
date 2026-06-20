#pragma once
#include "ISearchEngine.h"
#include <memory>
#include <mutex>
#include <string>
#include <sqlite3.h>

namespace ExplorerX::Infrastructure::Index {

class SqliteSearchEngine : public Domain::ISearchEngine {
public:
    SqliteSearchEngine(const std::string& dbPath);
    ~SqliteSearchEngine() override;

    std::future<Domain::Expected<void>> IndexDirectory(const Domain::Path& path) override;
    std::future<Domain::Expected<void>> IndexFile(const Domain::Path& path) override;
    std::future<Domain::Expected<void>> RemoveEntry(const Domain::Path& path) override;
    std::future<Domain::Expected<Domain::SearchResults>> Query(const Domain::SearchQuery& query) override;

private:
    Domain::Expected<void> InitializeDatabase();
    Domain::Expected<void> ExecSql(const char* sql);

    std::string m_dbPath;
    sqlite3* m_db = nullptr;
    std::mutex m_dbMutex;
};

} // namespace ExplorerX::Infrastructure::Index
