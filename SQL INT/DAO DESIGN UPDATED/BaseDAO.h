#pragma once
// ============================================================================
// BaseDAO.h
//
// Common infrastructure shared by every table-specific DAO.  Provides the
// Database reference and the non-copyable / non-movable policy that all
// DAOs enforce.  Concrete DAOs inherit from BaseDAO and add their own
// table-specific CRUD and finder methods; no virtual CRUD interface is
// declared here because parameter and return types differ per table.
//
// Design rules (inherited by all DAOs):
//  - A DAO is constructed with a db::Database& and holds that reference
//    for its lifetime.
//  - DAOs are neither copyable nor movable (they are tied to one Database
//    instance and one connection scope).
//  - Value-returning query methods are [[nodiscard]] so the compiler warns
//    when a result is accidentally discarded.
// ============================================================================

#include "Database.h"

namespace revolif::dao {

class BaseDAO {
public:
    explicit BaseDAO(db::Database& database) : db_(database) {}
    virtual ~BaseDAO() = default;

    BaseDAO(const BaseDAO&) = delete;
    BaseDAO& operator=(const BaseDAO&) = delete;
    BaseDAO(BaseDAO&&) = delete;
    BaseDAO& operator=(BaseDAO&&) = delete;

protected:
    db::Database& db_;
};

} // namespace revolif::dao
