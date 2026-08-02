#pragma once

#include <string>
#include <variant>
#include <optional>
#include <cstdint>
#include <stdexcept>

namespace revolif::db {

enum class ParamType
{
    Null,
    Int64,
    Double,
    Bool,
    String,
    DateTime
};

class DBParam {
public:
    DBParam();
    DBParam(std::nullptr_t);
    DBParam(int value);
    DBParam(int64_t value);
    DBParam(double value);
    DBParam(bool value);
    DBParam(std::string value);
    DBParam(const char* value);

    static DBParam dateTime(std::string isoDateTime);

    ParamType type() const noexcept { return type_; }

    int64_t asInt64() const;
    double asDouble() const;
    bool asBool() const;
    const std::string& asString() const;

    void setSensitive(bool sensitive) noexcept { sensitive_ = sensitive; }
    bool isSensitive() const noexcept { return sensitive_; }

    std::string debugString() const;

private:
    ParamType type_;
    std::variant<std::monostate, int64_t, double, bool, std::string> value_;
    bool sensitive_ = false;
};

} // namespace revolif::db
