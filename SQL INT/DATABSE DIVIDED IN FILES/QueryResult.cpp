// =============================================================================
// QueryResult.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "QueryResult.h"

namespace revolif::db {

QueryResult::QueryResult(uint64_t affectedRows,
                         int64_t lastInsertId,
                         bool success,
                         unsigned warningCount)
    : affectedRows_(affectedRows),
      lastInsertId_(lastInsertId),
      success_(success),
      warningCount_(warningCount) {}

} // namespace revolif::db
