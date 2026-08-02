#pragma once

#include <string>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <cctype>

namespace revolif::db {

class DatabaseConfig {
public:
    DatabaseConfig(std::string host,
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
                   unsigned schemaVersion = 0);

    void validate() const;
    std::string describe() const;

    static DatabaseConfig loadFromFile(const std::string& path);
    static DatabaseConfig loadFromEnvironment();

    const std::string& host() const noexcept { return host_; }
    unsigned port() const noexcept { return port_; }
    const std::string& username() const noexcept { return username_; }
    const std::string& password() const noexcept { return password_; }
    const std::string& schema() const noexcept { return schema_; }
    const std::string& charset() const noexcept { return charset_; }
    bool useSSL() const noexcept { return useSSL_; }
    unsigned connectTimeoutSec() const noexcept { return connectTimeoutSec_; }
    unsigned readTimeoutSec() const noexcept { return readTimeoutSec_; }
    unsigned reconnectAttempts() const noexcept { return reconnectAttempts_; }
    unsigned reconnectDelayMs() const noexcept { return reconnectDelayMs_; }
    bool loggingEnabled() const noexcept { return loggingEnabled_; }
    const std::string& logFilePath() const noexcept { return logFilePath_; }
    unsigned schemaVersion() const noexcept { return schemaVersion_; }

private:
    std::string host_;
    unsigned port_;
    std::string username_;
    std::string password_;
    std::string schema_;
    std::string charset_;
    bool useSSL_;
    unsigned connectTimeoutSec_;
    unsigned readTimeoutSec_;
    unsigned reconnectAttempts_;
    unsigned reconnectDelayMs_;
    bool loggingEnabled_;
    std::string logFilePath_;
    unsigned schemaVersion_;
};

} // namespace revolif::db
