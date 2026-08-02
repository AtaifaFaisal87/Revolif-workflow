// =============================================================================
// ResultSet.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "ResultSet.h"

namespace revolif::db {

ResultSet::ResultSet(std::vector<Row> rows) : rows_(std::move(rows)) {}

const Row& ResultSet::at(size_t index) const {
    if (index >= rows_.size())
        throw std::out_of_range("ResultSet::at: index out of range");
    return rows_[index];
}

} // namespace revolif::db
