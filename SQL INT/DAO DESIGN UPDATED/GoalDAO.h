#pragma once
// ============================================================================
// GoalDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `goals` table. GoalDAO does not decide
// *when* a goal counts as completed, and it does not compute completed_at
// itself — GoalManager passes the timestamp it wants written. GoalDAO also
// does not decide achievement eligibility; it only exposes the counts
// (countCompletedByUser) that GoalManager/AchievementManager compare
// against achievement thresholds.
//
// SQL operations (conceptual)
// ---------------------------------------------------------------------------
//   create                  INSERT INTO goals (...) VALUES (...)
//   findById                SELECT * FROM goals WHERE goal_id = ?
//   findByUser               SELECT * FROM goals WHERE user_id = ? ORDER BY deadline
//   findByUserAndStatus      SELECT * FROM goals WHERE user_id=? AND status=?
//   findByUserAndCategory    SELECT * FROM goals WHERE user_id=? AND category_id=?
//   findByUserAndTitle       SELECT * FROM goals WHERE user_id=? AND title LIKE CONCAT('%',?,'%')
//   findByUserAndDeadline    SELECT * FROM goals WHERE user_id=? AND DATE(deadline)=?
//   exists                   SELECT EXISTS(SELECT 1 FROM goals WHERE goal_id=?)
//   countByUser              SELECT COUNT(*) FROM goals WHERE user_id=?
//   countByUserAndStatus     SELECT COUNT(*) FROM goals WHERE user_id=? AND status=?
//   countByUserAndCategory   SELECT COUNT(*) FROM goals WHERE user_id=? AND category_id=?
//   countCompletedByUser     SELECT COUNT(*) FROM goals WHERE user_id=? AND status='COMPLETED'
//   update                   UPDATE goals SET <mutable columns> WHERE goal_id=?
//   updateStatus             UPDATE goals SET status=?, completed_at=? WHERE goal_id=?
//   remove                   DELETE FROM goals WHERE goal_id=?
//
// Return values
// -------------
//   create                -> int64_t                  new goal_id
//   findById               -> std::optional<GoalRecord>
//   findByUser / ...       -> std::vector<GoalRecord>
//   exists                 -> bool
//   countByUser / ...      -> uint64_t
//   update / updateStatus /
//   remove                 -> bool                     true iff affectedRows() > 0
//   countCompletedByUser    -> uint64_t
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// An invalid category_id surfaces as DBConstraintViolationException
// (fk_goals_category); GoalManager should pre-validate against
// CategoryDAO. GoalDAO never catches or wraps these exceptions.
//
// Interaction with managers
// -------------------------
// GoalManager owns one GoalDAO. On goal completion, GoalManager:
//   1. Computes the completedAt timestamp itself (a business/time concern).
//   2. Calls GoalDAO::updateStatus(goalId, "COMPLETED", completedAt).
//   3. Calls GoalDAO::countCompletedByUser(userId) and compares the result
//      against AchievementDAO::findAll() thresholds to decide whether an
//      achievement unlocks.
//   4. If one does, calls AchievementDAO::unlockForUser and
//      UserDAO::updateStreaks/updateDisplayedAchievement.
// Steps 2-4 touch three different tables and are wrapped by GoalManager in
// a single db::TransactionGuard; no DAO in that chain calls another DAO or
// manages the transaction itself.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class GoalDAO : public BaseDAO {
public:
    explicit GoalDAO(db::Database& database);
    ~GoalDAO() = default;

    GoalDAO(const GoalDAO&) = delete;
    GoalDAO& operator=(const GoalDAO&) = delete;
    GoalDAO(GoalDAO&&) = delete;
    GoalDAO& operator=(GoalDAO&&) = delete;

    // ---- Create ----
    [[nodiscard]] int64_t create(const GoalRecord& goal);

    // ---- Read ----
    [[nodiscard]] std::optional<GoalRecord> findById(int64_t goalId);
    [[nodiscard]] std::vector<GoalRecord>   findByUser(int64_t userId, const PageRequest& page = {});
    [[nodiscard]] std::vector<GoalRecord>   findByUserAndStatus(int64_t userId, const std::string& status);
    [[nodiscard]] std::vector<GoalRecord>   findByUserAndCategory(int64_t userId, int64_t categoryId);
    [[nodiscard]] std::vector<GoalRecord>   findByUserAndTitle(int64_t userId, const std::string& titleFragment);
    [[nodiscard]] std::vector<GoalRecord>   findByUserAndDeadline(int64_t userId, const std::string& deadlineDateIso);
    [[nodiscard]] bool exists(int64_t goalId);
    [[nodiscard]] uint64_t countByUser(int64_t userId);
    [[nodiscard]] uint64_t countByUserAndStatus(int64_t userId, const std::string& status);
    [[nodiscard]] uint64_t countByUserAndCategory(int64_t userId, int64_t categoryId);
    [[nodiscard]] uint64_t countCompletedByUser(int64_t userId);

    // ---- Update ----
    [[nodiscard]] bool update(const GoalRecord& goal);
    [[nodiscard]] bool updateStatus(int64_t goalId, const std::string& status,
                       std::optional<std::string> completedAtIso);

    // ---- Delete ----
    [[nodiscard]] bool remove(int64_t goalId);
};

} // namespace revolif::dao
