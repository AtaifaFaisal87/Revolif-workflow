#pragma once
// ============================================================================
// CategoryBudgetDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `category_budgets` table.  A budget
// row links a user, a category, and a monthly spending limit (stored as
// exact decimal text — see DaoTypes.h).  CategoryBudgetDAO never parses
// or does arithmetic on `monthly_limit`; it only stores and retrieves it.
// Whether the user is over budget is a CategoryBudgetManager/ExpenseManager
// decision, made by combining CategoryBudgetDAO's limit read with
// ExpenseDAO's aggregate sum.
//
// SQL operations (conceptual)
// ---------------------------------------------------------------------------
//   create                INSERT INTO category_budgets (...) VALUES (...)
//   findById              SELECT * FROM category_budgets WHERE budget_id = ?
//   findByUser            SELECT * FROM category_budgets WHERE user_id = ?
//   findByUserAndCategory SELECT * FROM category_budgets WHERE user_id=? AND category_id=?
//   exists                SELECT EXISTS(SELECT 1 FROM category_budgets WHERE budget_id=?)
//   countByUser           SELECT COUNT(*) FROM category_budgets WHERE user_id=?
//   update                UPDATE category_budgets SET monthly_limit=?, updated_at=? WHERE budget_id=?
//   remove                DELETE FROM category_budgets WHERE budget_id=?
//
// Return values
// -------------
//   create                -> int64_t                    new budget_id
//   findById               -> std::optional<CategoryBudgetRecord>
//   findByUser / ...       -> std::vector<CategoryBudgetRecord>
//   exists                 -> bool
//   countByUser            -> uint64_t
//   update / remove         -> bool                     true iff affectedRows() > 0
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// An invalid user_id or category_id surfaces as DBConstraintViolationException
// (fk_category_budgets_user / fk_category_budgets_category).
// Inserting a duplicate (user_id, category_id) pair surfaces as
// DBConstraintViolationException (uq_category_budgets_user_category);
// CategoryBudgetManager should check findByUserAndCategory first to offer
// a clean "budget already exists" message.
//
// Interaction with managers
// -------------------------
// CategoryBudgetManager owns one CategoryBudgetDAO.  ExpenseManager may
// hold a reference to the same CategoryBudgetDAO (from the composition
// root) to read limits when checking budgets after adding an expense.
// CategoryBudgetDAO never calls ExpenseDAO itself.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class CategoryBudgetDAO : public BaseDAO {
public:
    explicit CategoryBudgetDAO(db::Database& database);
    ~CategoryBudgetDAO() = default;

    CategoryBudgetDAO(const CategoryBudgetDAO&) = delete;
    CategoryBudgetDAO& operator=(const CategoryBudgetDAO&) = delete;
    CategoryBudgetDAO(CategoryBudgetDAO&&) = delete;
    CategoryBudgetDAO& operator=(CategoryBudgetDAO&&) = delete;

    // ---- Create ----
    [[nodiscard]] int64_t create(const CategoryBudgetRecord& budget);

    // ---- Read ----
    [[nodiscard]] std::optional<CategoryBudgetRecord> findById(int64_t budgetId);
    [[nodiscard]] std::vector<CategoryBudgetRecord>   findByUser(int64_t userId,
                                                                   const PageRequest& page = {});
    [[nodiscard]] std::optional<CategoryBudgetRecord> findByUserAndCategory(int64_t userId,
                                                                               int64_t categoryId);
    [[nodiscard]] bool exists(int64_t budgetId);
    [[nodiscard]] uint64_t countByUser(int64_t userId);

    // ---- Update ----
    [[nodiscard]] bool update(const CategoryBudgetRecord& budget);

    // ---- Delete ----
    [[nodiscard]] bool remove(int64_t budgetId);
};

} // namespace revolif::dao
