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
// DatabaseConfig
// =============================================================================

DatabaseConfig::DatabaseConfig(std::string host,
                                unsigned port,
                                std::string username,
                                std::string password,
                                std::string schema,
                                std::string charset,
                                bool useSSL,
                                unsigned connectTimeoutSec,
                                unsigned readTimeoutSec,
                                unsigned reconnectAttempts,
                                unsigned reconnectDelayMs,
                                bool loggingEnabled,
                                std::string logFilePath)
    : host_(std::move(host)),
      port_(port),
      username_(std::move(username)),
      password_(std::move(password)),
      schema_(std::move(schema)),
      charset_(std::move(charset)),
      useSSL_(useSSL),
      connectTimeoutSec_(connectTimeoutSec),
      readTimeoutSec_(readTimeoutSec),
      reconnectAttempts_(reconnectAttempts),
      reconnectDelayMs_(reconnectDelayMs),
      loggingEnabled_(loggingEnabled),
      logFilePath_(std::move(logFilePath)) {
    validate();
}

void DatabaseConfig::validate() const {
    if (host_.empty())
        throw std::invalid_argument("DatabaseConfig: host must not be empty");
    if (port_ == 0 || port_ > 65535)
        throw std::invalid_argument("DatabaseConfig: port must be between 1 and 65535");
    if (username_.empty())
        throw std::invalid_argument("DatabaseConfig: username must not be empty");
    if (schema_.empty())
        throw std::invalid_argument("DatabaseConfig: schema must not be empty");
    if (charset_.empty())
        throw std::invalid_argument("DatabaseConfig: charset must not be empty");
    if (reconnectAttempts_ > 20)
        throw std::invalid_argument("DatabaseConfig: reconnectAttempts unreasonably large (>20)");
}

std::string DatabaseConfig::describe() const {
    std::ostringstream oss;
    oss << "DatabaseConfig{host=" << host_
        << ", port=" << port_
        << ", user=" << username_
        << ", password=***"
        << ", schema=" << schema_
        << ", charset=" << charset_
        << ", ssl=" << (useSSL_ ? "true" : "false")
        << ", connectTimeout=" << connectTimeoutSec_ << "s"
        << ", readTimeout=" << readTimeoutSec_ << "s"
        << ", reconnectAttempts=" << reconnectAttempts_
        << ", reconnectDelayMs=" << reconnectDelayMs_
        << "}";
    return oss.str();
}

namespace {
std::string trim(const std::string& s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

bool parseBool(const std::string& s, bool defaultValue) {
    std::string v = trim(s);
    for (auto& c : v) c = static_cast<char>(::tolower(c));
    if (v == "true" || v == "1" || v == "yes") return true;
    if (v == "false" || v == "0" || v == "no") return false;
    return defaultValue;
}

std::string envOr(const char* name, const std::string& fallback) {
    const char* v = std::getenv(name);
    return v ? std::string(v) : fallback;
}
} // namespace

DatabaseConfig DatabaseConfig::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("DatabaseConfig::loadFromFile: could not open '" + path + "'");

    std::unordered_map<std::string, std::string> kv;
    std::string line;
    while (std::getline(file, line)) {
        const std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        const auto eq = trimmed.find('=');
        if (eq == std::string::npos) continue;
        kv[trim(trimmed.substr(0, eq))] = trim(trimmed.substr(eq + 1));
    }

    auto get = [&](const std::string& key, const std::string& def) {
        auto it = kv.find(key);
        return it != kv.end() ? it->second : def;
    };

    return DatabaseConfig(
        get("host", "127.0.0.1"),
        static_cast<unsigned>(std::stoul(get("port", "3306"))),
        get("username", ""),
        get("password", ""),
        get("schema", ""),
        get("charset", "utf8mb4"),
        parseBool(get("ssl", "true"), true),
        static_cast<unsigned>(std::stoul(get("connect_timeout_sec", "5"))),
        static_cast<unsigned>(std::stoul(get("read_timeout_sec", "30"))),
        static_cast<unsigned>(std::stoul(get("reconnect_attempts", "3"))),
        static_cast<unsigned>(std::stoul(get("reconnect_delay_ms", "500"))),
        parseBool(get("logging_enabled", "false"), false),
        get("log_file_path", ""));
}

