#pragma once

#include "DatabaseConfig.h"
#include "DBParam.h"
#include "DatabaseException.h"
#include "DatabaseHealth.h"
#include "DatabaseStatistics.h"
#include "ConnectionState.h"
#include "QueryType.h"
#include "LogLevel.h"
#include "Row.h"
#include "ResultSet.h"
#include "QueryResult.h"
#include "DatabaseErrorMapper.h"
#include "SQLConstants.h"

#include <memory>
#include <mutex>
#include <list>
#include <unordered_map>
#include <ctime>
#include <chrono>
#include <thread>
#include <cctype>

namespace sql {
    class Connection;
    class PreparedStatement;
    class ResultSet;
}

namespace revolif::db {

class ConnectionGuard;
class TransactionGuard;

class Database {
public:
    explicit Database(DatabaseConfig config);
    ~Database() noexcept;

    // Non-copyable, non-movable
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    // -------------------------------------------------------------------------
    // Connection management
    // -------------------------------------------------------------------------
    void connect();
    void disconnect() noexcept;
    bool isConnected() const noexcept;
    bool ping() const noexcept;
    bool testConnection();

    // -------------------------------------------------------------------------
    // Metadata & diagnostics
    // -------------------------------------------------------------------------
    std::string serverVersion();
    std::string currentDatabase();
    DatabaseHealth health() const;
    DatabaseStatistics statistics() const;

    // -------------------------------------------------------------------------
    // Query execution
    // -------------------------------------------------------------------------
    ResultSet executeQuery(const std::string& sqlText,
                           const std::vector<DBParam>& params = {});

    QueryResult executeUpdate(const std::string& sqlText,
                              const std::vector<DBParam>& params = {});

    template<typename T>
    T scalar(const std::string& sqlText,
             const std::vector<DBParam>& params = {});

    bool exists(const std::string& sqlText,
                const std::vector<DBParam>& params = {});

    // -------------------------------------------------------------------------
    // Transactions
    // -------------------------------------------------------------------------
    void beginTransaction();
    void commit();
    void rollback();
    bool inTransaction() const noexcept;

    // -------------------------------------------------------------------------
    // Utilities
    // -------------------------------------------------------------------------
    int64_t lastInsertId() const noexcept;
    QueryType detectQueryType(const std::string& sqlText) const noexcept;

private:
    // -------------------------------------------------------------------------
    // Internal constants
    // -------------------------------------------------------------------------
    static constexpr unsigned DEFAULT_PORT              = 3306;
    static constexpr unsigned DEFAULT_CONNECT_TIMEOUT   = 5;
    static constexpr unsigned DEFAULT_READ_TIMEOUT      = 30;
    static constexpr unsigned MAX_PREPARED_CACHE        = 100;
    static constexpr unsigned MAX_RECONNECT_ATTEMPTS    = 5;

    // -------------------------------------------------------------------------
    // Members
    // -------------------------------------------------------------------------
    DatabaseConfig config_;
    mutable std::recursive_mutex mutex_;

    std::unique_ptr<sql::Connection> connection_;
    ConnectionState state_ = ConnectionState::Disconnected;
    bool inTransaction_ = false;
    int64_t lastInsertId_ = 0;

    std::time_t connectedSince_ = 0;
    std::time_t lastReconnect_ = 0;

    DatabaseStatistics stats_;

    // LRU statement cache
    struct CacheEntry {
        std::unique_ptr<sql::PreparedStatement> stmt;
    };
    std::list<std::string> cacheOrder_;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap_;
    std::unordered_map<std::string, CacheEntry> statementCache_;

    // -------------------------------------------------------------------------
    // Internal helpers
    // -------------------------------------------------------------------------
    void ensureConnection();
    void reconnectWithRetry();

    sql::PreparedStatement& prepareStatement(const std::string& sqlText);
    void bindParameters(sql::PreparedStatement& stmt,
                        const std::vector<DBParam>& params);
    ResultSet readRows(sql::ResultSet& rs);

    void logEvent(LogLevel level, const std::string& message) const;
    void translateAndThrow(const sql::SQLException& e,
                           const std::string& sqlText) const;

    void clearStatementCache();
    void verifySchemaVersion();

    void setState(ConnectionState state) noexcept;
};

// ============================================================================
// Template implementation
// ============================================================================
template<typename T>
T Database::scalar(const std::string& sqlText,
                   const std::vector<DBParam>& params)
{
    auto rs = executeQuery(sqlText, params);
    if (rs.empty()) {
        throw DBQueryException("scalar query returned no rows",
                               0, "", sqlText, ErrorCategory::Query);
    }
    const auto& row = rs.at(0);
    const auto& opt = row.rawAt(0);
    if (!opt.has_value()) {
        throw DBQueryException("scalar query returned NULL",
                               0, "", sqlText, ErrorCategory::Query);
    }
    const std::string& val = *opt;

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
        return static_cast<T>(std::stoll(val));
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(val);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return val;
    } else if constexpr (std::is_same_v<T, bool>) {
        return (val == "1" || val == "true" || val == "TRUE");
    } else {
        static_assert(sizeof(T) == 0, "Unsupported scalar type");
    }
}

} // namespace revolif::db
