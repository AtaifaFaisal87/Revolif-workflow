// =============================================================================
// DatabaseErrorMapper.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "DatabaseErrorMapper.h"
#include <cppconn/exception.h>

namespace revolif::db {

ErrorCategory DatabaseErrorMapper::mapErrorCode(int errorCode) {
    switch (errorCode) {
        case 2002: case 2003: case 2006: case 2013: case 1045: case 1049:
            return ErrorCategory::Connection;
        case 1205: // ER_LOCK_WAIT_TIMEOUT
            return ErrorCategory::Timeout;
        case 1062: // ER_DUP_ENTRY
        case 1216: case 1217: // FK constraint fails (old error set)
        case 1451: case 1452: // FK constraint fails (row is / would be referenced)
        case 3819:            // CHECK constraint violation
            return ErrorCategory::ConstraintViolation;
        default:
            return ErrorCategory::Unknown;
    }
}

ErrorCategory DatabaseErrorMapper::mapSqlState(const std::string& sqlState) {
    if (sqlState.size() < 2) return ErrorCategory::Unknown;
    const std::string cls = sqlState.substr(0, 2);
    if (cls == "08") return ErrorCategory::Connection;
    if (cls == "23") return ErrorCategory::ConstraintViolation;
    if (cls == "40") return ErrorCategory::Transaction;
    return ErrorCategory::Unknown;
}

std::string DatabaseErrorMapper::friendlyMessage(int errorCode,
                                                 const std::string& sqlState,
                                                 const std::string& originalMessage) {
    (void)errorCode;
    (void)sqlState;
    return originalMessage;
}

} // namespace revolif::db
