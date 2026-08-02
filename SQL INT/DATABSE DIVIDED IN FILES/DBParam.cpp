// =============================================================================
// DBParam.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "DBParam.h"

namespace revolif::db {

DBParam::DBParam() : type_(ParamType::Null), value_(std::monostate{}) {}
DBParam::DBParam(std::nullptr_t) : type_(ParamType::Null), value_(std::monostate{}) {}
DBParam::DBParam(int value) : type_(ParamType::Int64), value_(static_cast<int64_t>(value)) {}
DBParam::DBParam(int64_t value) : type_(ParamType::Int64), value_(value) {}
DBParam::DBParam(double value) : type_(ParamType::Double), value_(value) {}
DBParam::DBParam(bool value) : type_(ParamType::Bool), value_(value) {}
DBParam::DBParam(std::string value) : type_(ParamType::String), value_(std::move(value)) {}
DBParam::DBParam(const char* value) : type_(ParamType::String), value_(std::string(value)) {}

DBParam DBParam::dateTime(std::string isoDateTime) {
    DBParam p(std::move(isoDateTime));
    p.type_ = ParamType::DateTime;
    return p;
}

int64_t DBParam::asInt64() const {
    if (type_ != ParamType::Int64)
        throw std::logic_error("DBParam::asInt64: parameter is not an Int64");
    return std::get<int64_t>(value_);
}

double DBParam::asDouble() const {
    if (type_ != ParamType::Double)
        throw std::logic_error("DBParam::asDouble: parameter is not a Double");
    return std::get<double>(value_);
}

bool DBParam::asBool() const {
    if (type_ != ParamType::Bool)
        throw std::logic_error("DBParam::asBool: parameter is not a Bool");
    return std::get<bool>(value_);
}

const std::string& DBParam::asString() const {
    if (type_ != ParamType::String && type_ != ParamType::DateTime)
        throw std::logic_error("DBParam::asString: parameter is not a String/DateTime");
    return std::get<std::string>(value_);
}

std::string DBParam::debugString() const {
    if (sensitive_) return "***";
    switch (type_) {
        case ParamType::Null:     return "NULL";
        case ParamType::Int64:    return std::to_string(std::get<int64_t>(value_));
        case ParamType::Double:   return std::to_string(std::get<double>(value_));
        case ParamType::Bool:     return std::get<bool>(value_) ? "true" : "false";
        case ParamType::String:   return "'" + std::get<std::string>(value_) + "'";
        case ParamType::DateTime: return "DATETIME('" + std::get<std::string>(value_) + "')";
    }
    return "?";
}

} // namespace revolif::db
