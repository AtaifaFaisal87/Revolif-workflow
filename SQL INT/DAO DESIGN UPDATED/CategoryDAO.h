#pragma once
// ============================================================================
// CategoryDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `categories` table. Categories are
// referenced by tasks, goals, expenses, and category_budgets via foreign
// key. CategoryDAO does not enforce business rules about which category
// belongs to which module — it only stores and retrieves the rows.  The
// Manager layer (TaskManager, GoalManager, ExpenseManager, etc.) is
// responsible for validating that a category's category_type matches the
// entity being created (e.g. a task must use a category whose
// category_type = 'TASK').
//
// SQL operations (conceptual)
// ---------------------------------------------------------------------------
//   create                INSERT INTO categories (...) VALUES (...)
//   findById              SELECT * FROM categories WHERE category_id = ?
//   findByType            SELECT * FROM categories WHERE category_type = ? ORDER BY name
//   findByName            SELECT * FROM categories WHERE name LIKE CONCAT('%',?,'%')
//   findAll               SELECT * FROM categories ORDER BY name
//   findDefaults          SELECT * FROM categories WHERE is_default = TRUE
//   exists                SELECT EXISTS(SELECT 1 FROM categories WHERE category_id=?)
//   existsByName          SELECT EXISTS(SELECT 1 FROM categories WHERE name=?)
//   countByType           SELECT COUNT(*) FROM categories WHERE category_type=?
//   update                UPDATE categories SET <mutable columns> WHERE category_id=?
//   remove                DELETE FROM categories WHERE category_id=?
//
// Return values
// -------------
//   create                -> int64_t                    new category_id
//   findById               -> std::optional<CategoryRecord>
//   findByType / ...       -> std::vector<CategoryRecord>
//   exists / existsByName  -> bool
//   countByType            -> uint64_t
//   update / remove         -> bool                     true iff affectedRows() > 0
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// Removing a category that is referenced by tasks, goals, expenses, or
// category_budgets surfaces as DBConstraintViolationException (FK
// violation); the caller (AdminManager/CategoryManager) should either
// reassign or delete dependent rows first, or catch the exception and
// surface a user-friendly message.
//
// Interaction with managers
// -------------------------
// CategoryManager owns one CategoryDAO.  TaskManager, GoalManager, and
// ExpenseManager may hold a reference to the same CategoryDAO instance
// (obtained from the composition root) to validate category_id and
// category_type before creating or updating their own entities.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class CategoryDAO : public BaseDAO {
public:
    explicit CategoryDAO(db::Database& database);
    ~CategoryDAO() = default;

    CategoryDAO(const CategoryDAO&) = delete;
    CategoryDAO& operator=(const CategoryDAO&) = delete;
    CategoryDAO(CategoryDAO&&) = delete;
    CategoryDAO& operator=(CategoryDAO&&) = delete;

    // ---- Create ----
    [[nodiscard]] int64_t create(const CategoryRecord& category);

    // ---- Read ----
    [[nodiscard]] std::optional<CategoryRecord> findById(int64_t categoryId);
    [[nodiscard]] std::vector<CategoryRecord>   findByType(const std::string& categoryType);
    [[nodiscard]] std::vector<CategoryRecord>   findByName(const std::string& nameFragment);
    [[nodiscard]] std::vector<CategoryRecord>   findAll();
    [[nodiscard]] std::vector<CategoryRecord>   findDefaults();
    [[nodiscard]] bool exists(int64_t categoryId);
    [[nodiscard]] bool existsByName(const std::string& name);
    [[nodiscard]] uint64_t countByType(const std::string& categoryType);

    // ---- Update ----
    [[nodiscard]] bool update(const CategoryRecord& category);

    // ---- Delete ----
    [[nodiscard]] bool remove(int64_t categoryId);
};

} // namespace revolif::dao
