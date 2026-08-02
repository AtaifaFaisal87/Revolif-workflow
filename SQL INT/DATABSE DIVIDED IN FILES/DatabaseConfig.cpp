// =============================================================================
// DatabaseConfig.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "DatabaseConfig.h"

namespace revolif::db {

namespace {
    std::string trim(const std::string& s) {
        const auto first = s.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        const auto last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, last - first + 1);
    }

    bool parseBool(const std::string& s, bool defaultValue) {
        std::string v = trim(s);
        for (auto& c : v) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
        if (v == "true" || v == "1" || v == "yes") return true;
        if (v == "false" || v == "0" || v == "no") return false;
        return defaultValue;
    }

    std::string envOr(const char* name, const std::string& fallback) {
        const char* v = std::getenv(name);
        return v ? std::string(v) : fallback;
    }
} // namespace

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
                               std::string logFilePath,
                               unsigned schemaVersion)
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
      logFilePath_(std::move(logFilePath)),
      schemaVersion_(schemaVersion)
{
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
        << ", schemaVersion=" << schemaVersion_
        << "}";
    return oss.str();
}

DatabaseConfig DatabaseConfig::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("DatabaseConfig::loadFromFile: could not open '"" + path + """);

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
        get("log_file_path", ""),
        static_cast<unsigned>(std::stoul(get("schema_version", "0"))));
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
        envOr("REVOLIF_DB_LOG_FILE_PATH", ""),
        static_cast<unsigned>(std::stoul(envOr("REVOLIF_DB_SCHEMA_VERSION", "0"))));
}

} // namespace revolif::db
