#pragma once
// ============================================================================
// JournalDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `journal` table.  JournalDAO does not
// interpret mood ratings, streaks, or sentiment — it only stores and
// retrieves the rows it is given.  Any analysis (average mood, entry
// frequency, etc.) is a JournalManager concern.
//
// SQL operations (conceptual)
// ---------------------------------------------------------------------------
//   create                INSERT INTO journal (...) VALUES (...)
//   findById              SELECT * FROM journal WHERE journal_id = ?
//   findByUser            SELECT * FROM journal WHERE user_id = ? ORDER BY entry_date DESC
//   findByUserAndDate     SELECT * FROM journal WHERE user_id=? AND entry_date=?
//   findByUserAndDateRange
//                         SELECT * FROM journal WHERE user_id=? AND entry_date BETWEEN ? AND ?
//   findByUserAndMood     SELECT * FROM journal WHERE user_id=? AND mood_rating=?
//   exists                SELECT EXISTS(SELECT 1 FROM journal WHERE journal_id=?)
//   countByUser           SELECT COUNT(*) FROM journal WHERE user_id=?
//   update                UPDATE journal SET <mutable columns> WHERE journal_id=?
//   remove                DELETE FROM journal WHERE journal_id=?
//
// Return values
// -------------
//   create                -> int64_t                    new journal_id
//   findById               -> std::optional<JournalRecord>
//   findByUser / ...       -> std::vector<JournalRecord>
//   exists                 -> bool
//   countByUser            -> uint64_t
//   update / remove         -> bool                     true iff affectedRows() > 0
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// An invalid user_id surfaces as DBConstraintViolationException
// (fk_journal_user).  JournalDAO never catches or wraps these exceptions.
//
// Interaction with managers
// -------------------------
// JournalManager owns one JournalDAO and is its only caller.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class JournalDAO : public BaseDAO {
public:
    explicit JournalDAO(db::Database& database);
    ~JournalDAO() = default;

    JournalDAO(const JournalDAO&) = delete;
    JournalDAO& operator=(const JournalDAO&) = delete;
    JournalDAO(JournalDAO&&) = delete;
    JournalDAO& operator=(JournalDAO&&) = delete;

    // ---- Create ----
    [[nodiscard]] int64_t create(const JournalRecord& entry);

    // ---- Read ----
    [[nodiscard]] std::optional<JournalRecord> findById(int64_t journalId);
    [[nodiscard]] std::vector<JournalRecord>   findByUser(int64_t userId,
                                                            const PageRequest& page = {});
    [[nodiscard]] std::vector<JournalRecord>   findByUserAndDate(int64_t userId,
                                                                   const std::string& entryDateIso);
    [[nodiscard]] std::vector<JournalRecord>   findByUserAndDateRange(int64_t userId,
                                                                        const std::string& startDateIso,
                                                                        const std::string& endDateIso);
    [[nodiscard]] std::vector<JournalRecord>   findByUserAndMood(int64_t userId, int moodRating);
    [[nodiscard]] bool exists(int64_t journalId);
    [[nodiscard]] uint64_t countByUser(int64_t userId);

    // ---- Update ----
    [[nodiscard]] bool update(const JournalRecord& entry);

    // ---- Delete ----
    [[nodiscard]] bool remove(int64_t journalId);
};

} // namespace revolif::dao
