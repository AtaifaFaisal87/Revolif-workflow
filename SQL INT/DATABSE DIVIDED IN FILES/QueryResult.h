#pragma once

#include <cstdint>

namespace revolif::db {

class QueryResult {
public:
    QueryResult(uint64_t affectedRows, int64_t lastInsertId, bool success, unsigned warningCount);

    uint64_t affectedRows() const noexcept { return affectedRows_; }
    int64_t lastInsertId() const noexcept { return lastInsertId_; }
    bool success() const noexcept { return success_; }
    unsigned warningCount() const noexcept { return warningCount_; }

private:
    uint64_t affectedRows_;
    int64_t lastInsertId_;
    bool success_;
    unsigned warningCount_;
};

} // namespace revolif::db
