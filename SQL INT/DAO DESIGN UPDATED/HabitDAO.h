#pragma once
// ============================================================================
// HabitDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `habits` table and its append-only
// child table `habit_history`.  HabitDAO treats habit_history as an
// integral part of the habit lifecycle: completions and misses are logged
// through HabitDAO, and aggregate queries (streaks, completion counts) are
// offered here so that HabitManager does not need a separate DAO.
//
// HabitDAO does not decide what "streak" means operationally — it only
// exposes the raw history rows and simple counts.  Streak calculation
// (consecutive days, grace periods, etc.) is a Manager-layer concern.
//
// SQL operations (conceptual) — habits
// ---------------------------------------------------------------------------
//   create                INSERT INTO habits (...) VALUES (...)
//   findById              SELECT * FROM habits WHERE habit_id = ?
//   findByUser            SELECT * FROM habits WHERE user_id = ? ORDER BY created_at DESC
//   findByUserAndActive   SELECT * FROM habits WHERE user_id=? AND is_active=?
//   findByUserAndFrequency SELECT * FROM habits WHERE user_id=? AND frequency=?
//   exists                SELECT EXISTS(SELECT 1 FROM habits WHERE habit_id=?)
//   countByUser           SELECT COUNT(*) FROM habits WHERE user_id=?
//   countByUserAndActive  SELECT COUNT(*) FROM habits WHERE user_id=? AND is_active=?
//   update                UPDATE habits SET <mutable columns> WHERE habit_id=?
//   updateActive          UPDATE habits SET is_active=? WHERE habit_id=?
//   remove                DELETE FROM habits WHERE habit_id=?
//
// SQL operations (conceptual) — habit_history
// ---------------------------------------------------------------------------
//   createHistory         INSERT INTO habit_history (...) VALUES (...)
//   findHistoryByHabit    SELECT * FROM habit_history WHERE habit_id=? ORDER BY completion_datetime DESC
//   findHistoryByHabitAndDateRange
//                         SELECT * FROM habit_history WHERE habit_id=? AND
//                         completion_datetime BETWEEN ? AND ?
//   findHistoryByHabitAndStatus
//                         SELECT * FROM habit_history WHERE habit_id=? AND status=?
//   countCompletedByHabit SELECT COUNT(*) FROM habit_history WHERE habit_id=? AND status='COMPLETED'
//   countMissedByHabit    SELECT COUNT(*) FROM habit_history WHERE habit_id=? AND status='MISSED'
//   removeHistory         DELETE FROM habit_history WHERE history_id=?
//
// Return values
// -------------
//   create(...)           -> int64_t                    new habit_id
//   createHistory(...)    -> int64_t                    new history_id
//   findById               -> std::optional<HabitRecord>
//   findByUser / ...       -> std::vector<HabitRecord>
//   findHistoryByHabit / ... -> std::vector<HabitHistoryRecord>
//   exists                 -> bool
//   countXxx               -> uint64_t
//   update / updateActive /
//   remove / removeHistory -> bool                     true iff affectedRows() > 0
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// Inserting a habit_history row with an invalid habit_id surfaces as
// DBConstraintViolationException (fk_habit_history_habit).  HabitDAO
// never catches or wraps these exceptions.
//
// Interaction with managers
// -------------------------
// HabitManager owns one HabitDAO and is its only caller.  When a user
// marks a habit as completed, HabitManager calls createHistory(...) and
// may then call countCompletedByHabit(...) to decide whether an
// achievement or streak update is warranted.  HabitManager is also
// responsible for deactivating a habit (updateActive) when the user
// pauses it; HabitDAO only writes the flag it is given.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class HabitDAO : public BaseDAO {
public:
    explicit HabitDAO(db::Database& database);
    ~HabitDAO() = default;

    HabitDAO(const HabitDAO&) = delete;
    HabitDAO& operator=(const HabitDAO&) = delete;
    HabitDAO(HabitDAO&&) = delete;
    HabitDAO& operator=(HabitDAO&&) = delete;

    // ---- Create (habits) ----
    [[nodiscard]] int64_t create(const HabitRecord& habit);

    // ---- Read (habits) ----
    [[nodiscard]] std::optional<HabitRecord> findById(int64_t habitId);
    [[nodiscard]] std::vector<HabitRecord>   findByUser(int64_t userId, const PageRequest& page = {});
    [[nodiscard]] std::vector<HabitRecord>   findByUserAndActive(int64_t userId, bool active);
    [[nodiscard]] std::vector<HabitRecord>   findByUserAndFrequency(int64_t userId,
                                                                      const std::string& frequency);
    [[nodiscard]] bool exists(int64_t habitId);
    [[nodiscard]] uint64_t countByUser(int64_t userId);
    [[nodiscard]] uint64_t countByUserAndActive(int64_t userId, bool active);

    // ---- Update (habits) ----
    [[nodiscard]] bool update(const HabitRecord& habit);
    [[nodiscard]] bool updateActive(int64_t habitId, bool active);

    // ---- Delete (habits) ----
    [[nodiscard]] bool remove(int64_t habitId);

    // ---- Create (habit_history) ----
    [[nodiscard]] int64_t createHistory(const HabitHistoryRecord& entry);

    // ---- Read (habit_history) ----
    [[nodiscard]] std::vector<HabitHistoryRecord> findHistoryByHabit(int64_t habitId,
                                                                       const PageRequest& page = {});
    [[nodiscard]] std::vector<HabitHistoryRecord> findHistoryByHabitAndDateRange(int64_t habitId,
                                                                                   const std::string& startIso,
                                                                                   const std::string& endIso);
    [[nodiscard]] std::vector<HabitHistoryRecord> findHistoryByHabitAndStatus(int64_t habitId,
                                                                                const std::string& status);
    [[nodiscard]] uint64_t countCompletedByHabit(int64_t habitId);
    [[nodiscard]] uint64_t countMissedByHabit(int64_t habitId);

    // ---- Delete (habit_history) ----
    [[nodiscard]] bool removeHistory(int64_t historyId);
};

} // namespace revolif::dao
