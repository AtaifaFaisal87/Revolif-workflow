// ============================================================================
// CategoryDAO.cpp
//
// Implementation of all persistence operations for the `categories` table.
// Every query uses prepared statements via the Database wrapper.
// Database exceptions propagate unchanged; the only exceptions thrown
// directly by this file are std::invalid_argument for empty lookup keys.
// ============================================================================

#include "CategoryDAO.h"

#include <stdexcept>

namespace revolif::dao {

namespace {

// ---------------------------------------------------------------------------
// Maps a single database row to a CategoryRecord.
// ---------------------------------------------------------------------------
CategoryRecord mapRow(const db::Row& row) {
    CategoryRecord rec;
    rec.categoryId = row.getInt64("category_id");
    rec.name = row.getString("name");
    rec.categoryType = row.getString("category_type");
    rec.isDefault = row.getBool("is_default");
    rec.createdAt = row.getString("created_at");
    return rec;
}

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

CategoryDAO::CategoryDAO(db::Database& database) : BaseDAO(database) {}

// =============================================================================
// Create
// =============================================================================

int64_t CategoryDAO::create(const CategoryRecord& category) {
    static const std::string sql =
        "INSERT INTO categories (name, category_type, is_default) "
        "VALUES (?, ?, ?)";

    db_.executeUpdate(sql, {
        db::DBParam(category.name),
        db::DBParam(category.categoryType),
        db::DBParam(category.isDefault)
    });
    return db_.lastInsertId();
}

// =============================================================================
// Read
// =============================================================================

std::optional<CategoryRecord> CategoryDAO::findById(int64_t categoryId) {
    auto rs = db_.executeQuery(
        "SELECT * FROM categories WHERE category_id = ?",
        {db::DBParam(categoryId)});

    if (rs.empty()) {
        return std::nullopt;
    }
    return mapRow(rs.at(0));
}

std::vector<CategoryRecord> CategoryDAO::findByType(const std::string& categoryType) {
    auto rs = db_.executeQuery(
        "SELECT * FROM categories WHERE category_type = ? ORDER BY name ASC",
        {db::DBParam(categoryType)});

    std::vector<CategoryRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<CategoryRecord> CategoryDAO::findByName(const std::string& nameFragment) {
    if (nameFragment.empty()) {
        throw std::invalid_argument(
            "CategoryDAO::findByName: nameFragment must not be empty");
    }

    auto rs = db_.executeQuery(
        "SELECT * FROM categories WHERE name LIKE CONCAT('%', ?, '%') ORDER BY name ASC",
        {db::DBParam(nameFragment)});

    std::vector<CategoryRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<CategoryRecord> CategoryDAO::findAll() {
    auto rs = db_.executeQuery("SELECT * FROM categories ORDER BY name ASC");

    std::vector<CategoryRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

std::vector<CategoryRecord> CategoryDAO::findDefaults() {
    auto rs = db_.executeQuery(
        "SELECT * FROM categories WHERE is_default = TRUE ORDER BY name ASC");

    std::vector<CategoryRecord> results;
    results.reserve(rs.rowCount());

    for (size_t i = 0; i < rs.rowCount(); ++i) {
        results.push_back(mapRow(rs.at(i)));
    }
    return results;
}

// =============================================================================
// Existence checks
// =============================================================================

bool CategoryDAO::exists(int64_t categoryId) {
    return db_.exists(
        "SELECT 1 FROM categories WHERE category_id = ? LIMIT 1",
        {db::DBParam(categoryId)});
}

bool CategoryDAO::existsByName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument(
            "CategoryDAO::existsByName: name must not be empty");
    }

    return db_.exists(
        "SELECT 1 FROM categories WHERE name = ? LIMIT 1",
        {db::DBParam(name)});
}

// =============================================================================
// Counts
// =============================================================================

uint64_t CategoryDAO::countByType(const std::string& categoryType) {
    return db_.scalar<uint64_t>(
        "SELECT COUNT(*) FROM categories WHERE category_type = ?",
        {db::DBParam(categoryType)});
}

// =============================================================================
// Update
// =============================================================================

bool CategoryDAO::update(const CategoryRecord& category) {
    static const std::string sql =
        "UPDATE categories SET name = ?, category_type = ?, is_default = ? "
        "WHERE category_id = ?";

    auto result = db_.executeUpdate(sql, {
        db::DBParam(category.name),
        db::DBParam(category.categoryType),
        db::DBParam(category.isDefault),
        db::DBParam(category.categoryId)
    });

    return result.affectedRows() > 0;
}

// =============================================================================
// Delete
// =============================================================================

bool CategoryDAO::remove(int64_t categoryId) {
    auto result = db_.executeUpdate(
        "DELETE FROM categories WHERE category_id = ?",
        {db::DBParam(categoryId)});

    return result.affectedRows() > 0;
}

} // namespace revolif::dao
