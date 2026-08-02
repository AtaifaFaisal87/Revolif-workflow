// =============================================================================
// ConnectionGuard.cpp
// REVOLIF - Database Access Layer
// =============================================================================

#include "ConnectionGuard.h"

namespace revolif::db {

ConnectionGuard::ConnectionGuard(Database& db) : db_(db) {
    db_.ensureConnection();
}

} // namespace revolif::db
