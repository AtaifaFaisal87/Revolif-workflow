#pragma once

#include "Database.h"

namespace revolif::db {

class ConnectionGuard {
public:
    explicit ConnectionGuard(Database& db);

private:
    Database& db_;
};

} // namespace revolif::db
