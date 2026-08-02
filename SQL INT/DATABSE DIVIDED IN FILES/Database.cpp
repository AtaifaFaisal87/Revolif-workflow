// =============================================================================
// Database.cpp
// REVOLIF - Database Access Layer implementation
//
// This is the ONLY translation unit in REVOLIF that includes MySQL
// Connector/C++ headers. Everything above this layer (Repositories,
// Managers, System, Authentication) talks only to the types declared in
// Database.h.
// =============================================================================

#include "Database.h"

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <ctime>

namespace revolif::db {

// =============================================================================
// Construction / Destruction
// =============================================================================

Database::Database(DatabaseConfig config) : config_(std::move(config)) {}

Database::~Database() noexcept {
    try {
        disconnect();
    } catch (...) {
        try {
            logEvent(LogLevel::Error,
                     "Exception suppressed during Database destruction");
        } catch (...) {}
    }
}

// =============================================================================
// Connection management
// =============================================================================

void Database::connect() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    try {
        setState(ConnectionState::Connecting);

        sql::Driver* driver = get_driver_instance();

        sql::ConnectOptionsMap options;
        options["hostName"] = config_.host();
        options["port"]     = static_cast<int>(config_.port());
        options["userName"] = config_.username();
        options["password"] = config_.password();
        options["OPT_CONNECT_TIMEOUT"] = static_cast<int>(config_.connectTimeoutSec());
        options["OPT_READ_TIMEOUT"]      = static_cast<int>(config_.readTimeoutSec());
        options["OPT_RECONNECT"] = false; // we manage reconnection ourselves

        if (config_.useSSL()) {
            options["sslEnforce"] = true;
        } else {
            options["sslMode"] = sql::SSL_MODE_DISABLED;
        }

        connection_.reset(driver->connect(options));
        connection_->setSchema(config_.schema());

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        stmt->execute("SET NAMES '"" + config_.charset() + """);

        clearStatementCache();
        setState(ConnectionState::Connected);
        inTransaction_ = false;
        connectedSince_ = std::time(nullptr);
        ++stats_.successfulConnections;

        logEvent(LogLevel::Info, "Connected: " + config_.describe());
    } catch (const sql::SQLException& e) {
        setState(ConnectionState::Disconnected);
        ++stats_.failedConnections;
        translateAndThrow(e, "[connect]");
    }

    verifySchemaVersion();
}

void Database::disconnect() noexcept {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    setState(ConnectionState::Disconnecting);

    try {
        if (inTransaction_) {
            logEvent(LogLevel::Warning,
                     "Rolling back active transaction during disconnect");
            try { connection_->rollback(); } catch (...) {}
            inTransaction_ = false;
        }
    } catch (...) {}

    clearStatementCache();

    try {
        if (connection_ && !connection_->isClosed()) {
            connection_->close();
        }
    } catch (...) {
        // Ignore errors while closing; there is nothing more we can do.
    }

    connection_.reset();
    setState(ConnectionState::Disconnected);
    logEvent(LogLevel::Info, "Disconnected");
}

bool Database::isConnected() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (state_ != ConnectionState::Connected || !connection_) return false;
    try {
        return !connection_->isClosed();
    } catch (...) {
        return false;
    }
}

bool Database::ping() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    try {
        if (!connection_) return false;
        return connection_->isValid();
    } catch (...) {
        return false;
    }
}

bool Database::testConnection() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    try {
        ensureConnection();
        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(SQL::Ping));
        return rs->next();
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, SQL::Ping);
    }
}

// =============================================================================
// Metadata & diagnostics
// =============================================================================

std::string Database::serverVersion() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ensureConnection();
    try {
        return connection_->getMetaData()->getDatabaseProductVersion();
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, "[serverVersion]");
    }
}

std::string Database::currentDatabase() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ensureConnection();
    try {
        return connection_->getSchema();
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, "[currentDatabase]");
    }
}

DatabaseHealth Database::health() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    DatabaseHealth h;
    h.connected       = (state_ == ConnectionState::Connected);
    h.transactionActive = inTransaction_;
    h.charset         = config_.charset();

    if (connection_ && state_ == ConnectionState::Connected) {
        try {
            h.serverVersion = connection_->getMetaData()->getDatabaseProductVersion();
            h.databaseName  = connection_->getSchema();
        } catch (...) {
            // Best-effort only
        }
    }

    auto start = std::chrono::steady_clock::now();
    bool pingOk = const_cast<Database*>(this)->ping();
    auto end   = std::chrono::steady_clock::now();
    h.ping = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (!pingOk) {
        h.connected = false;
    }

    h.connectedSince = connectedSince_;
    h.lastReconnect  = lastReconnect_;
    return h;
}

