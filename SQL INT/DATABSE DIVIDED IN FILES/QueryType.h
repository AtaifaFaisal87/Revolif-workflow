#pragma once

namespace revolif::db {

enum class QueryType
{
    Select,
    Insert,
    Update,
    Delete,
    Replace,
    Create,
    Drop,
    Alter,
    Transaction,
    Other
};

} // namespace revolif::db
