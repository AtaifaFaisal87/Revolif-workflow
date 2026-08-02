// ============================================================================
// UserDAO.cpp
//
// Implementation of all persistence operations for the `users` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "UserDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a UserRecord.  Column names match the
// revolif_schema.sql CREATE TABLE exactly.
// ---------------------------------------------------------------------------
UserRecord mapRow(const db::Row& row) {
    UserRecord rec;
    rec.userId = row.getInt64("user_id");
    rec.username = row.getString("username");
    rec.name = row.getString("name");

    if (!row.isNull("email")) {
        rec.email = row.getString("email");
    }

    rec.passwordHash = row.getString("password_hash");
    rec.role = row.getString("role");

    if (!row.isNull("date_of_birth")) {
        rec.dateOfBirth = row.getString("date_of_birth");
    }

    rec.registrationDate = row.getString("registration_date");

    if (!row.isNull("last_login")) {
        rec.lastLogin = row.getString("last_login");
    }

    if (!row.isNull("title")) {
        rec.title = row.getString("title");
    }

    rec.isActive = row.getBool("is_active");
    rec.currentStreak = static_cast<int>(row.getInt64("current_streak"));
    rec.bestStreak = static_cast<int>(row.getInt64("best_streak"));

    if (!row.isNull("displayed_achievement_id")) {
        rec.displayedAchievementId = row.getInt64("displayed_achievement_id");
    }

    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

UserDAO::UserDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t UserDAO::create(const UserRecord& user) {
    static const std::string sql =
        "INSERT INTO users ("
        "username, name, email, password_hash, role, "
        "date_of_birth, registration_date, last_login, title, "
        "is_active, current_streak, best_streak, displayed_achievement_id"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    std::vector<db::DBParam> params;
    params.reserve(13);

    params.emplace_back(user.username);
    params.emplace_back(user.name);
    params.emplace_back(user.email.has_value()
                            ? db::DBParam(*user.email)
                            : db::DBParam(nullptr));
    params.emplace_back(user.passwordHash);
    params.emplace_back(user.role);
    params.emplace_back(user.dateOfBirth.has_value()
                            ? db::DBParam(*user.dateOfBirth)
                            : db::DBParam(nullptr));
    params.emplace_back(user.registrationDate);
    params.emplace_back(user.lastLogin.has_value()
                            ? db::DBParam(*user.lastLogin)
                            : db::DBParam(nullptr));
    params.emplace_back(user.title.has_value()
                            ? db::DBParam(*user.title)
                            : db::DBParam(nullptr));
    params.emplace_back(user.isActive);
    params.emplace_back(user.currentStreak);
    params.emplace_back(user.bestStreak);
    params.emplace_back(user.displayedAchievementId.has_value()
                            ? db::DBParam(*user.displayedAchievementId)
                            : db::DBParam(nullptr));

    db_.executeUpdate(sql, params);
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<UserRecord> UserDAO::findById(int64_t userId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM users WHERE user_id = ?",
        {db::DBParam(userId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::optional<UserRecord> UserDAO::findByUsername(const std::string& username) {
    if (username.empty()) {
        throw std::invalid_argument(
            "UserDAO::findByUsername: username must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM users WHERE username = ?",
        {db::DBParam(username)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::optional<UserRecord> UserDAO::findByEmail(const std::string& email) {
    if (email.empty()) {
        throw std::invalid_argument(
            "UserDAO::findByEmail: email must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM users WHERE email = ?",
        {db::DBParam(email)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<UserRecord> UserDAO::findAll() {
    auto rs = db_.executeQuery("SELECT * FROM users");

    std::vector<UserRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<UserRecord> UserDAO::findByRole(const std::string& role) {
    auto rs = db_.executeQuery(
        "SELECT * FROM users WHERE role = ?",
        {db::DBParam(role)});

    std::vector<UserRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks
// =============================================================================

bool UserDAO::exists(int64_t userId) {
    return db_.exists(
        "SELECT 1 FROM users WHERE user_id = ? LIMIT 1",
        {db::DBParam(userId)});
}

bool UserDAO::existsByUsername(const std::string& username) {
    if (username.empty()) {
        throw std::invalid_argument(
            "UserDAO::existsByUsername: username must not be empty");
    }

    return db_.exists(
        "SELECT 1 FROM users WHERE username = ? LIMIT 1",
        {db::DBParam(username)});
}

bool UserDAO::existsByEmail(const std::string& email) {
    if (email.empty()) {
        throw std::invalid_argument(
            "UserDAO::existsByEmail: email must not be empty");
    }

    return db_.exists(
        "SELECT 1 FROM users WHERE email = ? LIMIT 1",
        {db::DBParam(email)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t UserDAO::countAll() {
    return db_.scalar<uint64_t>("SELECT COUNT(*) FROM users");
}

uint64_t UserDAO::countByRole(const std::string& role) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM users WHERE role = ?",
        {db::DBParam(role)});
}

// =============================================================================
// Update
// =============================================================================

bool UserDAO::update(const UserRecord& user) {
    static const std::string sql =
        "UPDATE users SET "
        "username = ?, name = ?, email = ?, password_hash = ?, role = ?, "
        "date_of_birth = ?, registration_date = ?, last_login = ?, "
        "title = ?, is_active = ?, current_streak = ?, best_streak = ?, "
        "displayed_achievement_id = ? "
        "WHERE user_id = ?";

    std::vector<db::DBParam> params;
    params.reserve(14);

    params.emplace_back(user.username);
    params.emplace_back(user.name);
    params.emplace_back(user.email.has_value()
                            ? db::DBParam(*user.email)
                            : db::DBParam(nullptr));
    params.emplace_back(user.passwordHash);
    params.emplace_back(user.role);
    params.emplace_back(user.dateOfBirth.has_value()
                            ? db::DBParam(*user.dateOfBirth)
                            : db::DBParam(nullptr));
    params.emplace_back(user.registrationDate);
    params.emplace_back(user.lastLogin.has_value()
                            ? db::DBParam(*user.lastLogin)
                            : db::DBParam(nullptr));
    params.emplace_back(user.title.has_value()
                            ? db::DBParam(*user.title)
                            : db::DBParam(nullptr));
    params.emplace_back(user.isActive);
    params.emplace_back(user.currentStreak);
    params.emplace_back(user.bestStreak);
    params.emplace_back(user.displayedAchievementId.has_value()
                            ? db::DBParam(*user.displayedAchievementId)
                            : db::DBParam(nullptr));
    params.emplace_back(user.userId);

    auto result = db_.executeUpdate(sql, params);
    return result.affectedRows() > 0;
}

bool UserDAO::updateLastLogin(int64_t userId,
                              const std::string& loginTimestampIso) {
    auto result = db_.executeUpdate(
        "UPDATE users SET last_login = ? WHERE user_id = ?",
        {db::DBParam(loginTimestampIso), db::DBParam(userId)});

    return result.affectedRows() > 0;
}

bool UserDAO::updateStreaks(int64_t userId,
                            int currentStreak,
                            int bestStreak) {
    auto result = db_.executeUpdate(
        "UPDATE users SET current_streak = ?, best_streak = ? WHERE user_id = ?",
        {db::DBParam(currentStreak),
         db::DBParam(bestStreak),
         db::DBParam(userId)});

    return result.affectedRows() > 0;
}

bool UserDAO::updateDisplayedAchievement(
    int64_t userId,
    std::optional<int64_t> achievementId) {

    auto result = db_.executeUpdate(
        "UPDATE users SET displayed_achievement_id = ? WHERE user_id = ?",
        {achievementId.has_value()
             ? db::DBParam(*achievementId)
             : db::DBParam(nullptr),
         db::DBParam(userId)});

    return result.affectedRows() > 0;
}

bool UserDAO::setActive(int64_t userId, bool active) {
    auto result = db_.executeUpdate(
        "UPDATE users SET is_active = ? WHERE user_id = ?",
        {db::DBParam(active), db::DBParam(userId)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool UserDAO::remove(int64_t userId) {
    auto result = db_.executeUpdate(
        "DELETE FROM users WHERE user_id = ?",
        {db::DBParam(userId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
