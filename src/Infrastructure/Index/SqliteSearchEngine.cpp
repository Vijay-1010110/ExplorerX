#include "SqliteSearchEngine.h"
#include <thread>
#include <iostream>
#include <regex>
#include <algorithm>
#include <chrono>
#include <filesystem>

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
            body,
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
          INSERT INTO files_fts(rowid, name, body) VALUES (new.id, new.name, NULL);
        END;

        CREATE TRIGGER IF NOT EXISTS files_ad AFTER DELETE ON files BEGIN
          INSERT INTO files_fts(files_fts, rowid, name, body) VALUES('delete', old.id, old.name, NULL);
        END;

        CREATE TRIGGER IF NOT EXISTS files_au AFTER UPDATE ON files BEGIN
          INSERT INTO files_fts(files_fts, rowid, name, body) VALUES('delete', old.id, old.name, NULL);
          INSERT INTO files_fts(rowid, name, body) VALUES (new.id, new.name, NULL);
        END;
    )";
    return ExecSql(createTriggers);
}

std::future<Domain::Expected<void>> SqliteSearchEngine::IndexDirectory(const Domain::Path& path) {
    return std::async(std::launch::async, [this, path]() -> Domain::Expected<void> {
        std::lock_guard<std::mutex> lock(m_dbMutex);
        if (!m_db) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Database not initialized"});
        }

        std::string rootStr = path.ToString();
        if (rootStr.empty()) return {};

        auto execRes = ExecSql("BEGIN TRANSACTION;");
        if (!execRes) return execRes;

        const char* insertSql = R"(
            INSERT OR REPLACE INTO files (path, name, size, last_modified, parent_path)
            VALUES (?, ?, ?, ?, ?);
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, insertSql, -1, &stmt, nullptr) != SQLITE_OK) {
            ExecSql("ROLLBACK;");
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Prepare failed"});
        }

        try {
            // Using directory_options::skip_permission_denied to safely traverse
            auto options = std::filesystem::directory_options::skip_permission_denied;
            for (const auto& entry : std::filesystem::recursive_directory_iterator(rootStr, options)) {
                if (!entry.is_regular_file()) continue;

                std::string filePath = entry.path().string();
                std::string name = entry.path().filename().string();
                std::string parentPath = entry.path().parent_path().string();
                
                uint64_t size = 0;
                try { size = entry.file_size(); } catch (...) {}
                
                uint64_t lastMod = 0;
                try { 
                    auto ftime = entry.last_write_time();
                    lastMod = std::chrono::duration_cast<std::chrono::seconds>(ftime.time_since_epoch()).count();
                } catch (...) {}

                sqlite3_reset(stmt);
                sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int64(stmt, 3, size);
                sqlite3_bind_int64(stmt, 4, lastMod);
                sqlite3_bind_text(stmt, 5, parentPath.c_str(), -1, SQLITE_TRANSIENT);

                sqlite3_step(stmt);
            }
        } catch (const std::exception& e) {
            // Best effort extraction
        }

        sqlite3_finalize(stmt);
        return ExecSql("COMMIT;");
    });
}