DatabaseConfig DatabaseConfig::loadFromEnvironment() {
    return DatabaseConfig(
        envOr("REVOLIF_DB_HOST", "127.0.0.1"),
        static_cast<unsigned>(std::stoul(envOr("REVOLIF_DB_PORT", "3306"))),
        envOr("REVOLIF_DB_USER", ""),
        envOr("REVOLIF_DB_PASSWORD", ""),
        envOr("REVOLIF_DB_SCHEMA", ""),
        envOr("REVOLIF_DB_CHARSET", "utf8mb4"),
        parseBool(envOr("REVOLIF_DB_SSL", "true"), true),
        static_cast<unsigned>(std::stoul(envOr("REVOLIF_DB_CONNECT_TIMEOUT_SEC", "5"))),
        static_cast<unsigned>(std::stoul(envOr("REVOLIF_DB_READ_TIMEOUT_SEC", "30"))),
        static_cast<unsigned>(std::stoul(envOr("REVOLIF_DB_RECONNECT_ATTEMPTS", "3"))),
        static_cast<unsigned>(std::stoul(envOr("REVOLIF_DB_RECONNECT_DELAY_MS", "500"))),
        parseBool(envOr("REVOLIF_DB_LOGGING_ENABLED", "false"), false),
        envOr("REVOLIF_DB_LOG_FILE_PATH", ""));
}

// =============================================================================
// DBParam
// =============================================================================

DBParam::DBParam() : type_(Type::Null), value_(std::monostate{}) {}
DBParam::DBParam(std::nullptr_t) : type_(Type::Null), value_(std::monostate{}) {}
DBParam::DBParam(int value) : type_(Type::Int64), value_(static_cast<int64_t>(value)) {}
DBParam::DBParam(int64_t value) : type_(Type::Int64), value_(value) {}
DBParam::DBParam(double value) : type_(Type::Double), value_(value) {}
DBParam::DBParam(bool value) : type_(Type::Bool), value_(value) {}
DBParam::DBParam(std::string value) : type_(Type::String), value_(std::move(value)) {}
DBParam::DBParam(const char* value) : type_(Type::String), value_(std::string(value)) {}

DBParam DBParam::dateTime(std::string isoDateTime) {
    DBParam p(std::move(isoDateTime));
    p.type_ = Type::DateTime;
    return p;
}

int64_t DBParam::asInt64() const {
    if (type_ != Type::Int64)
        throw std::logic_error("DBParam::asInt64: parameter is not an Int64");
    return std::get<int64_t>(value_);
}

double DBParam::asDouble() const {
    if (type_ != Type::Double)
        throw std::logic_error("DBParam::asDouble: parameter is not a Double");
    return std::get<double>(value_);
}

bool DBParam::asBool() const {
    if (type_ != Type::Bool)
        throw std::logic_error("DBParam::asBool: parameter is not a Bool");
    return std::get<bool>(value_);
}

const std::string& DBParam::asString() const {
    if (type_ != Type::String && type_ != Type::DateTime)
        throw std::logic_error("DBParam::asString: parameter is not a String/DateTime");
    return std::get<std::string>(value_);
}

std::string DBParam::debugString() const {
    if (sensitive_) return "***";
    switch (type_) {
        case Type::Null: return "NULL";
        case Type::Int64: return std::to_string(std::get<int64_t>(value_));
        case Type::Double: return std::to_string(std::get<double>(value_));
        case Type::Bool: return std::get<bool>(value_) ? "true" : "false";
        case Type::String: return "'" + std::get<std::string>(value_) + "'";
        case Type::DateTime: return "DATETIME('" + std::get<std::string>(value_) + "')";
    }
    return "?";
}

// =============================================================================
// DatabaseException hierarchy
// =============================================================================

DatabaseException::DatabaseException(const std::string& message,
                                      int mysqlErrorCode,
                                      std::string sqlState,
                                      std::string query)
    : std::runtime_error(message),
      mysqlErrorCode_(mysqlErrorCode),
      sqlState_(std::move(sqlState)),
      query_(std::move(query)) {}

// =============================================================================
// Row
// =============================================================================

Row::Row(std::vector<std::string> columnNames, std::vector<std::optional<std::string>> values)
    : columnNames_(std::move(columnNames)), values_(std::move(values)) {
    if (columnNames_.size() != values_.size())
        throw std::invalid_argument("Row: column name count does not match value count");
}

