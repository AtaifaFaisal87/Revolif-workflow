#pragma once
// ============================================================================
// ExpenseDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `expenses` table. Amounts are carried
// as exact decimal text (see DaoTypes.h) — ExpenseDAO never parses,
// rounds, or does arithmetic on `amount`; it only stores and retrieves it.
// Whether a set of expenses puts a user over budget is a
// CategoryBudgetManager/ExpenseManager decision, made by combining
// ExpenseDAO's aggregate read below with CategoryBudgetDAO's stored limit.
//
// SQL operations (conceptual)
// ---------------------------------------------------------------------------
//   create                        INSERT INTO expenses (...) VALUES (...)
//   findById                      SELECT * FROM expenses WHERE expense_id = ?
//   findByUser                    SELECT * FROM expenses WHERE user_id = ? ORDER BY expense_date DESC
//   findByUserAndCategory         SELECT * FROM expenses WHERE user_id=? AND category_id=?
//   findByUserAndDateRange        SELECT * FROM expenses WHERE user_id=? AND expense_date BETWEEN ? AND ?
//   findByUserAndTitle            SELECT * FROM expenses WHERE user_id=? AND title LIKE CONCAT('%',?,'%')
//   exists                        SELECT EXISTS(SELECT 1 FROM expenses WHERE expense_id=?)
//   countByUser                   SELECT COUNT(*) FROM expenses WHERE user_id=?
//   countByUserAndCategory        SELECT COUNT(*) FROM expenses WHERE user_id=? AND category_id=?
//   update                        UPDATE expenses SET <mutable columns> WHERE expense_id=?
//   remove                        DELETE FROM expenses WHERE expense_id=?
//   sumAmountByUserAndCategoryForMonth
//                                 SELECT COALESCE(SUM(amount),0) FROM expenses
//                                 WHERE user_id=? AND category_id=? AND
//                                 DATE_FORMAT(expense_date,'%Y-%m')=?
//
// Return values
// -------------
//   create                -> int64_t                    new expense_id
//   findById               -> std::optional<ExpenseRecord>
//   findByUser / ...       -> std::vector<ExpenseRecord>
//   exists                 -> bool
//   countByUser / ...      -> uint64_t
//   update / remove         -> bool                       true iff affectedRows() > 0
//   sumAmountByUserAndCategoryForMonth -> std::string      decimal text, "0.00" when no rows
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// The CHECK constraint chk_expenses_amount_positive rejects amount <= 0 at
// the database with a constraint-violation error; ExpenseManager should
// validate amount > 0 before calling create/update to avoid surfacing a raw
// SQL error to the UI. An invalid category_id surfaces as
// DBConstraintViolationException (fk_expenses_category).
//
// Interaction with managers
// -------------------------
// ExpenseManager owns one ExpenseDAO and is its only caller. When adding an
// expense, ExpenseManager may first call
// sumAmountByUserAndCategoryForMonth(...) and compare it against a limit it
// reads from CategoryBudgetDAO to warn the user before/after calling
// create — that comparison is ExpenseManager's business logic, not
// ExpenseDAO's. ExpenseDAO never calls CategoryBudgetDAO itself.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class ExpenseDAO : public BaseDAO {
public:
    explicit ExpenseDAO(db::Database& database);
    ~ExpenseDAO() = default;

    ExpenseDAO(const ExpenseDAO&) = delete;
    ExpenseDAO& operator=(const ExpenseDAO&) = delete;
    ExpenseDAO(ExpenseDAO&&) = delete;
    ExpenseDAO& operator=(ExpenseDAO&&) = delete;

    // ---- Create ----
    [[nodiscard]] int64_t create(const ExpenseRecord& expense);

    // ---- Read ----
    [[nodiscard]] std::optional<ExpenseRecord> findById(int64_t expenseId);
    [[nodiscard]] std::vector<ExpenseRecord>   findByUser(int64_t userId, const PageRequest& page = {});
    [[nodiscard]] std::vector<ExpenseRecord>   findByUserAndCategory(int64_t userId, int64_t categoryId);
    [[nodiscard]] std::vector<ExpenseRecord>   findByUserAndDateRange(int64_t userId,
                                                          const std::string& startDateIso,
                                                          const std::string& endDateIso);
    [[nodiscard]] std::vector<ExpenseRecord>   findByUserAndTitle(int64_t userId, const std::string& titleFragment);
    [[nodiscard]] bool exists(int64_t expenseId);
    [[nodiscard]] uint64_t countByUser(int64_t userId);
    [[nodiscard]] uint64_t countByUserAndCategory(int64_t userId, int64_t categoryId);
    [[nodiscard]] std::string                  sumAmountByUserAndCategoryForMonth(int64_t userId,
                                                                     int64_t categoryId,
                                                                     const std::string& yearMonth);

    // ---- Update ----
    [[nodiscard]] bool update(const ExpenseRecord& expense);

    // ---- Delete ----
    [[nodiscard]] bool remove(int64_t expenseId);
};

} // namespace revolif::dao
