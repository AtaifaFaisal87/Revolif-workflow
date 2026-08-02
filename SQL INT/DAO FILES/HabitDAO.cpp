// ============================================================================
// HabitDAO.cpp
//
// Implementation of all persistence operations for the `habits` and
// `habit_history` tables.  Every query uses prepared statements via the
// Database wrapper.  Database exceptions propagate unchanged; the only
// exceptions thrown directly by this file are std::invalid_argument for
// empty lookup keys.
// ============================================================================

#include "HabitDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a HabitRecord.
// ---------------------------------------------------------------------------
HabitRecord mapHabitRow(const db::Row& row) {
    HabitRecord rec;
    rec.habitId = row.getInt64("habit_id");
    rec.userId = row.getInt64("user_id");
    rec.name = row.getString("name");

    if (!row.isNull("description")) {
        rec.description = row.getString("description");
    }

    rec.frequency = row.getString("frequency");
    rec.isActive = row.getBool("is_active");
    rec.createdAt = row.getString("created_at");
    return rec;
}

// ---------------------------------------------------------------------------
// Maps a single database row to a HabitHistoryRecord.
// ---------------------------------------------------------------------------
HabitHistoryRecord mapHistoryRow(const db::Row& row) {
    HabitHistoryRecord rec;
    rec.historyId = row.getInt64("history_id");
    rec.habitId = row.getInt64("habit_id");
    rec.completionDatetime = row.getString("completion_datetime");
    rec.status = row.getString("status");
    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

HabitDAO::HabitDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create (habits)
// =============================================================================

int64_t HabitDAO::create(const HabitRecord& habit) {
    static const std::string sql =
        "INSERT INTO habits (user_id, name, description, frequency, is_active) "
        "VALUES (?, ?, ?, ?, ?)";

    std::vector<db::DBParam> params;
    params.reserve(5);

    params.emplace_back(habit.userId);
    params.emplace_back(habit.name);
    params.emplace_back(habit.description.has_value()
                            ? db::DBParam(*habit.description)
                            : db::DBParam(nullptr));
    params.emplace_back(habit.frequency);
    params.emplace_back(habit.isActive);

    db_.executeUpdate(sql, params);
    return db_.lastInsertId();
}

// =============================================================================
// Read (habits)
// =============================================================================

std::optional<HabitRecord> HabitDAO::findById(int64_t habitId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM habits WHERE habit_id = ?",
        {db::DBParam(habitId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapHabitRow(rs.at(0));
}

std::vector<HabitRecord> HabitDAO::findByUser(int64_t userId,
                                              const PageRequest& page) {
    auto rs = db_.executeQuery(
        "SELECT * FROM habits WHERE user_id = ? ORDER BY created_at DESC LIMIT ? OFFSET ?",
        {db::DBParam(userId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<HabitRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapHabitRow(rs.at(i)));
    }
    return results;
}

std::vector<HabitRecord> HabitDAO::findByUserAndActive(int64_t userId, bool active) {
    auto rs = db_.executeQuery(
        "SELECT * FROM habits WHERE user_id = ? AND is_active = ? ORDER BY created_at DESC",
        {db::DBParam(userId), db::DBParam(active)});

    std::vector<HabitRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapHabitRow(rs.at(i)));
    }
    return results;
}

std::vector<HabitRecord> HabitDAO::findByUserAndFrequency(
    int64_t userId,
    const std::string& frequency) {

    if (frequency.empty()) {
        throw std::invalid_argument(
            "HabitDAO::findByUserAndFrequency: frequency must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM habits WHERE user_id = ? AND frequency = ? ORDER BY created_at DESC",
        {db::DBParam(userId), db::DBParam(frequency)});

    std::vector<HabitRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapHabitRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks (habits)
// =============================================================================

bool HabitDAO::exists(int64_t habitId) {
    return db_.exists(
        "SELECT 1 FROM habits WHERE habit_id = ? LIMIT 1",
        {db::DBParam(habitId)});
}

// =============================================================================
// Counts (habits)
// =============================================================================

uint64_t HabitDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM habits WHERE user_id = ?",
        {db::DBParam(userId)});
}

uint64_t HabitDAO::countByUserAndActive(int64_t userId, bool active) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM habits WHERE user_id = ? AND is_active = ?",
        {db::DBParam(userId), db::DBParam(active)});
}