DatabaseStatistics Database::statistics() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return stats_;
}

// =============================================================================
// Query execution
// =============================================================================

ResultSet Database::executeQuery(const std::string& sqlText,
                                 const std::vector<DBParam>& params)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ensureConnection();

    try {
        sql::PreparedStatement& stmt = prepareStatement(sqlText);
        bindParameters(stmt, params);

        std::unique_ptr<sql::ResultSet> rs(stmt.executeQuery());
        ResultSet result = readRows(*rs);

        ++stats_.executedQueries;
        logEvent(LogLevel::Debug,
                 "Executed query [" + std::to_string(static_cast<int>(detectQueryType(sqlText))) +
                 "]: " + sqlText);
        return result;
    } catch (const sql::SQLException& e) {
        ++stats_.failedQueries;
        translateAndThrow(e, sqlText);
    }
}

QueryResult Database::executeUpdate(const std::string& sqlText,
                                    const std::vector<DBParam>& params)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ensureConnection();

    try {
        sql::PreparedStatement& stmt = prepareStatement(sqlText);
        bindParameters(stmt, params);

        const int affected = stmt.executeUpdate();

        int64_t insertId = 0;
        try {
            std::unique_ptr<sql::ResultSet> keys(stmt.getGeneratedKeys());
            if (keys && keys->next()) {
                insertId = keys->getInt64(1);
            }
        } catch (const sql::SQLException&) {
            insertId = 0;
        }
        if (insertId != 0) lastInsertId_ = insertId;

        unsigned warnings = 0;
        try {
            const sql::SQLWarning* w = stmt.getWarnings();
            while (w) { ++warnings; w = w->getNextWarning(); }
        } catch (...) {
            warnings = 0;
        }

        ++stats_.executedQueries;
        logEvent(LogLevel::Debug,
                 "Executed update [" + std::to_string(static_cast<int>(detectQueryType(sqlText))) +
                 "]: " + sqlText);
        return QueryResult(static_cast<uint64_t>(affected), insertId, true, warnings);
    } catch (const sql::SQLException& e) {
        ++stats_.failedQueries;
        translateAndThrow(e, sqlText);
    }
}

bool Database::exists(const std::string& sqlText,
                      const std::vector<DBParam>& params)
{
    auto rs = executeQuery(sqlText, params);
    return !rs.empty();
}

// =============================================================================
// Transactions
// =============================================================================

void Database::beginTransaction() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ensureConnection();
    if (inTransaction_) {
        throw DBTransactionException(
            "beginTransaction: a transaction is already in progress",
            0, "", SQL::Begin, ErrorCategory::Transaction);
    }
    try {
        connection_->setAutoCommit(false);
        inTransaction_ = true;
        ++stats_.transactionsStarted;
        logEvent(LogLevel::Info, "Transaction started");
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, SQL::Begin);
    }
}

void Database::commit() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!inTransaction_) {
        throw DBTransactionException(
            "commit: no transaction is in progress",
            0, "", SQL::Commit, ErrorCategory::Transaction);
    }
    try {
        connection_->commit();
        connection_->setAutoCommit(true);
        inTransaction_ = false;
        ++stats_.committedTransactions;
        logEvent(LogLevel::Info, "Transaction committed");
    } catch (const sql::SQLException& e) {
        inTransaction_ = false;
        translateAndThrow(e, SQL::Commit);
    }
}

void Database::rollback() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!inTransaction_) {
        throw DBTransactionException(
            "rollback: no transaction is in progress",
            0, "", SQL::Rollback, ErrorCategory::Transaction);
    }
    try {
        connection_->rollback();
        connection_->setAutoCommit(true);
        inTransaction_ = false;
        ++stats_.rolledBackTransactions;
        logEvent(LogLevel::Info, "Transaction rolled back");
    } catch (const sql::SQLException& e) {
        inTransaction_ = false;
        translateAndThrow(e, SQL::Rollback);
    }
}

bool Database::inTransaction() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return inTransaction_;
}

int64_t Database::lastInsertId() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return lastInsertId_;
}

// =============================================================================
// Query type detection
// =============================================================================

