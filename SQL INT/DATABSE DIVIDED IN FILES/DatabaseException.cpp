// =============================================================================
// DatabaseException.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "DatabaseException.h"

namespace revolif::db {

DatabaseException::DatabaseException(const std::string& message,
                                       int mysqlErrorCode,
                                       std::string sqlState,
                                       std::string query,
                                       ErrorCategory category)
    : std::runtime_error(message),
      mysqlErrorCode_(mysqlErrorCode),
      sqlState_(std::move(sqlState)),
      query_(std::move(query)),
      category_(category) {}

} // namespace revolif::db
