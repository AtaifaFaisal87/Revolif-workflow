// ============================================================================
// AchievementDAO.cpp
//
// Implementation of all persistence operations for the `achievements`
// (global catalog) and `user_achievements` (bridge) tables.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "AchievementDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to an AchievementRecord.
// ---------------------------------------------------------------------------
AchievementRecord mapAchievementRow(const db::Row& row) {
    AchievementRecord rec;
    rec.achievementId = row.getInt64("achievement_id");
    rec.name = row.getString("name");
    rec.description = row.getString("description");
    rec.requiredGoals = static_cast<int>(row.getInt64("required_goals"));
    rec.isDefault = row.getBool("is_default");
    rec.createdAt = row.getString("created_at");
    return rec;
}

// ---------------------------------------------------------------------------
// Maps a single database row to a UserAchievementRecord.
// ---------------------------------------------------------------------------
UserAchievementRecord mapUserAchievementRow(const db::Row& row) {
    UserAchievementRecord rec;
    rec.userId = row.getInt64("user_id");
    rec.achievementId = row.getInt64("achievement_id");
    rec.unlockedAt = row.getString("unlocked_at");
    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

AchievementDAO::AchievementDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create (achievements)
// =============================================================================

int64_t AchievementDAO::create(const AchievementRecord& achievement) {
    static const std::string sql =
        "INSERT INTO achievements (name, description, required_goals, is_default) "
        "VALUES (?, ?, ?, ?)";

    db_.executeUpdate(sql, {
        db::DBParam(achievement.name),
        db::DBParam(achievement.description),
        db::DBParam(static_cast<int64_t>(achievement.requiredGoals)),
        db::DBParam(achievement.isDefault)
    });
    return db_.lastInsertId();
}

// =============================================================================
// Read (achievements)
// =============================================================================

std::optional<AchievementRecord> AchievementDAO::findById(int64_t achievementId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM achievements WHERE achievement_id = ?",
        {db::DBParam(achievementId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapAchievementRow(rs.at(0));
}

std::vector<AchievementRecord> AchievementDAO::findAll() {
    auto rs = db_.executeQuery("SELECT * FROM achievements ORDER BY required_goals ASC");

    std::vector<AchievementRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapAchievementRow(rs.at(i)));
    }
    return results;
}

std::vector<AchievementRecord> AchievementDAO::findByDefault(bool isDefault) {
    auto rs = db_.executeQuery(
        "SELECT * FROM achievements WHERE is_default = ? ORDER BY required_goals ASC",
        {db::DBParam(isDefault)});

    std::vector<AchievementRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapAchievementRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks (achievements)
// =============================================================================

bool AchievementDAO::exists(int64_t achievementId) {
    return db_.exists(
        "SELECT 1 FROM achievements WHERE achievement_id = ? LIMIT 1",
        {db::DBParam(achievementId)});
}

// =============================================================================
// Counts (achievements)
// =============================================================================

uint64_t AchievementDAO::countAll() {
    return db_.scalar<uint64_t>("SELECT COUNT(*) FROM achievements");
}

// =============================================================================
// Update (achievements)
// =============================================================================

bool AchievementDAO::update(const AchievementRecord& achievement) {
    static const std::string sql =
        "UPDATE achievements SET name = ?, description = ?, required_goals = ?, is_default = ? "
        "WHERE achievement_id = ?";

    auto result = db_.executeUpdate(sql, {
        db::DBParam(achievement.name),
        db::DBParam(achievement.description),
        db::DBParam(static_cast<int64_t>(achievement.requiredGoals)),
        db::DBParam(achievement.isDefault),
        db::DBParam(achievement.achievementId)
    });

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete (achievements)
// =============================================================================

bool AchievementDAO::remove(int64_t achievementId) {
    auto result = db_.executeUpdate(
        "DELETE FROM achievements WHERE achievement_id = ?",
        {db::DBParam(achievementId)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Create (user_achievements)
// =============================================================================

bool AchievementDAO::unlockForUser(int64_t userId,
                                    int64_t achievementId,
                                    const std::string& unlockedAtIso) {
    if (unlockedAtIso.empty()) {
        throw std::invalid_argument(
            "AchievementDAO::unlockForUser: unlockedAtIso must not be empty");
    }

    auto result = db_.executeUpdate(
        "INSERT INTO user_achievements (user_id, achievement_id, unlocked_at) VALUES (?, ?, ?)",
        {db::DBParam(userId),
         db::DBParam(achievementId),
         db::DBParam(unlockedAtIso)});

    return result.affectedRows() > 0;
}

// =============================================================================
// Read (user_achievements)
// =============================================================================

std::vector<UserAchievementRecord> AchievementDAO::findByUser(int64_t userId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM user_achievements WHERE user_id = ?",
        {db::DBParam(userId)});

    std::vector<UserAchievementRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapUserAchievementRow(rs.at(i)));
    }
    return results;
}

std::optional<UserAchievementRecord> AchievementDAO::findByUserAndAchievement(
    int64_t userId,
    int64_t achievementId) {

    auto rs = db_.executeQuery(
        "SELECT * FROM user_achievements WHERE user_id = ? AND achievement_id = ?",
        {db::DBParam(userId), db::DBParam(achievementId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapUserAchievementRow(rs.at(0));
}

bool AchievementDAO::hasAchievement(int64_t userId, int64_t achievementId) {
    return db_.exists(
        "SELECT 1 FROM user_achievements WHERE user_id = ? AND achievement_id = ? LIMIT 1",
        {db::DBParam(userId), db::DBParam(achievementId)});
}

// =============================================================================
// Counts (user_achievements)
// =============================================================================

uint64_t AchievementDAO::countByUser(int64_t userId) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM user_achievements WHERE user_id = ?",
        {db::DBParam(userId)});
}

// =============================================================================
// Delete (user_achievements)
// =============================================================================

bool AchievementDAO::removeForUser(int64_t userId, int64_t achievementId) {
    auto result = db_.executeUpdate(
        "DELETE FROM user_achievements WHERE user_id = ? AND achievement_id = ?",
        {db::DBParam(userId), db::DBParam(achievementId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
