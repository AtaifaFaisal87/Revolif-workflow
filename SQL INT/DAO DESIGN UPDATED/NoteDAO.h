#pragma once
// ============================================================================
// NoteDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `notes` table.  NoteDAO does not
// decide what "pinned" means in the UI (top of list, special styling, etc.)
// — it only stores and retrieves the flag it is given.  Sorting or
// filtering by pin status is a NoteManager concern.
//
// SQL operations (conceptual)
// ---------------------------------------------------------------------------
//   create                INSERT INTO notes (...) VALUES (...)
//   findById              SELECT * FROM notes WHERE note_id = ?
//   findByUser            SELECT * FROM notes WHERE user_id = ? ORDER BY updated_at DESC
//   findByUserAndPinned   SELECT * FROM notes WHERE user_id=? AND is_pinned=? ORDER BY updated_at DESC
//   findByUserAndTitle    SELECT * FROM notes WHERE user_id=? AND title LIKE CONCAT('%',?,'%')
//   exists                SELECT EXISTS(SELECT 1 FROM notes WHERE note_id=?)
//   countByUser           SELECT COUNT(*) FROM notes WHERE user_id=?
//   update                UPDATE notes SET <mutable columns> WHERE note_id=?
//   updatePinned          UPDATE notes SET is_pinned=? WHERE note_id=?
//   remove                DELETE FROM notes WHERE note_id=?
//
// Return values
// -------------
//   create                -> int64_t                    new note_id
//   findById               -> std::optional<NoteRecord>
//   findByUser / ...       -> std::vector<NoteRecord>
//   exists                 -> bool
//   countByUser            -> uint64_t
//   update / updatePinned /
//   remove                  -> bool                     true iff affectedRows() > 0
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// An invalid user_id surfaces as DBConstraintViolationException
// (fk_notes_user).  NoteDAO never catches or wraps these exceptions.
//
// Interaction with managers
// -------------------------
// NoteManager owns one NoteDAO and is its only caller.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class NoteDAO : public BaseDAO {
public:
    explicit NoteDAO(db::Database& database);
    ~NoteDAO() = default;

    NoteDAO(const NoteDAO&) = delete;
    NoteDAO& operator=(const NoteDAO&) = delete;
    NoteDAO(NoteDAO&&) = delete;
    NoteDAO& operator=(NoteDAO&&) = delete;

    // ---- Create ----
    [[nodiscard]] int64_t create(const NoteRecord& note);

    // ---- Read ----
    [[nodiscard]] std::optional<NoteRecord> findById(int64_t noteId);
    [[nodiscard]] std::vector<NoteRecord>   findByUser(int64_t userId,
                                                         const PageRequest& page = {});
    [[nodiscard]] std::vector<NoteRecord>   findByUserAndPinned(int64_t userId, bool pinned);
    [[nodiscard]] std::vector<NoteRecord>   findByUserAndTitle(int64_t userId,
                                                                 const std::string& titleFragment);
    [[nodiscard]] bool exists(int64_t noteId);
    [[nodiscard]] uint64_t countByUser(int64_t userId);

    // ---- Update ----
    [[nodiscard]] bool update(const NoteRecord& note);
    [[nodiscard]] bool updatePinned(int64_t noteId, bool pinned);

    // ---- Delete ----
    [[nodiscard]] bool remove(int64_t noteId);
};

} // namespace revolif::dao
