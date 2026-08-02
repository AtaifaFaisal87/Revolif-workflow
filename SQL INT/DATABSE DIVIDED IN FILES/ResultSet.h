#pragma once

#include "Row.h"
#include <vector>
#include <stdexcept>

namespace revolif::db {

class ResultSet {
public:
    explicit ResultSet(std::vector<Row> rows);

    const Row& at(size_t index) const;
    size_t rowCount() const noexcept { return rows_.size(); }
    bool empty() const noexcept { return rows_.empty(); }

private:
    std::vector<Row> rows_;
};

} // namespace revolif::db
