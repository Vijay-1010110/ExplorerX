#include "SqliteSearchEngine.h"
#include <thread>
#include <iostream>

namespace ExplorerX::Infrastructure::Index {

SqliteSearchEngine::SqliteSearchEngine(const std::string& dbPath) : m_dbPath(dbPath) {
    auto result = InitializeDatabase();
    if (!result) {
        // In a real app we'd log this. For now, print to stderr.
        std::cerr << "Failed to initialize SQLite Search Engine: " << result.error().Message << "\n";
    }
}

SqliteSearchEngine::~SqliteSearchEngine() {
    std::lock_guard<std::mutex> lock(m_dbMutex);
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

Domain::Expected<void> SqliteSearchEngine::ExecSql(const char* sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err(errMsg ? errMsg : "Unknown SQLite error");
        if (errMsg) sqlite3_free(errMsg);
        return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, err});
    }
    return {};
}

Domain::Expected<void> SqliteSearchEngine::InitializeDatabase() {
    std::lock_guard<std::mutex> lock(m_dbMutex);
    
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Cannot open database: " + std::string(sqlite3_errmsg(m_db))});
    }

    const char* createFilesTable = R"(
        CREATE TABLE IF NOT EXISTS files (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            path TEXT UNIQUE NOT NULL,
            name TEXT NOT NULL,
            size INTEGER NOT NULL,
            last_modified INTEGER NOT NULL,
            parent_path TEXT NOT NULL
        );
    )";
    auto result = ExecSql(createFilesTable);
    if (!result) return result;

    const char* createFtsTable = R"(
        CREATE VIRTUAL TABLE IF NOT EXISTS files_fts USING fts5(
            name, 
            path UNINDEXED, 
            content='files', 
            content_rowid='id'
        );
    )";
    result = ExecSql(createFtsTable);
    if (!result) return result;

    // FTS triggers
    const char* createTriggers = R"(
        CREATE TRIGGER IF NOT EXISTS files_ai AFTER INSERT ON files BEGIN
          INSERT INTO files_fts(rowid, name) VALUES (new.id, new.name);
        END;

        CREATE TRIGGER IF NOT EXISTS files_ad AFTER DELETE ON files BEGIN
          INSERT INTO files_fts(files_fts, rowid, name) VALUES('delete', old.id, old.name);
        END;

        CREATE TRIGGER IF NOT EXISTS files_au AFTER UPDATE ON files BEGIN
          INSERT INTO files_fts(files_fts, rowid, name) VALUES('delete', old.id, old.name);
          INSERT INTO files_fts(rowid, name) VALUES (new.id, new.name);
        END;
    )";
    return ExecSql(createTriggers);
}

std::future<Domain::Expected<void>> SqliteSearchEngine::IndexDirectory(const Domain::Path& path) {
    // Return an asynchronous operation
    return std::async(std::launch::async, [this, path]() -> Domain::Expected<void> {
        // Placeholder for actual recursive directory indexing.
        std::lock_guard<std::mutex> lock(m_dbMutex);
        if (!m_db) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Database not initialized"});
        }
        
        // For scaffolding, we just return success
        return {};
    });
}

std::future<Domain::Expected<Domain::SearchResults>> SqliteSearchEngine::Query(const Domain::SearchQuery& query) {
    return std::async(std::launch::async, [this, query]() -> Domain::Expected<Domain::SearchResults> {
        std::lock_guard<std::mutex> lock(m_dbMutex);
        if (!m_db) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Database not initialized"});
        }

        // Placeholder for real search implementation using FTS5
        Domain::SearchResults results;
        return results;
    });
}

} // namespace ExplorerX::Infrastructure::Index
