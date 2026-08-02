#pragma once

#include "DatabaseException.h"
#include <string>

namespace sql { class SQLException; }

namespace revolif::db {

class DatabaseErrorMapper {
public:
    static ErrorCategory mapErrorCode(int errorCode);
    static ErrorCategory mapSqlState(const std::string& sqlState);
    static std::string friendlyMessage(int errorCode,
                                       const std::string& sqlState,
                                       const std::string& originalMessage);
};

} // namespace revolif::db
