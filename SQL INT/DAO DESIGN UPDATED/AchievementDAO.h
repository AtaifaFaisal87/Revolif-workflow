#pragma once
// ============================================================================
// AchievementDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `achievements` table (global catalog)
// and the bridge table `user_achievements`.  AchievementDAO treats
// user_achievements as an integral part of the achievement lifecycle:
// unlocks, per-user lookups, and counts are all exposed here so that
// AchievementManager does not need a second DAO.
//
// AchievementDAO does not decide *when* an achievement is unlocked — it
// only writes the unlock row it is told to write.  Eligibility checks
// (e.g. "has the user completed enough goals?") are a Manager concern.
//
// SQL operations (conceptual) — achievements
// ---------------------------------------------------------------------------
//   create                INSERT INTO achievements (...) VALUES (...)
//   findById              SELECT * FROM achievements WHERE achievement_id = ?
//   findAll               SELECT * FROM achievements ORDER BY required_goals
//   findByDefault         SELECT * FROM achievements WHERE is_default = ?
//   exists                SELECT EXISTS(SELECT 1 FROM achievements WHERE achievement_id=?)
//   countAll              SELECT COUNT(*) FROM achievements
//   update                UPDATE achievements SET <mutable columns> WHERE achievement_id=?
//   remove                DELETE FROM achievements WHERE achievement_id=?
//
// SQL operations (conceptual) — user_achievements
// ---------------------------------------------------------------------------
//   unlockForUser         INSERT INTO user_achievements (...) VALUES (...)
//   findByUser            SELECT * FROM user_achievements WHERE user_id = ?
//   findByUserAndAchievement
//                         SELECT * FROM user_achievements WHERE user_id=? AND achievement_id=?
//   hasAchievement        SELECT EXISTS(SELECT 1 FROM user_achievements
//                         WHERE user_id=? AND achievement_id=?)
//   countByUser           SELECT COUNT(*) FROM user_achievements WHERE user_id=?
//   removeForUser         DELETE FROM user_achievements WHERE user_id=? AND achievement_id=?
//
// Return values
// -------------
//   create(...)           -> int64_t                    new achievement_id
//   findById               -> std::optional<AchievementRecord>
//   findAll / findByDefault -> std::vector<AchievementRecord>
//   exists                 -> bool
//   countAll               -> uint64_t
//   update / remove        -> bool                     true iff affectedRows() > 0
//   unlockForUser          -> bool                     true iff inserted
//   findByUser             -> std::vector<UserAchievementRecord>
//   findByUserAndAchievement -> std::optional<UserAchievementRecord>
//   hasAchievement         -> bool
//   countByUser            -> uint64_t
//   removeForUser          -> bool                     true iff affectedRows() > 0
//
// Error handling
// --------------
// Standard contract: DatabaseException subtypes propagate unchanged.
// Attempting to unlock the same achievement twice surfaces as
// DBConstraintViolationException (primary-key violation on
// pk_user_achievements); AchievementManager should call hasAchievement
// first, or catch the exception and treat it as an idempotent no-op.
// Removing a default achievement that is already unlocked by users will
// cascade via the foreign key (fk_user_achievements_achievement) depending
// on the schema's ON DELETE policy; AchievementDAO does not enforce
// policy — it only executes the SQL.
//
// Interaction with managers
// -------------------------
// AchievementManager owns one AchievementDAO.  On goal completion,
// GoalManager (or AchievementManager) reads the requiredGoals thresholds
// from AchievementDAO::findAll(), compares them against
// GoalDAO::countCompletedByUser(userId), and if a threshold is met calls
// AchievementDAO::unlockForUser(...).  If an achievement is unlocked,
// GoalManager/AchievementManager may then call
// UserDAO::updateDisplayedAchievement inside the same
// db::TransactionGuard.  No DAO in that chain calls another DAO.
// ============================================================================

#include "BaseDAO.h"
#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class AchievementDAO : public BaseDAO {
public:
    explicit AchievementDAO(db::Database& database);
    ~AchievementDAO() = default;

    AchievementDAO(const AchievementDAO&) = delete;
    AchievementDAO& operator=(const AchievementDAO&) = delete;
    AchievementDAO(AchievementDAO&&) = delete;
    AchievementDAO& operator=(AchievementDAO&&) = delete;

    // ---- Create (achievements) ----
    [[nodiscard]] int64_t create(const AchievementRecord& achievement);

    // ---- Read (achievements) ----
    [[nodiscard]] std::optional<AchievementRecord> findById(int64_t achievementId);
    [[nodiscard]] std::vector<AchievementRecord>   findAll();
    [[nodiscard]] std::vector<AchievementRecord>   findByDefault(bool isDefault);
    [[nodiscard]] bool exists(int64_t achievementId);
    [[nodiscard]] uint64_t countAll();

    // ---- Update (achievements) ----
    [[nodiscard]] bool update(const AchievementRecord& achievement);

    // ---- Delete (achievements) ----
    [[nodiscard]] bool remove(int64_t achievementId);

    // ---- Create (user_achievements) ----
    [[nodiscard]] bool unlockForUser(int64_t userId,
                                      int64_t achievementId,
                                      const std::string& unlockedAtIso);

    // ---- Read (user_achievements) ----
    [[nodiscard]] std::vector<UserAchievementRecord> findByUser(int64_t userId);
    [[nodiscard]] std::optional<UserAchievementRecord> findByUserAndAchievement(int64_t userId,
                                                                                   int64_t achievementId);
    [[nodiscard]] bool hasAchievement(int64_t userId, int64_t achievementId);
    [[nodiscard]] uint64_t countByUser(int64_t userId);

    // ---- Delete (user_achievements) ----
    [[nodiscard]] bool removeForUser(int64_t userId, int64_t achievementId);
};

} // namespace revolif::dao
