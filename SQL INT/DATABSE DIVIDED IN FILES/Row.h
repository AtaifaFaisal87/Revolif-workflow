#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <stdexcept>

namespace revolif::db {

class Row {
public:
    Row(std::vector<std::string> columnNames, std::vector<std::optional<std::string>> values);

    size_t indexOf(const std::string& column) const;
    const std::optional<std::string>& rawAt(size_t index) const;

    bool isNull(const std::string& column) const;
    bool isNull(size_t index) const;

    std::string getString(const std::string& column) const;
    int64_t getInt64(const std::string& column) const;
    double getDouble(const std::string& column) const;
    bool getBool(const std::string& column) const;

    size_t columnCount() const noexcept { return columnNames_.size(); }

private:
    std::vector<std::string> columnNames_;
    std::vector<std::optional<std::string>> values_;
};

} // namespace revolif::db
