#pragma once

#include <stdexcept>
#include <string>

namespace revolif::db {

enum class ErrorCategory
{
    Connection,
    Query,
    ConstraintViolation,
    Transaction,
    Timeout,
    Unknown
};

class DatabaseException : public std::runtime_error {
public:
    DatabaseException(const std::string& message,
                      int mysqlErrorCode,
                      std::string sqlState,
                      std::string query,
                      ErrorCategory category);

    int mysqlErrorCode() const noexcept { return mysqlErrorCode_; }
    const std::string& sqlState() const noexcept { return sqlState_; }
    const std::string& query() const noexcept { return query_; }
    ErrorCategory category() const noexcept { return category_; }

private:
    int mysqlErrorCode_;
    std::string sqlState_;
    std::string query_;
    ErrorCategory category_;
};

class DBConnectionException : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

class DBQueryException : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

class DBConstraintViolationException : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

class DBTransactionException : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

class DBTimeoutException : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

} // namespace revolif::db
