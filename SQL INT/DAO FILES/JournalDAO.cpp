// ============================================================================
// JournalDAO.cpp
//
// Implementation of all persistence operations for the `journal` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "JournalDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a JournalRecord.
// ---------------------------------------------------------------------------
JournalRecord mapRow(const db::Row& row) {
    JournalRecord rec;
    rec.journalId = row.getInt64("journal_id");
    rec.userId = row.getInt64("user_id");
    rec.entryDate = row.getString("entry_date");
    rec.content = row.getString("content");

    if (!row.isNull("mood_rating")) {
        rec.moodRating = static_cast<int>(row.getInt64("mood_rating"));
    }

    rec.createdAt = row.getString("created_at");
    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

JournalDAO::JournalDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t JournalDAO::create(const JournalRecord& entry) {
    static const std::string sql =
        "INSERT INTO journal (user_id, entry_date, content, mood_rating) "
        "VALUES (?, ?, ?, ?)";

    std::vector<db::DBParam> params;
    params.reserve(4);

    params.emplace_back(entry.userId);
    params.emplace_back(entry.entryDate);
    params.emplace_back(entry.content);
    params.emplace_back(entry.moodRating.has_value()
                            ? db::DBParam(static_cast<int64_t>(*entry.moodRating))
                            : db::DBParam(nullptr));

    db_.executeUpdate(sql, params);
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<JournalRecord> JournalDAO::findById(int64_t journalId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM journal WHERE journal_id = ?",
        {db::DBParam(journalId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<JournalRecord> JournalDAO::findByUser(int64_t userId,
                                                  const PageRequest& page) {
    auto rs = db_.executeQuery(
        "SELECT * FROM journal WHERE user_id = ? ORDER BY entry_date DESC LIMIT ? OFFSET ?",
        {db::DBParam(userId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<JournalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<JournalRecord> JournalDAO::findByUserAndDate(
    int64_t userId,
    const std::string& entryDateIso) {

    if (entryDateIso.empty()) {
        throw std::invalid_argument(
            "JournalDAO::findByUserAndDate: entryDateIso must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM journal WHERE user_id = ? AND entry_date = ? ORDER BY entry_date DESC",
        {db::DBParam(userId), db::DBParam(entryDateIso)});

    std::vector<JournalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<JournalRecord> JournalDAO::findByUserAndDateRange(
    int64_t userId,
    const std::string& startDateIso,
    const std::string& endDateIso) {

    if (startDateIso.empty() || endDateIso.empty()) {
        throw std::invalid_argument(
            "JournalDAO::findByUserAndDateRange: date range bounds must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM journal WHERE user_id = ? AND entry_date BETWEEN ? AND ? ORDER BY entry_date DESC",
        {db::DBParam(userId),
         db::DBParam(startDateIso),
         db::DBParam(endDateIso)});

    std::vector<JournalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<JournalRecord> JournalDAO::findByUserAndMood(int64_t userId, int moodRating) {
    auto rs = db_.executeQuery(
        "SELECT * FROM journal WHERE user_id = ? AND mood_rating = ? ORDER BY entry_date DESC",
        {db::DBParam(userId), db::DBParam(static_cast<int64_t>(moodRating))});

    std::vector<JournalRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks
// =============================================================================

bool JournalDAO::exists(int64_t journalId) {
    return db_.exists(
        "SELECT 1 FROM journal WHERE journal_id = ? LIMIT 1",
        {db::DBParam(journalId)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t JournalDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM journal WHERE user_id = ?",
        {db::DBParam(userId)});
}

// =============================================================================
// Update
// =============================================================================

bool JournalDAO::update(const JournalRecord& entry) {
    static const std::string sql =
        "UPDATE journal SET "
        "user_id = ?, entry_date = ?, content = ?, mood_rating = ? "
        "WHERE journal_id = ?";

    std::vector<db::DBParam> params;
    params.reserve(5);

    params.emplace_back(entry.userId);
    params.emplace_back(entry.entryDate);
    params.emplace_back(entry.content);
    params.emplace_back(entry.moodRating.has_value()
                            ? db::DBParam(static_cast<int64_t>(*entry.moodRating))
                            : db::DBParam(nullptr));
    params.emplace_back(entry.journalId);

    auto result = db_.executeUpdate(sql, params);
    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool JournalDAO::remove(int64_t journalId) {
    auto result = db_.executeUpdate(
        "DELETE FROM journal WHERE journal_id = ?",
        {db::DBParam(journalId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