QueryType Database::detectQueryType(const std::string& sqlText) const noexcept {
    std::string upper;
    upper.reserve(sqlText.size());
    for (char c : sqlText) {
        upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }

    size_t start = 0;
    while (start < upper.size() &&
           std::isspace(static_cast<unsigned char>(upper[start]))) {
        ++start;
    }

    if (start >= upper.size()) return QueryType::Other;

    const auto& s = upper.substr(start);
    if (s.rfind("SELECT", 0) == 0)     return QueryType::Select;
    if (s.rfind("INSERT", 0) == 0)     return QueryType::Insert;
    if (s.rfind("UPDATE", 0) == 0)     return QueryType::Update;
    if (s.rfind("DELETE", 0) == 0)     return QueryType::Delete;
    if (s.rfind("REPLACE", 0) == 0)    return QueryType::Replace;
    if (s.rfind("CREATE", 0) == 0)     return QueryType::Create;
    if (s.rfind("DROP", 0) == 0)       return QueryType::Drop;
    if (s.rfind("ALTER", 0) == 0)      return QueryType::Alter;
    if (s.rfind("START", 0) == 0 ||
        s.rfind("BEGIN", 0) == 0 ||
        s.rfind("COMMIT", 0) == 0 ||
        s.rfind("ROLLBACK", 0) == 0 ||
        s.rfind("SAVEPOINT", 0) == 0)  return QueryType::Transaction;

    return QueryType::Other;
}

// =============================================================================
// Internal helpers
// =============================================================================

void Database::ensureConnection() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (isConnected() && ping()) return;
    reconnectWithRetry();
}

void Database::reconnectWithRetry() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    const bool wasInTransaction = inTransaction_;
    if (wasInTransaction) {
        inTransaction_ = false;
        logEvent(LogLevel::Warning,
                 "Connection lost while a transaction was open; transaction aborted");
    }

    std::exception_ptr lastError;
    const unsigned attempts = config_.reconnectAttempts();

    for (unsigned attempt = 1; attempt <= attempts; ++attempt) {
        try {
            connect();
            ++stats_.reconnects;
            lastReconnect_ = std::time(nullptr);
            logEvent(LogLevel::Info,
                     "Reconnected successfully on attempt " + std::to_string(attempt));
            if (wasInTransaction) {
                throw DBTransactionException(
                    "Connection was lost and has been re-established, but the "
                    "in-progress transaction could not be recovered and was rolled "
                    "back by the server. The caller must retry the transaction.",
                    0, "", "[reconnect]", ErrorCategory::Transaction);
            }
            return;
        } catch (const DBTransactionException&) {
            throw;
        } catch (...) {
            lastError = std::current_exception();
            if (attempt < attempts) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(config_.reconnectDelayMs()));
            }
        }
    }

    logEvent(LogLevel::Error,
             "Failed to reconnect after " + std::to_string(attempts) + " attempt(s)");
    if (lastError) std::rethrow_exception(lastError);
    throw DBConnectionException("Failed to (re)connect to database",
                                 0, "", "[reconnect]", ErrorCategory::Connection);
}

sql::PreparedStatement& Database::prepareStatement(const std::string& sqlText) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    auto it = cacheMap_.find(sqlText);
    if (it != cacheMap_.end()) {
        // Move to front (most recently used)
        cacheOrder_.splice(cacheOrder_.begin(), cacheOrder_, it->second);
        return *statementCache_[sqlText].stmt;
    }

    // Evict LRU entries if necessary
    while (cacheOrder_.size() >= MAX_PREPARED_CACHE) {
        const std::string& lru = cacheOrder_.back();
        statementCache_.erase(lru);
        cacheMap_.erase(lru);
        cacheOrder_.pop_back();
    }

    std::unique_ptr<sql::PreparedStatement> stmt(connection_->prepareStatement(sqlText));
    auto* raw = stmt.get();

    cacheOrder_.push_front(sqlText);
    cacheMap_[sqlText] = cacheOrder_.begin();
    statementCache_.emplace(sqlText, CacheEntry{std::move(stmt)});
    return *raw;
}

void Database::bindParameters(sql::PreparedStatement& stmt,
                              const std::vector<DBParam>& params)
{
    for (size_t i = 0; i < params.size(); ++i) {
        const unsigned index = static_cast<unsigned>(i + 1); // JDBC-style, 1-based
        const DBParam& p = params[i];
        switch (p.type()) {
            case ParamType::Null:
                stmt.setNull(index, sql::DataType::VARCHAR);
                break;
            case ParamType::Int64:
                stmt.setInt64(index, p.asInt64());
                break;
            case ParamType::Double:
                stmt.setDouble(index, p.asDouble());
                break;
            case ParamType::Bool:
                stmt.setBoolean(index, p.asBool());
                break;
            case ParamType::String:
            case ParamType::DateTime:
                stmt.setString(index, p.asString());
                break;
        }
    }
}

