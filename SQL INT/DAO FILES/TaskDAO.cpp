// ============================================================================
// TaskDAO.cpp
//
// Implementation of all persistence operations for the `tasks` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "TaskDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a TaskRecord.  Column names match the
// revolif_schema.sql CREATE TABLE exactly.
// ---------------------------------------------------------------------------
TaskRecord mapRow(const db::Row& row) {
    TaskRecord rec;
    rec.taskId = row.getInt64("task_id");
    rec.userId = row.getInt64("user_id");
    rec.categoryId = row.getInt64("category_id");
    rec.taskType = row.getString("task_type");
    rec.title = row.getString("title");

    if (!row.isNull("description")) {
        rec.description = row.getString("description");
    }

    rec.deadline = row.getString("deadline");
    rec.status = row.getString("status");
    rec.priority = row.getString("priority");
    rec.isRecurring = row.getBool("is_recurring");
    rec.recurrenceInterval = row.getString("recurrence_interval");
    rec.createdAt = row.getString("created_at");

    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

TaskDAO::TaskDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t TaskDAO::create(const TaskRecord& task) {
    static const std::string sql =
        "INSERT INTO tasks ("
        "user_id, category_id, task_type, title, description, "
        "deadline, status, priority, is_recurring, recurrence_interval"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    std::vector<db::DBParam> params;
    params.reserve(10);

    params.emplace_back(task.userId);
    params.emplace_back(task.categoryId);
    params.emplace_back(task.taskType);
    params.emplace_back(task.title);
    params.emplace_back(task.description.has_value()
                            ? db::DBParam(*task.description)
                            : db::DBParam(nullptr));
    params.emplace_back(task.deadline);
    params.emplace_back(task.status);
    params.emplace_back(task.priority);
    params.emplace_back(task.isRecurring);
    params.emplace_back(task.recurrenceInterval);

    db_.executeUpdate(sql, params);
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<TaskRecord> TaskDAO::findById(int64_t taskId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM tasks WHERE task_id = ?",
        {db::DBParam(taskId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<TaskRecord> TaskDAO::findByUser(int64_t userId,
                                              const PageRequest& page) {
    auto rs = db_.executeQuery(
        "SELECT * FROM tasks WHERE user_id = ? ORDER BY deadline ASC LIMIT ? OFFSET ?",
        {db::DBParam(userId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<TaskRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<TaskRecord> TaskDAO::findByUserAndStatus(int64_t userId,
                                                      const std::string& status) {
    auto rs = db_.executeQuery(
        "SELECT * FROM tasks WHERE user_id = ? AND status = ? ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(status)});

    std::vector<TaskRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<TaskRecord> TaskDAO::findByUserAndCategory(int64_t userId,
                                                         int64_t categoryId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM tasks WHERE user_id = ? AND category_id = ? ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(categoryId)});

    std::vector<TaskRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<TaskRecord> TaskDAO::findByUserAndTitle(int64_t userId,
                                                     const std::string& titleFragment) {
    if (titleFragment.empty()) {
        throw std::invalid_argument(
            "TaskDAO::findByUserAndTitle: titleFragment must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM tasks WHERE user_id = ? AND title LIKE CONCAT('%', ?, '%') ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(titleFragment)});

    std::vector<TaskRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<TaskRecord> TaskDAO::findByUserAndDeadline(
    int64_t userId,
    const std::string& deadlineDateIso) {

    if (deadlineDateIso.empty()) {
        throw std::invalid_argument(
            "TaskDAO::findByUserAndDeadline: deadlineDateIso must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM tasks WHERE user_id = ? AND DATE(deadline) = ? ORDER BY deadline ASC",
        {db::DBParam(userId), db::DBParam(deadlineDateIso)});

    std::vector<TaskRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<TaskRecord> TaskDAO::findRecurringByUser(int64_t userId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM tasks WHERE user_id = ? AND is_recurring = TRUE ORDER BY deadline ASC",
        {db::DBParam(userId)});

    std::vector<TaskRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks
// =============================================================================

bool TaskDAO::exists(int64_t taskId) {
    return db_.exists(
        "SELECT 1 FROM tasks WHERE task_id = ? LIMIT 1",
        {db::DBParam(taskId)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t TaskDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM tasks WHERE user_id = ?",
        {db::DBParam(userId)});
}

uint64_t TaskDAO::countByUserAndStatus(int64_t userId,
                                        const std::string& status) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM tasks WHERE user_id = ? AND status = ?",
        {db::DBParam(userId), db::DBParam(status)});
}

uint64_t TaskDAO::countByUserAndCategory(int64_t userId,
                                          int64_t categoryId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM tasks WHERE user_id = ? AND category_id = ?",
        {db::DBParam(userId), db::DBParam(categoryId)});
}

// =============================================================================
// Update
// =============================================================================

bool TaskDAO::update(const TaskRecord& task) {
    static const std::string sql =
        "UPDATE tasks SET "
        "user_id = ?, category_id = ?, task_type = ?, title = ?, "
        "description = ?, deadline = ?, status = ?, priority = ?, "
        "is_recurring = ?, recurrence_interval = ? "
        "WHERE task_id = ?";

    std::vector<db::DBParam> params;
    params.reserve(11);

    params.emplace_back(task.userId);
    params.emplace_back(task.categoryId);
    params.emplace_back(task.taskType);
    params.emplace_back(task.title);
    params.emplace_back(task.description.has_value()
                            ? db::DBParam(*task.description)
                            : db::DBParam(nullptr));
    params.emplace_back(task.deadline);
    params.emplace_back(task.status);
    params.emplace_back(task.priority);
    params.emplace_back(task.isRecurring);
    params.emplace_back(task.recurrenceInterval);
    params.emplace_back(task.taskId);

    auto result = db_.executeUpdate(sql, params);
    return result.affectedRows() > 0;
}

bool TaskDAO::updateStatus(int64_t taskId, const std::string& status) {
    auto result = db_.executeUpdate(
        "UPDATE tasks SET status = ? WHERE task_id = ?",
        {db::DBParam(status), db::DBParam(taskId)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool TaskDAO::remove(int64_t taskId) {
    auto result = db_.executeUpdate(
        "DELETE FROM tasks WHERE task_id = ?",
        {db::DBParam(taskId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