size_t Row::indexOf(const std::string& column) const {
    for (size_t i = 0; i < columnNames_.size(); ++i)
        if (columnNames_[i] == column) return i;
    throw std::out_of_range("Row: no such column '" + column + "'");
}

const std::optional<std::string>& Row::rawAt(size_t index) const {
    if (index >= values_.size())
        throw std::out_of_range("Row: column index out of range");
    return values_[index];
}

bool Row::isNull(const std::string& column) const { return isNull(indexOf(column)); }
bool Row::isNull(size_t index) const { return !rawAt(index).has_value(); }

std::string Row::getString(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getString: column '" + column + "' is NULL");
    return *v;
}

int64_t Row::getInt64(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getInt64: column '" + column + "' is NULL");
    return std::stoll(*v);
}

double Row::getDouble(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getDouble: column '" + column + "' is NULL");
    return std::stod(*v);
}

bool Row::getBool(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getBool: column '" + column + "' is NULL");
    return *v == "1" || *v == "true";
}

// =============================================================================
// ResultSet
// =============================================================================

ResultSet::ResultSet(std::vector<Row> rows) : rows_(std::move(rows)) {}

const Row& ResultSet::at(size_t index) const {
    if (index >= rows_.size())
        throw std::out_of_range("ResultSet::at: index out of range");
    return rows_[index];
}

// =============================================================================
// QueryResult
// =============================================================================

QueryResult::QueryResult(uint64_t affectedRows, int64_t lastInsertId, bool success, unsigned warningCount)
    : affectedRows_(affectedRows), lastInsertId_(lastInsertId), success_(success), warningCount_(warningCount) {}

// =============================================================================
// Database
// =============================================================================

Database::Database(DatabaseConfig config) : config_(std::move(config)) {}

Database::~Database() {
    // noexcept per the header contract: swallow anything disconnect() throws
    // so destruction never propagates an exception during unwind/shutdown.
    try {
        disconnect();
    } catch (...) {
        // Best-effort logging only; never let the destructor throw.
        try { logEvent("Exception suppressed during Database destruction"); } catch (...) {}
    }
}

void Database::logEvent(const std::string& message) const {
    if (!config_.loggingEnabled()) return;

    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream line;
    line << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] " << message;

    if (!config_.logFilePath().empty()) {
        std::ofstream out(config_.logFilePath(), std::ios::app);
        if (out.is_open()) out << line.str() << '\n';
    } else {
        std::cerr << line.str() << '\n';
    }
}

void Database::clearStatementCache() {
    // Prepared statements are tied to a specific sql::Connection; once the
    // connection is dropped/replaced, cached statements are dangling and
    // must not be reused.
    statementCache_.clear();
}

void Database::connect() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    try {
        sql::Driver* driver = get_driver_instance();

        sql::ConnectOptionsMap options;
        options["hostName"] = config_.host();
        options["port"] = static_cast<int>(config_.port());
        options["userName"] = config_.username();
        options["password"] = config_.password();
        options["OPT_CONNECT_TIMEOUT"] = static_cast<int>(config_.connectTimeoutSec());
        options["OPT_READ_TIMEOUT"] = static_cast<int>(config_.readTimeoutSec());
        options["OPT_RECONNECT"] = false; // we manage reconnection ourselves
        if (config_.useSSL()) {
            options["sslEnforce"] = true;
        } else {
            options["sslMode"] = sql::SSL_MODE_DISABLED;
        }

        connection_.reset(driver->connect(options));
        connection_->setSchema(config_.schema());

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        stmt->execute("SET NAMES '" + config_.charset() + "'");

        clearStatementCache();
        connected_ = true;
        inTransaction_ = false;
        logEvent("Connected: " + config_.describe());
    } catch (const sql::SQLException& e) {
        connected_ = false;
        translateAndThrow(e, "[connect]");
    }
}

void Database::disconnect() noexcept {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    try {
        clearStatementCache();
        if (connection_ && !connection_->isClosed()) {
            connection_->close();
        }
    } catch (...) {
        // Ignore errors while closing; there is nothing more we can do.
    }
    connection_.reset();
    connected_ = false;
    inTransaction_ = false;
}

