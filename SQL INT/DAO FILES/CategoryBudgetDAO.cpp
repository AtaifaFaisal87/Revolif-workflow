// ============================================================================
// CategoryBudgetDAO.cpp
//
// Implementation of all persistence operations for the `category_budgets` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "CategoryBudgetDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a CategoryBudgetRecord.
// ---------------------------------------------------------------------------
CategoryBudgetRecord mapRow(const db::Row& row) {
    CategoryBudgetRecord rec;
    rec.budgetId = row.getInt64("budget_id");
    rec.userId = row.getInt64("user_id");
    rec.categoryId = row.getInt64("category_id");
    rec.monthlyLimit = row.getString("monthly_limit");
    rec.createdAt = row.getString("created_at");
    rec.updatedAt = row.getString("updated_at");
    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

CategoryBudgetDAO::CategoryBudgetDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t CategoryBudgetDAO::create(const CategoryBudgetRecord& budget) {
    static const std::string sql =
        "INSERT INTO category_budgets (user_id, category_id, monthly_limit) "
        "VALUES (?, ?, ?)";

    db_.executeUpdate(sql, {
        db::DBParam(budget.userId),
        db::DBParam(budget.categoryId),
        db::DBParam(budget.monthlyLimit)
    });
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<CategoryBudgetRecord> CategoryBudgetDAO::findById(int64_t budgetId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM category_budgets WHERE budget_id = ?",
        {db::DBParam(budgetId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<CategoryBudgetRecord> CategoryBudgetDAO::findByUser(
    int64_t userId,
    const PageRequest& page) {

    auto rs = db_.executeQuery(
        "SELECT * FROM category_budgets WHERE user_id = ? LIMIT ? OFFSET ?",
        {db::DBParam(userId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<CategoryBudgetRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::optional<CategoryBudgetRecord> CategoryBudgetDAO::findByUserAndCategory(
    int64_t userId,
    int64_t categoryId) {

    auto rs = db_.executeQuery(
        "SELECT * FROM category_budgets WHERE user_id = ? AND category_id = ?",
        {db::DBParam(userId), db::DBParam(categoryId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

// =============================================================================
// Existence checks
// =============================================================================

bool CategoryBudgetDAO::exists(int64_t budgetId) {
    return db_.exists(
        "SELECT 1 FROM category_budgets WHERE budget_id = ? LIMIT 1",
        {db::DBParam(budgetId)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t CategoryBudgetDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM category_budgets WHERE user_id = ?",
        {db::DBParam(userId)});
}

// =============================================================================
// Update
// =============================================================================

bool CategoryBudgetDAO::update(const CategoryBudgetRecord& budget) {
    static const std::string sql =
        "UPDATE category_budgets SET monthly_limit = ?, updated_at = ? "
        "WHERE budget_id = ?";

    auto result = db_.executeUpdate(sql, {
        db::DBParam(budget.monthlyLimit),
        db::DBParam(budget.updatedAt),
        db::DBParam(budget.budgetId)
    });

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool CategoryBudgetDAO::remove(int64_t budgetId) {
    auto result = db_.executeUpdate(
        "DELETE FROM category_budgets WHERE budget_id = ?",
        {db::DBParam(budgetId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
