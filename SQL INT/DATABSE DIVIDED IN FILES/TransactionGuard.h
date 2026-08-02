#pragma once

#include "Database.h"

namespace revolif::db {

class TransactionGuard {
public:
    explicit TransactionGuard(Database& db);
    ~TransactionGuard();

    void commit();

    // Non-copyable, non-movable
    TransactionGuard(const TransactionGuard&) = delete;
    TransactionGuard& operator=(const TransactionGuard&) = delete;
    TransactionGuard(TransactionGuard&&) = delete;
    TransactionGuard& operator=(TransactionGuard&&) = delete;

private:
    Database& db_;
    bool active_;
    bool committed_;
};

} // namespace revolif::db