// =============================================================================
// Update (habits)
// =============================================================================

bool HabitDAO::update(const HabitRecord& habit) {
    static const std::string sql =
        "UPDATE habits SET "
        "user_id = ?, name = ?, description = ?, frequency = ?, is_active = ? "
        "WHERE habit_id = ?";

    std::vector<db::DBParam> params;
    params.reserve(6);

    params.emplace_back(habit.userId);
    params.emplace_back(habit.name);
    params.emplace_back(habit.description.has_value()
                            ? db::DBParam(*habit.description)
                            : db::DBParam(nullptr));
    params.emplace_back(habit.frequency);
    params.emplace_back(habit.isActive);
    params.emplace_back(habit.habitId);

    auto result = db_.executeUpdate(sql, params);
    return result.affectedRows() > 0;
}

bool HabitDAO::updateActive(int64_t habitId, bool active) {
    auto result = db_.executeUpdate(
        "UPDATE habits SET is_active = ? WHERE habit_id = ?",
        {db::DBParam(active), db::DBParam(habitId)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete (habits)
// =============================================================================

bool HabitDAO::remove(int64_t habitId) {
    auto result = db_.executeUpdate(
        "DELETE FROM habits WHERE habit_id = ?",
        {db::DBParam(habitId)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Create (habit_history)
// =============================================================================

int64_t HabitDAO::createHistory(const HabitHistoryRecord& entry) {
    static const std::string sql =
        "INSERT INTO habit_history (habit_id, completion_datetime, status) "
        "VALUES (?, ?, ?)";

    db_.executeUpdate(sql, {
        db::DBParam(entry.habitId),
        db::DBParam(entry.completionDatetime),
        db::DBParam(entry.status)
    });
    return db_.lastInsertId();
}

// =============================================================================
// Read (habit_history)
// =============================================================================

std::vector<HabitHistoryRecord> HabitDAO::findHistoryByHabit(
    int64_t habitId,
    const PageRequest& page) {

    auto rs = db_.executeQuery(
        "SELECT * FROM habit_history WHERE habit_id = ? ORDER BY completion_datetime DESC LIMIT ? OFFSET ?",
        {db::DBParam(habitId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<HabitHistoryRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapHistoryRow(rs.at(i)));
    }
    return results;
}

std::vector<HabitHistoryRecord> HabitDAO::findHistoryByHabitAndDateRange(
    int64_t habitId,
    const std::string& startIso,
    const std::string& endIso) {

    if (startIso.empty() || endIso.empty()) {
        throw std::invalid_argument(
            "HabitDAO::findHistoryByHabitAndDateRange: date range bounds must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM habit_history WHERE habit_id = ? AND completion_datetime BETWEEN ? AND ? ORDER BY completion_datetime DESC",
        {db::DBParam(habitId),
         db::DBParam(startIso),
         db::DBParam(endIso)});

    std::vector<HabitHistoryRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapHistoryRow(rs.at(i)));
    }
    return results;
}

std::vector<HabitHistoryRecord> HabitDAO::findHistoryByHabitAndStatus(
    int64_t habitId,
    const std::string& status) {

    if (status.empty()) {
        throw std::invalid_argument(
            "HabitDAO::findHistoryByHabitAndStatus: status must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM habit_history WHERE habit_id = ? AND status = ? ORDER BY completion_datetime DESC",
        {db::DBParam(habitId), db::DBParam(status)});

    std::vector<HabitHistoryRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapHistoryRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Counts (habit_history)
// =============================================================================

uint64_t HabitDAO::countCompletedByHabit(int64_t habitId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM habit_history WHERE habit_id = ? AND status = 'COMPLETED'",
        {db::DBParam(habitId)});
}

uint64_t HabitDAO::countMissedByHabit(int64_t habitId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM habit_history WHERE habit_id = ? AND status = 'MISSED'",
        {db::DBParam(habitId)});
}

// =============================================================================
// Delete (habit_history)
// =============================================================================

bool HabitDAO::removeHistory(int64_t historyId) {
    auto result = db_.executeUpdate(
        "DELETE FROM habit_history WHERE history_id = ?",
        {db::DBParam(historyId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
