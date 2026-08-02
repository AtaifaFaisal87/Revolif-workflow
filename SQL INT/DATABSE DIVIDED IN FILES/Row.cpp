// =============================================================================
// Row.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "Row.h"

namespace revolif::db {

Row::Row(std::vector<std::string> columnNames, std::vector<std::optional<std::string>> values)
    : columnNames_(std::move(columnNames)), values_(std::move(values)) {
    if (columnNames_.size() != values_.size())
        throw std::invalid_argument("Row: column name count does not match value count");
}

size_t Row::indexOf(const std::string& column) const {
    for (size_t i = 0; i < columnNames_.size(); ++i)
        if (columnNames_[i] == column) return i;
    throw std::out_of_range("Row: no such column '"" + column + """);
}

const std::optional<std::string>& Row::rawAt(size_t index) const {
    if (index >= values_.size())
        throw std::out_of_range("Row: column index out of range");
    return values_[index];
}

bool Row::isNull(const std::string& column) const { return isNull(indexOf(column)); }
bool Row::isNull(size_t index) const { return !rawAt(index).has_value(); }

std::string Row::getString(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getString: column '"" + column + "" is NULL");
    return *v;
}

int64_t Row::getInt64(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getInt64: column '"" + column + "" is NULL");
    return std::stoll(*v);
}

double Row::getDouble(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getDouble: column '"" + column + "" is NULL");
    return std::stod(*v);
}

bool Row::getBool(const std::string& column) const {
    const auto& v = rawAt(indexOf(column));
    if (!v.has_value())
        throw std::runtime_error("Row::getBool: column '"" + column + "" is NULL");
    return *v == "1" || *v == "true";
}

} // namespace revolif::db
