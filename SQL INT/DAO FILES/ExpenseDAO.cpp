// ============================================================================
// ExpenseDAO.cpp
//
// Implementation of all persistence operations for the `expenses` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "ExpenseDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to an ExpenseRecord.
// ---------------------------------------------------------------------------
ExpenseRecord mapRow(const db::Row& row) {
    ExpenseRecord rec;
    rec.expenseId = row.getInt64("expense_id");
    rec.userId = row.getInt64("user_id");
    rec.categoryId = row.getInt64("category_id");
    rec.title = row.getString("title");
    rec.amount = row.getString("amount");
    rec.expenseDate = row.getString("expense_date");

    if (!row.isNull("description")) {
        rec.description = row.getString("description");
    }

    rec.createdAt = row.getString("created_at");
    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

ExpenseDAO::ExpenseDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t ExpenseDAO::create(const ExpenseRecord& expense) {
    static const std::string sql =
        "INSERT INTO expenses ("
        "user_id, category_id, title, amount, expense_date, description"
        ") VALUES (?, ?, ?, ?, ?, ?)";

    std::vector<db::DBParam> params;
    params.reserve(6);

    params.emplace_back(expense.userId);
    params.emplace_back(expense.categoryId);
    params.emplace_back(expense.title);
    params.emplace_back(expense.amount);
    params.emplace_back(expense.expenseDate);
    params.emplace_back(expense.description.has_value()
                            ? db::DBParam(*expense.description)
                            : db::DBParam(nullptr));

    db_.executeUpdate(sql, params);
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<ExpenseRecord> ExpenseDAO::findById(int64_t expenseId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM expenses WHERE expense_id = ?",
        {db::DBParam(expenseId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<ExpenseRecord> ExpenseDAO::findByUser(int64_t userId,
                                                    const PageRequest& page) {
    auto rs = db_.executeQuery(
        "SELECT * FROM expenses WHERE user_id = ? ORDER BY expense_date DESC LIMIT ? OFFSET ?",
        {db::DBParam(userId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<ExpenseRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<ExpenseRecord> ExpenseDAO::findByUserAndCategory(int64_t userId,
                                                              int64_t categoryId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM expenses WHERE user_id = ? AND category_id = ? ORDER BY expense_date DESC",
        {db::DBParam(userId), db::DBParam(categoryId)});

    std::vector<ExpenseRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<ExpenseRecord> ExpenseDAO::findByUserAndDateRange(
    int64_t userId,
    const std::string& startDateIso,
    const std::string& endDateIso) {

    if (startDateIso.empty() || endDateIso.empty()) {
        throw std::invalid_argument(
            "ExpenseDAO::findByUserAndDateRange: date range bounds must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM expenses WHERE user_id = ? AND expense_date BETWEEN ? AND ? ORDER BY expense_date DESC",
        {db::DBParam(userId),
         db::DBParam(startDateIso),
         db::DBParam(endDateIso)});

    std::vector<ExpenseRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<ExpenseRecord> ExpenseDAO::findByUserAndTitle(
    int64_t userId,
    const std::string& titleFragment) {

    if (titleFragment.empty()) {
        throw std::invalid_argument(
            "ExpenseDAO::findByUserAndTitle: titleFragment must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM expenses WHERE user_id = ? AND title LIKE CONCAT('%', ?, '%') ORDER BY expense_date DESC",
        {db::DBParam(userId), db::DBParam(titleFragment)});

    std::vector<ExpenseRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks
// =============================================================================

bool ExpenseDAO::exists(int64_t expenseId) {
    return db_.exists(
        "SELECT 1 FROM expenses WHERE expense_id = ? LIMIT 1",
        {db::DBParam(expenseId)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t ExpenseDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM expenses WHERE user_id = ?",
        {db::DBParam(userId)});
}

uint64_t ExpenseDAO::countByUserAndCategory(int64_t userId, int64_t categoryId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM expenses WHERE user_id = ? AND category_id = ?",
        {db::DBParam(userId), db::DBParam(categoryId)});
}

// =============================================================================
// Aggregate
// =============================================================================

std::string ExpenseDAO::sumAmountByUserAndCategoryForMonth(
    int64_t userId,
    int64_t categoryId,
    const std::string& yearMonth) {

    if (yearMonth.empty()) {
        throw std::invalid_argument(
            "ExpenseDAO::sumAmountByUserAndCategoryForMonth: yearMonth must not be empty");
    }

    return db_.scalar<std::string>(
        "SELECT COALESCE(SUM(amount), 0) FROM expenses "
        "WHERE user_id = ? AND category_id = ? AND DATE_FORMAT(expense_date, '%Y-%m') = ?",
        {db::DBParam(userId),
         db::DBParam(categoryId),
         db::DBParam(yearMonth)});
}

// =============================================================================
// Update
// =============================================================================

bool ExpenseDAO::update(const ExpenseRecord& expense) {
    static const std::string sql =
        "UPDATE expenses SET "
        "user_id = ?, category_id = ?, title = ?, amount = ?, "
        "expense_date = ?, description = ? "
        "WHERE expense_id = ?";

    std::vector<db::DBParam> params;
    params.reserve(7);

    params.emplace_back(expense.userId);
    params.emplace_back(expense.categoryId);
    params.emplace_back(expense.title);
    params.emplace_back(expense.amount);
    params.emplace_back(expense.expenseDate);
    params.emplace_back(expense.description.has_value()
                            ? db::DBParam(*expense.description)
                            : db::DBParam(nullptr));
    params.emplace_back(expense.expenseId);

    auto result = db_.executeUpdate(sql, params);
    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool ExpenseDAO::remove(int64_t expenseId) {
    auto result = db_.executeUpdate(
        "DELETE FROM expenses WHERE expense_id = ?",
        {db::DBParam(expenseId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
