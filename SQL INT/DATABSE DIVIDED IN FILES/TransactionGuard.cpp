// =============================================================================
// TransactionGuard.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "TransactionGuard.h"

namespace revolif::db {

TransactionGuard::TransactionGuard(Database& db)
    : db_(db), active_(false), committed_(false)
{
    db_.beginTransaction();
    active_ = true;
}

void TransactionGuard::commit() {
    if (!active_ || committed_) {
        throw DBTransactionException(
            "TransactionGuard::commit: no active, uncommitted transaction",
            0, "", "[TransactionGuard::commit]", ErrorCategory::Transaction);
    }
    db_.commit();
    committed_ = true;
    active_ = false;
}

TransactionGuard::~TransactionGuard() {
    if (active_ && !committed_) {
        try {
            db_.rollback();
        } catch (...) {
            // Destructor must not throw; the connection layer already logged
            // the underlying error via translateAndThrow/logEvent.
        }
    }
}

} // namespace revolif::db
