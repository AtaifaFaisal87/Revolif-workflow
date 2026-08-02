#pragma once
// ============================================================================
// UserDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `users` table. Nothing outside this
// class is permitted to build SQL against `users`. UserDAO does not know
// about passwords being "correct", streak "rules", or achievement-unlock
// policy — it only stores and retrieves the columns it is given. All of
// that judgement belongs to the Manager/Authentication layer above it.
//
// SQL operations (conceptual — see .cpp, not written here)
// ---------------------------------------------------------------------------
//   create                    INSERT INTO users (...) VALUES (...)
//   findById                  SELECT * FROM users WHERE user_id = ?
//   findByUsername            SELECT * FROM users WHERE username = ?
//   findByEmail               SELECT * FROM users WHERE email = ?
//   findAll / findByRole      SELECT * FROM users [WHERE role = ?]
//   update                    UPDATE users SET <all mutable columns> WHERE user_id = ?
//   updateLastLogin           UPDATE users SET last_login = ? WHERE user_id = ?
//   updateStreaks             UPDATE users SET current_streak=?, best_streak=? WHERE user_id=?
//   updateDisplayedAchievement UPDATE users SET displayed_achievement_id = ? WHERE user_id = ?
//   setActive                 UPDATE users SET is_active = ? WHERE user_id = ?
//   remove                    DELETE FROM users WHERE user_id = ?
//   existsByUsername/Email    SELECT EXISTS(SELECT 1 FROM users WHERE ...)
//
// Return values
// -------------
//   create(...)          -> int64_t                    new user_id (lastInsertId)
//   findXxx(...)          -> std::optional<UserRecord>  nullopt when not found
//   findAll/findByRole    -> std::vector<UserRecord>    empty vector when none
//   update / setActive /
//   updateLastLogin /
//   updateStreaks /
//   updateDisplayedAchievement /
//   remove                -> bool                       true iff affectedRows() > 0
//   existsByUsername/Email -> bool
//
// Error handling
// --------------
// UserDAO performs no translation of persistence errors: any exception
// thrown by Database (DBConnectionException, DBQueryException,
// DBConstraintViolationException, DBTransactionException,
// DBTimeoutException) propagates to the caller unchanged. This matters in
// particular for `create`, where a duplicate username/email is reported as
// DBConstraintViolationException (unique-key violation on uq_users_username
// / uq_users_email) — the caller (Authentication/UserManager) is expected
// to catch that specific category and turn it into a user-facing message
// (e.g. via the existing UserException type). UserDAO itself never throws
// or references UserException/ValidationException/AuthException; those are
// Manager-layer concepts.
// The only exception UserDAO may raise itself is std::invalid_argument,
// and only for a plain programming-contract violation (e.g. an empty
// username passed to findByUsername) — never for anything requiring
// business judgement.
//
// Interaction with managers
// -------------------------
// A single UserDAO instance is owned by the Authentication/System
// composition root and shared (by reference) with any Manager that needs
// user data (TaskManager, GoalManager, ExpenseManager, HabitManager, etc.,
// for resolving a user_id). UserDAO never calls another DAO and never
// begins/commits a transaction itself — when a Manager operation must
// update `users` atomically alongside another table (e.g. unlocking an
// achievement together with updateDisplayedAchievement), the Manager wraps
// both DAO calls in a single db::TransactionGuard.
// ============================================================================

#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class UserDAO {
public:
    explicit UserDAO(db::Database& database);
    ~UserDAO() = default;

    UserDAO(const UserDAO&) = delete;
    UserDAO& operator=(const UserDAO&) = delete;
    UserDAO(UserDAO&&) = delete;
    UserDAO& operator=(UserDAO&&) = delete;

    // ---- Create ----
    int64_t create(const UserRecord& user);

    // ---- Read ----
    std::optional<UserRecord> findById(int64_t userId);
    std::optional<UserRecord> findByUsername(const std::string& username);
    std::optional<UserRecord> findByEmail(const std::string& email);
    std::vector<UserRecord>   findAll();
    std::vector<UserRecord>   findByRole(const std::string& role);
    bool existsByUsername(const std::string& username);
    bool existsByEmail(const std::string& email);

    // ---- Update ----
    bool update(const UserRecord& user);
    bool updateLastLogin(int64_t userId, const std::string& loginTimestampIso);
    bool updateStreaks(int64_t userId, int currentStreak, int bestStreak);
    bool updateDisplayedAchievement(int64_t userId,
                                     std::optional<int64_t> achievementId);
    bool setActive(int64_t userId, bool active);

    // ---- Delete ----
    bool remove(int64_t userId);

private:
    db::Database& db_;
};

} // namespace revolif::dao
