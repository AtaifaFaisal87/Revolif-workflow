// ============================================================================
// NoteDAO.cpp
//
// Implementation of all persistence operations for the `notes` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "NoteDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a NoteRecord.
// ---------------------------------------------------------------------------
NoteRecord mapRow(const db::Row& row) {
    NoteRecord rec;
    rec.noteId = row.getInt64("note_id");
    rec.userId = row.getInt64("user_id");
    rec.title = row.getString("title");
    rec.content = row.getString("content");
    rec.isPinned = row.getBool("is_pinned");
    rec.createdAt = row.getString("created_at");
    rec.updatedAt = row.getString("updated_at");
    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

NoteDAO::NoteDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t NoteDAO::create(const NoteRecord& note) {
    static const std::string sql =
        "INSERT INTO notes (user_id, title, content, is_pinned) "
        "VALUES (?, ?, ?, ?)";

    db_.executeUpdate(sql, {
        db::DBParam(note.userId),
        db::DBParam(note.title),
        db::DBParam(note.content),
        db::DBParam(note.isPinned)
    });
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<NoteRecord> NoteDAO::findById(int64_t noteId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM notes WHERE note_id = ?",
        {db::DBParam(noteId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<NoteRecord> NoteDAO::findByUser(int64_t userId,
                                              const PageRequest& page) {
    auto rs = db_.executeQuery(
        "SELECT * FROM notes WHERE user_id = ? ORDER BY updated_at DESC LIMIT ? OFFSET ?",
        {db::DBParam(userId),
         db::DBParam(static_cast<int64_t>(page.limit)),
         db::DBParam(static_cast<int64_t>(page.offset))});

    std::vector<NoteRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<NoteRecord> NoteDAO::findByUserAndPinned(int64_t userId, bool pinned) {
    auto rs = db_.executeQuery(
        "SELECT * FROM notes WHERE user_id = ? AND is_pinned = ? ORDER BY updated_at DESC",
        {db::DBParam(userId), db::DBParam(pinned)});

    std::vector<NoteRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<NoteRecord> NoteDAO::findByUserAndTitle(
    int64_t userId,
    const std::string& titleFragment) {

    if (titleFragment.empty()) {
        throw std::invalid_argument(
            "NoteDAO::findByUserAndTitle: titleFragment must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM notes WHERE user_id = ? AND title LIKE CONCAT('%', ?, '%') ORDER BY updated_at DESC",
        {db::DBParam(userId), db::DBParam(titleFragment)});

    std::vector<NoteRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks
// =============================================================================

bool NoteDAO::exists(int64_t noteId) {
    return db_.exists(
        "SELECT 1 FROM notes WHERE note_id = ? LIMIT 1",
        {db::DBParam(noteId)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t NoteDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM notes WHERE user_id = ?",
        {db::DBParam(userId)});
}

// =============================================================================
// Update
// =============================================================================

bool NoteDAO::update(const NoteRecord& note) {
    static const std::string sql =
        "UPDATE notes SET "
        "user_id = ?, title = ?, content = ?, is_pinned = ? "
        "WHERE note_id = ?";

    auto result = db_.executeUpdate(sql, {
        db::DBParam(note.userId),
        db::DBParam(note.title),
        db::DBParam(note.content),
        db::DBParam(note.isPinned),
        db::DBParam(note.noteId)
    });

    return result.affectedRows() > 0;
}

bool NoteDAO::updatePinned(int64_t noteId, bool pinned) {
    auto result = db_.executeUpdate(
        "UPDATE notes SET is_pinned = ? WHERE note_id = ?",
        {db::DBParam(pinned), db::DBParam(noteId)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool NoteDAO::remove(int64_t noteId) {
    auto result = db_.executeUpdate(
        "DELETE FROM notes WHERE note_id = ?",
        {db::DBParam(noteId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
