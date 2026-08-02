// ============================================================================
// GoalDAO.cpp
//
// Implementation of all persistence operations for the `goals` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "GoalDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a GoalRecord.  Column names match the
// revolif_schema.sql CREATE TABLE exactly.
// ---------------------------------------------------------------------------
GoalRecord mapRow(const db::Row& row) {
    GoalRecord rec;
    rec.goalId = row.getInt64("goal_id");
    rec.userId = row.getInt64("user_id");
    rec.categoryId = row.getInt64("category_id");
    rec.title = row.getString("title");

    if (!row.isNull("description")) {
        rec.description = row.getString("description");
    }

    rec.deadline = row.getString("deadline");
    rec.status = row.getString("status");
    rec.createdAt = row.getString("created_at");

    if (!row.isNull("completed_at")) {
        rec.completedAt = row.getString("completed_at");
    }

    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

GoalDAO::GoalDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t GoalDAO::create(const GoalRecord& goal) {
    static const std::string sql =
        "INSERT INTO goals ("
        "user_id, category_id, title, description, deadline, status, completed_at"
        ") VALUES (?, ?, ?, ?, ?, ?, ?)";

    std::vector<db::DBParam> params;
    params.reserve(7);

    params.emplace_back(goal.userId);
    params.emplace_back(goal.categoryId);
    params.emplace_back(goal.title);
    params.emplace_back(goal.description.has_value()
                            ? db::DBParam(*goal.description)
                            : db::DBParam(nullptr));
    params.emplace_back(goal.deadline);
    params.emplace_back(goal.status);
    params.emplace_back(goal.completedAt.has_value()
                            ? db::DBParam(*goal.completedAt)
                            : db::DBParam(nullptr));

    db_.executeUpdate(sql, params);
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<GoalRecord> GoalDAO::findById(int64_t goalId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM goals WHERE goal_id = ?",
        {db::DBParam(goalId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<GoalRecord> GoalDAO::findByUser(int64_t userId,
                                              const PageRequest& page) {
    auto rs = db_.executeQuery(
        "SELECT * FROM goals WHERE user_id = ? ORDER BY deadline ASC LIMIT ? OFFSET ?",
        {db::DBParam(userId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<GoalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<GoalRecord> GoalDAO::findByUserAndStatus(int64_t userId,
                                                      const std::string& status) {
    auto rs = db_.executeQuery(
        "SELECT * FROM goals WHERE user_id = ? AND status = ? ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(status)});

    std::vector<GoalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<GoalRecord> GoalDAO::findByUserAndCategory(int64_t userId,
                                                         int64_t categoryId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM goals WHERE user_id = ? AND category_id = ? ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(categoryId)});

    std::vector<GoalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<GoalRecord> GoalDAO::findByUserAndTitle(int64_t userId,
                                                     const std::string& titleFragment) {
    if (titleFragment.empty()) {
        throw std::invalid_argument(
            "GoalDAO::findByUserAndTitle: titleFragment must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM goals WHERE user_id = ? AND title LIKE CONCAT('%', ?, '%') ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(titleFragment)});

    std::vector<GoalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<GoalRecord> GoalDAO::findByUserAndDeadline(
    int64_t userId,
    const std::string& deadlineDateIso) {

    if (deadlineDateIso.empty()) {
        throw std::invalid_argument(
            "GoalDAO::findByUserAndDeadline: deadlineDateIso must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM goals WHERE user_id = ? AND DATE(deadline) = ? ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(deadlineDateIso)});

    std::vector<GoalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks
// =============================================================================

bool GoalDAO::exists(int64_t goalId) {
    return db_.exists(
        "SELECT 1 FROM goals WHERE goal_id = ? LIMIT 1",
        {db::DBParam(goalId)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t GoalDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM goals WHERE user_id = ?",
        {db::DBParam(userId)});
}

uint64_t GoalDAO::countByUserAndStatus(int64_t userId,
                                        const std::string& status) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM goals WHERE user_id = ? AND status = ?",
        {db::DBParam(userId), db::DBParam(status)});
}

uint64_t GoalDAO::countByUserAndCategory(int64_t userId,
                                          int64_t categoryId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM goals WHERE user_id = ? AND category_id = ?",
        {db::DBParam(userId), db::DBParam(categoryId)});
}

uint64_t GoalDAO::countCompletedByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM goals WHERE user_id = ? AND status = 'COMPLETED'",
        {db::DBParam(userId)});
}

// =============================================================================
// Update
// =============================================================================

bool GoalDAO::update(const GoalRecord& goal) {
    static const std::string sql =
        "UPDATE goals SET "
        "user_id = ?, category_id = ?, title = ?, description = ?, "
        "deadline = ?, status = ?, completed_at = ? "
        "WHERE goal_id = ?";

    std::vector<db::DBParam> params;
    params.reserve(8);

    params.emplace_back(goal.userId);
    params.emplace_back(goal.categoryId);
    params.emplace_back(goal.title);
    params.emplace_back(goal.description.has_value()
                            ? db::DBParam(*goal.description)
                            : db::DBParam(nullptr));
    params.emplace_back(goal.deadline);
    params.emplace_back(goal.status);
    params.emplace_back(goal.completedAt.has_value()
                            ? db::DBParam(*goal.completedAt)
                            : db::DBParam(nullptr));
    params.emplace_back(goal.goalId);

    auto result = db_.executeUpdate(sql, params);
    return result.affectedRows() > 0;
}

bool GoalDAO::updateStatus(int64_t goalId,
                           const std::string& status,
                           std::optional<std::string> completedAtIso) {
    auto result = db_.executeUpdate(
        "UPDATE goals SET status = ?, completed_at = ? WHERE goal_id = ?",
        {db::DBParam(status),
         completedAtIso.has_value()
             ? db::DBParam(*completedAtIso)
             : db::DBParam(nullptr),
         db::DBParam(goalId)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool GoalDAO::remove(int64_t goalId) {
    auto result = db_.executeUpdate(
        "DELETE FROM goals WHERE goal_id = ?",
        {db::DBParam(goalId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