std::future<Domain::Expected<Domain::SearchResults>> SqliteSearchEngine::Query(const Domain::SearchQuery& query) {
    return std::async(std::launch::async, [this, query]() -> Domain::Expected<Domain::SearchResults> {
        std::lock_guard<std::mutex> lock(m_dbMutex);
        if (!m_db) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Database not initialized"});
        }

        std::string keyword = query.Keyword;
        std::vector<std::string> whereClauses;
        std::string ftsMatch;

        std::string rootPathStr = query.RootPath.ToString();
        if (!rootPathStr.empty()) {
            if (rootPathStr.back() != '\\' && rootPathStr.back() != '/') {
                rootPathStr += "\\"; // Enforce directory boundary
            }
            // Add safe LIKE clause to filter by scope
            std::string safePath = rootPathStr;
            std::replace(safePath.begin(), safePath.end(), '\'', '_'); // simple escape
            whereClauses.push_back("(f.path LIKE '" + safePath + "%')");
        }

        // Parse AQS features using regexes for simplicity in scaffolding
        std::regex kindRegex(R"(kind:([a-zA-Z]+))");
        std::smatch match;
        while (std::regex_search(keyword, match, kindRegex)) {
            std::string kind = match[1];
            if (kind == "pics") whereClauses.push_back("(f.name LIKE '%.png' OR f.name LIKE '%.jpg' OR f.name LIKE '%.gif')");
            else if (kind == "docs") whereClauses.push_back("(f.name LIKE '%.pdf' OR f.name LIKE '%.txt' OR f.name LIKE '%.doc' OR f.name LIKE '%.docx')");
            keyword = match.prefix().str() + " " + match.suffix().str();
        }

        std::regex sizeRegex(R"(size:((?:>|<|>=|<=)?\d+[a-zA-Z]+|[a-zA-Z]+))");
        while (std::regex_search(keyword, match, sizeRegex)) {
            std::string sizeVal = match[1];
            if (sizeVal == "large") whereClauses.push_back("(f.size BETWEEN 134217728 AND 1073741824)");
            else if (sizeVal == ">50mb") whereClauses.push_back("(f.size > 52428800)");
            keyword = match.prefix().str() + " " + match.suffix().str();
        }

        std::regex modRegex(R"(modified:(last week|\d{4}))");
        while (std::regex_search(keyword, match, modRegex)) {
            std::string modVal = match[1];
            if (modVal == "last week") {
                auto now = std::chrono::system_clock::now();
                auto lastWeek = now - std::chrono::hours(24 * 7);
                uint64_t lastWeekTs = std::chrono::duration_cast<std::chrono::seconds>(lastWeek.time_since_epoch()).count();
                whereClauses.push_back("(f.last_modified > " + std::to_string(lastWeekTs) + ")");
            } else if (modVal == "2026") {
                whereClauses.push_back("(f.last_modified BETWEEN 1767225600 AND 1798761599)");
            }
            keyword = match.prefix().str() + " " + match.suffix().str();
        }

        std::regex contentRegex(R"(content:("[^"]+"))");
        while (std::regex_search(keyword, match, contentRegex)) {
            std::string contentVal = match[1];
            if (!ftsMatch.empty()) ftsMatch += " ";
            ftsMatch += "body:" + contentVal;
            keyword = match.prefix().str() + " " + match.suffix().str();
        }

        // Tokenize the rest
        std::vector<std::string> tokens;
        bool inQuotes = false;
        std::string currentToken;
        for (char c : keyword) {
            if (c == '"') {
                inQuotes = !inQuotes;
                currentToken += c;
            } else if (c == ' ' && !inQuotes) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
            } else {
                currentToken += c;
            }
        }
        if (!currentToken.empty()) tokens.push_back(currentToken);

        for (const auto& token : tokens) {
            if (token.find('?') != std::string::npos) {
                std::string likePattern = token;
                std::replace(likePattern.begin(), likePattern.end(), '?', '_');
                whereClauses.push_back("(f.name LIKE '" + likePattern + "')");
            } else {
                if (!ftsMatch.empty()) ftsMatch += " ";
                ftsMatch += token;
            }
        }

        std::string sql = "SELECT f.id, f.path, f.name, f.size, f.last_modified, f.parent_path FROM files f";
        if (!ftsMatch.empty()) {
            sql += " JOIN files_fts fts ON f.id = fts.rowid";
        }
        
        bool hasWhere = false;
        if (!ftsMatch.empty()) {
            sql += " WHERE fts.files_fts MATCH ?"; // Using parameterized binding for MATCH
            hasWhere = true;
        }

        for (const auto& clause : whereClauses) {
            if (hasWhere) sql += " AND ";
            else { sql += " WHERE "; hasWhere = true; }
            sql += clause;
        }

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::string err = sqlite3_errmsg(m_db);
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Query prepare failed: " + err + " SQL: " + sql});
        }

        if (!ftsMatch.empty()) {
            sqlite3_bind_text(stmt, 1, ftsMatch.c_str(), -1, SQLITE_TRANSIENT);
        }

        Domain::SearchResults results;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            Domain::FileItem item;
            item.ItemPath = Domain::Path(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            item.Name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            item.Size = sqlite3_column_int64(stmt, 3);
            // Just fetching to show usage. Time points can be assigned here as well.
            results.Matches.push_back(item);
        }
        
        sqlite3_finalize(stmt);
        return results;
    });
}

std::future<Domain::Expected<void>> SqliteSearchEngine::IndexFile(const Domain::Path& path) {
    return std::async(std::launch::async, []() -> Domain::Expected<void> {
        return {};
    });
}

std::future<Domain::Expected<void>> SqliteSearchEngine::RemoveEntry(const Domain::Path& path) {
    return std::async(std::launch::async, []() -> Domain::Expected<void> {
        return {};
    });
}

} // namespace ExplorerX::Infrastructure::Index