bool Database::isConnected() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!connected_ || !connection_) return false;
    try {
        return !connection_->isClosed();
    } catch (...) {
        return false;
    }
}

bool Database::ping() noexcept {
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
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT 1"));
        return rs->next();
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, "[testConnection]");
    }
}

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

void Database::ensureConnection() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (isConnected() && ping()) return;
    reconnectWithRetry();
}

void Database::reconnectWithRetry() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    const bool wasInTransaction = inTransaction_;
    if (wasInTransaction) {
        // A dropped connection implicitly aborts any open transaction.
        // We cannot recover it - surface that clearly instead of silently
        // reconnecting as if nothing happened.
        inTransaction_ = false;
        logEvent("Connection lost while a transaction was open; transaction aborted");
    }

    std::exception_ptr lastError;
    const unsigned attempts = config_.reconnectAttempts();

    for (unsigned attempt = 1; attempt <= attempts; ++attempt) {
        try {
            connect();
            logEvent("Reconnected successfully on attempt " + std::to_string(attempt));
            if (wasInTransaction) {
                throw DBTransactionException(
                    "Connection was lost and has been re-established, but the "
                    "in-progress transaction could not be recovered and was rolled "
                    "back by the server. The caller must retry the transaction.");
            }
            return;
        } catch (const DBTransactionException&) {
            throw; // propagate immediately; this is not a connect failure
        } catch (...) {
            lastError = std::current_exception();
            if (attempt < attempts) {
                std::this_thread::sleep_for(std::chrono::milliseconds(config_.reconnectDelayMs()));
            }
        }
    }

    logEvent("Failed to reconnect after " + std::to_string(attempts) + " attempt(s)");
    if (lastError) std::rethrow_exception(lastError);
    throw DBConnectionException("Failed to (re)connect to database", 0, "", "[reconnect]");
}

sql::PreparedStatement& Database::prepareStatement(const std::string& sqlText) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto it = statementCache_.find(sqlText);
    if (it != statementCache_.end()) return *it->second;

    std::unique_ptr<sql::PreparedStatement> stmt(connection_->prepareStatement(sqlText));
    auto* raw = stmt.get();
    statementCache_.emplace(sqlText, std::move(stmt));
    return *raw;
}

void Database::bindParameters(sql::PreparedStatement& stmt, const std::vector<DBParam>& params) {
    for (size_t i = 0; i < params.size(); ++i) {
        const unsigned index = static_cast<unsigned>(i + 1); // JDBC-style, 1-based
        const DBParam& p = params[i];
        switch (p.type()) {
            case DBParam::Type::Null:
                stmt.setNull(index, sql::DataType::VARCHAR);
                break;
            case DBParam::Type::Int64:
                stmt.setInt64(index, p.asInt64());
                break;
            case DBParam::Type::Double:
                stmt.setDouble(index, p.asDouble());
                break;
            case DBParam::Type::Bool:
                stmt.setBoolean(index, p.asBool());
                break;
            case DBParam::Type::String:
            case DBParam::Type::DateTime:
                stmt.setString(index, p.asString());
                break;
        }
    }
}

ResultSet Database::executeQuery(const std::string& sqlText, const std::vector<DBParam>& params) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ensureConnection();

    try {
        sql::PreparedStatement& stmt = prepareStatement(sqlText);
        bindParameters(stmt, params);

        std::unique_ptr<sql::ResultSet> rs(stmt.executeQuery());

        sql::ResultSetMetaData* meta = rs->getMetaData();
        const unsigned columnCount = meta->getColumnCount();
        std::vector<std::string> columnNames;
        columnNames.reserve(columnCount);
        for (unsigned c = 1; c <= columnCount; ++c) {
            columnNames.push_back(meta->getColumnLabel(c));
        }

        std::vector<Row> rows;
        while (rs->next()) {
            std::vector<std::optional<std::string>> values;
            values.reserve(columnCount);
            for (unsigned c = 1; c <= columnCount; ++c) {
                if (rs->isNull(c)) {
                    values.emplace_back(std::nullopt);
                } else {
                    values.emplace_back(rs->getString(c));
                }
            }
            rows.emplace_back(columnNames, std::move(values));
        }

        return ResultSet(std::move(rows));
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, sqlText);
    }
}