ResultSet Database::readRows(sql::ResultSet& rs) {
    sql::ResultSetMetaData* meta = rs.getMetaData();
    const unsigned columnCount = meta->getColumnCount();

    std::vector<std::string> columnNames;
    columnNames.reserve(columnCount);
    for (unsigned c = 1; c <= columnCount; ++c) {
        columnNames.push_back(meta->getColumnLabel(c));
    }

    std::vector<Row> rows;
    while (rs.next()) {
        std::vector<std::optional<std::string>> values;
        values.reserve(columnCount);
        for (unsigned c = 1; c <= columnCount; ++c) {
            if (rs.isNull(c)) {
                values.emplace_back(std::nullopt);
            } else {
                values.emplace_back(rs.getString(c));
            }
        }
        rows.emplace_back(columnNames, std::move(values));
    }

    return ResultSet(std::move(rows));
}

// =============================================================================
// Logging
// =============================================================================

void Database::logEvent(LogLevel level, const std::string& message) const {
    if (!config_.loggingEnabled()) return;

    const auto now = std::chrono::system_clock::now();
    const auto now_time_t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream line;
    line << "[" << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S") << "] ";

    switch (level) {
        case LogLevel::Debug:   line << "[DEBUG]";   break;
        case LogLevel::Info:    line << "[INFO]";    break;
        case LogLevel::Warning: line << "[WARNING]"; break;
        case LogLevel::Error:   line << "[ERROR]";   break;
        case LogLevel::Fatal:   line << "[FATAL]";   break;
    }

    line << " [Thread " << std::this_thread::get_id() << "] ";
    line << "[Database] " << message;

    if (!config_.logFilePath().empty()) {
        std::ofstream out(config_.logFilePath(), std::ios::app);
        if (out.is_open()) out << line.str() << '\n';
    } else {
        std::cerr << line.str() << '\n';
    }
}

// =============================================================================
// Error translation
// =============================================================================

void Database::translateAndThrow(const sql::SQLException& e,
                                   const std::string& sqlText) const
{
    const std::string message  = e.what();
    const int errorCode        = e.getErrorCode();
    const std::string sqlState = e.getSQLState();

    logEvent(LogLevel::Error,
             "Database error [" + std::to_string(errorCode) + "/" + sqlState +
             "] on " + sqlText + ": " + message);

    ErrorCategory category = DatabaseErrorMapper::mapErrorCode(errorCode);
    if (category == ErrorCategory::Unknown) {
        category = DatabaseErrorMapper::mapSqlState(sqlState);
    }

    switch (category) {
        case ErrorCategory::Connection:
            throw DBConnectionException(message, errorCode, sqlState, sqlText, category);
        case ErrorCategory::Timeout:
            throw DBTimeoutException(message, errorCode, sqlState, sqlText, category);
        case ErrorCategory::ConstraintViolation:
            throw DBConstraintViolationException(message, errorCode, sqlState, sqlText, category);
        case ErrorCategory::Transaction:
            throw DBTransactionException(message, errorCode, sqlState, sqlText, category);
        default:
            throw DBQueryException(message, errorCode, sqlState, sqlText, category);
    }
}

// =============================================================================
// Cache & state management
// =============================================================================

void Database::clearStatementCache() {
    statementCache_.clear();
    cacheMap_.clear();
    cacheOrder_.clear();
}

void Database::setState(ConnectionState state) noexcept {
    state_ = state;
}

// =============================================================================
// Schema version verification
// =============================================================================

void Database::verifySchemaVersion() {
    if (config_.schemaVersion() == 0) return;

    try {
        bool hasTable = exists(
            "SELECT 1 FROM information_schema.tables "
            "WHERE table_schema = ? AND table_name = 'schema_version'",
            {config_.schema()});

        if (!hasTable) {
            throw DBConnectionException(
                "Schema version table not found",
                0, "", SQL::SchemaVersion, ErrorCategory::Connection);
        }

        auto rs = executeQuery(SQL::SchemaVersion);
        if (rs.empty()) {
            throw DBConnectionException(
                "Schema version table exists but has no rows",
                0, "", SQL::SchemaVersion, ErrorCategory::Connection);
        }

        unsigned actualVersion = static_cast<unsigned>(rs.at(0).getInt64("version"));
        if (actualVersion != config_.schemaVersion()) {
            throw DBConnectionException(
                "Schema version mismatch: expected " + std::to_string(config_.schemaVersion()) +
                ", found " + std::to_string(actualVersion),
                0, "", SQL::SchemaVersion, ErrorCategory::Connection);
        }

        logEvent(LogLevel::Info,
                 "Schema version verified: " + std::to_string(actualVersion));
    } catch (const DatabaseException&) {
        throw;
    } catch (const std::exception& e) {
        throw DBConnectionException(
            std::string("Failed to verify schema version: ") + e.what(),
            0, "", SQL::SchemaVersion, ErrorCategory::Connection);
    }
}

} // namespace revolif::db