QueryResult Database::executeUpdate(const std::string& sqlText, const std::vector<DBParam>& params) {
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
            // No AUTO_INCREMENT column on this statement; not an error.
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

        return QueryResult(static_cast<uint64_t>(affected), insertId, true, warnings);
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, sqlText);
    }
}

void Database::beginTransaction() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ensureConnection();
    if (inTransaction_) {
        throw DBTransactionException("beginTransaction: a transaction is already in progress "
                                      "(nested transactions are not supported)");
    }
    try {
        connection_->setAutoCommit(false);
        inTransaction_ = true;
        logEvent("Transaction started");
    } catch (const sql::SQLException& e) {
        translateAndThrow(e, "[beginTransaction]");
    }
}

void Database::commit() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!inTransaction_) {
        throw DBTransactionException("commit: no transaction is in progress");
    }
    try {
        connection_->commit();
        connection_->setAutoCommit(true);
        inTransaction_ = false;
        logEvent("Transaction committed");
    } catch (const sql::SQLException& e) {
        inTransaction_ = false;
        translateAndThrow(e, "[commit]");
    }
}

void Database::rollback() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!inTransaction_) {
        throw DBTransactionException("rollback: no transaction is in progress");
    }
    try {
        connection_->rollback();
        connection_->setAutoCommit(true);
        inTransaction_ = false;
        logEvent("Transaction rolled back");
    } catch (const sql::SQLException& e) {
        inTransaction_ = false;
        translateAndThrow(e, "[rollback]");
    }
}

void Database::translateAndThrow(const std::exception& e, const std::string& sqlText) const {
    const auto* sqlEx = dynamic_cast<const sql::SQLException*>(&e);
    const std::string message = e.what();
    const int errorCode = sqlEx ? sqlEx->getErrorCode() : 0;
    const std::string sqlState = sqlEx ? sqlEx->getSQLState() : "";

    logEvent("Database error [" + std::to_string(errorCode) + "/" + sqlState + "] on " +
              sqlText + ": " + message);

    // Connection-level failures.
    // 2002/2003: can't connect; 2006/2013: server gone away / lost connection;
    // 1045: access denied; 1049: unknown database.
    switch (errorCode) {
        case 2002: case 2003: case 2006: case 2013: case 1045: case 1049:
            throw DBConnectionException(message, errorCode, sqlState, sqlText);
        case 1205: // ER_LOCK_WAIT_TIMEOUT
            throw DBTimeoutException(message, errorCode, sqlState, sqlText);
        case 1062: // ER_DUP_ENTRY
        case 1216: case 1217: // FK constraint fails (old error set)
        case 1451: case 1452: // FK constraint fails (row is / would be referenced)
        case 3819:            // CHECK constraint violation
            throw DBConstraintViolationException(message, errorCode, sqlState, sqlText);
        default:
            break;
    }

    // Fall back to SQLSTATE class when the specific vendor code above
    // did not match (covers driver/version differences).
    if (sqlState.size() >= 2) {
        const std::string cls = sqlState.substr(0, 2);
        if (cls == "08") throw DBConnectionException(message, errorCode, sqlState, sqlText);
        if (cls == "23") throw DBConstraintViolationException(message, errorCode, sqlState, sqlText);
        if (cls == "40") throw DBTransactionException(message, errorCode, sqlState, sqlText);
    }

    throw DBQueryException(message, errorCode, sqlState, sqlText);
}

// =============================================================================
// ConnectionGuard
// =============================================================================

ConnectionGuard::ConnectionGuard(Database& db) : db_(db) {
    db_.ensureConnection();
}

// =============================================================================
// TransactionGuard
// =============================================================================

TransactionGuard::TransactionGuard(Database& db) : db_(db), active_(false), committed_(false) {
    db_.beginTransaction();
    active_ = true;
}

void TransactionGuard::commit() {
    if (!active_ || committed_) {
        throw DBTransactionException("TransactionGuard::commit: no active, uncommitted transaction");
    }
    db_.commit();
    committed_ = true;
    active_ = false;
}

TransactionGuard::~TransactionGuard() {
    if (active_ && !committed_) {
        try {
            db_.rollback();
        } catch (...) {
            // Destructor must not throw; the connection layer already logged
            // the underlying error via translateAndThrow/logEvent.
        }
    }
}

} // namespace revolif::db
