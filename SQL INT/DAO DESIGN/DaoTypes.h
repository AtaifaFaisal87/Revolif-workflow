#pragma once
// ============================================================================
// DaoTypes.h
//
// Plain data-transfer objects (DTOs) that mirror the finalized MySQL schema,
// one struct per table. These are the ONLY objects that cross the boundary
// between the DAO layer and the Manager layer.
//
// Design rules:
//  - A record struct has data members only: no constructors beyond
//    aggregate-init, no methods, no validation, no formatting.
//  - Column types map onto C++ types as follows:
//      INT/BIGINT UNSIGNED PK/FK   -> int64_t
//      VARCHAR / TEXT / ENUM       -> std::string   (ENUM values travel as
//                                     their literal string, e.g. "PENDING";
//                                     parsing into a C++ enum class is a
//                                     Manager-layer concern, not a DAO one)
//      DECIMAL(10,2)               -> std::string   (see architecture notes:
//                                     kept as exact decimal text to avoid
//                                     floating-point rounding of money;
//                                     Managers use a Money/decimal helper)
//      BOOLEAN                     -> bool
//      DATETIME / DATE             -> std::string   (ISO-8601, e.g.
//                                     "2026-07-29 10:15:00"); Managers convert
//                                     to/from the existing Date/Time classes
//      NULLable column             -> std::optional<T>
//
// Nothing in this file is an "implementation" of behavior: it is the shape
// of the data the DAO layer reads and writes.
// ============================================================================

#include <cstdint>
#include <optional>
#include <string>

namespace revolif::dao {

// ---------------------------------------------------------------------------
// categories  (referenced by tasks/goals/expenses/category_budgets; see the
// "Missing CategoryDAO" note in the architecture review)
// ---------------------------------------------------------------------------
struct CategoryRecord {
    int64_t     categoryId;
    std::string name;
    std::string categoryType;   // "TASK" | "GOAL" | "EXPENSE"
    bool        isDefault;
    std::string createdAt;
};

// ---------------------------------------------------------------------------
// users
// ---------------------------------------------------------------------------
struct UserRecord {
    int64_t                    userId;
    std::string                username;
    std::string                name;
    std::optional<std::string> email;
    std::string                passwordHash;
    std::string                role;               // "USER" | "ADMIN"
    std::optional<std::string> dateOfBirth;
    std::string                registrationDate;
    std::optional<std::string> lastLogin;
    std::optional<std::string> title;
    bool                        isActive;
    int                         currentStreak;
    int                         bestStreak;
    std::optional<int64_t>     displayedAchievementId;
};

// ---------------------------------------------------------------------------
// tasks
// ---------------------------------------------------------------------------
struct TaskRecord {
    int64_t                    taskId;
    int64_t                    userId;
    int64_t                    categoryId;
    std::string                taskType;            // "ACADEMIC" | "DAILY"
    std::string                title;
    std::optional<std::string> description;
    std::string                deadline;
    std::string                status;               // "PENDING" | "COMPLETED"
    std::string                priority;             // "LOW" | "MEDIUM" | "HIGH"
    bool                        isRecurring;
    std::string                recurrenceInterval;   // "NONE"|"DAILY"|"WEEKLY"|"MONTHLY"
    std::string                createdAt;
};

// ---------------------------------------------------------------------------
// goals
// ---------------------------------------------------------------------------
struct GoalRecord {
    int64_t                    goalId;
    int64_t                    userId;
    int64_t                    categoryId;
    std::string                title;
    std::optional<std::string> description;
    std::string                deadline;
    std::string                status;              // "INCOMPLETE" | "COMPLETED"
    std::string                createdAt;
    std::optional<std::string> completedAt;
};

// ---------------------------------------------------------------------------
// expenses
// ---------------------------------------------------------------------------
struct ExpenseRecord {
    int64_t                    expenseId;
    int64_t                    userId;
    int64_t                    categoryId;
    std::string                title;
    std::string                amount;              // DECIMAL(10,2) as text
    std::string                expenseDate;
    std::optional<std::string> description;
    std::string                createdAt;
};

// ---------------------------------------------------------------------------
// category_budgets
// ---------------------------------------------------------------------------
struct CategoryBudgetRecord {
    int64_t     budgetId;
    int64_t     userId;
    int64_t     categoryId;
    std::string monthlyLimit;   // DECIMAL(10,2) as text
    std::string createdAt;
    std::string updatedAt;
};

// ---------------------------------------------------------------------------
// habits
// ---------------------------------------------------------------------------
struct HabitRecord {
    int64_t                    habitId;
    int64_t                    userId;
    std::string                name;
    std::optional<std::string> description;
    std::string                frequency;   // "DAILY" | "WEEKLY" | "MONTHLY"
    bool                        isActive;
    std::string                createdAt;
};

// ---------------------------------------------------------------------------
// habit_history  (append-only completion log)
// ---------------------------------------------------------------------------
struct HabitHistoryRecord {
    int64_t     historyId;
    int64_t     habitId;
    std::string completionDatetime;
    std::string status;   // "COMPLETED" | "MISSED"
};

// ---------------------------------------------------------------------------
// journal
// ---------------------------------------------------------------------------
struct JournalRecord {
    int64_t                 journalId;
    int64_t                 userId;
    std::string             entryDate;
    std::string             content;
    std::optional<int>      moodRating;   // 1-5, nullable
    std::string             createdAt;
};

// ---------------------------------------------------------------------------
// notes
// ---------------------------------------------------------------------------
struct NoteRecord {
    int64_t     noteId;
    int64_t     userId;
    std::string title;
    std::string content;
    bool        isPinned;
    std::string createdAt;
    std::string updatedAt;
};

// ---------------------------------------------------------------------------
// achievements  (global catalog)
// ---------------------------------------------------------------------------
struct AchievementRecord {
    int64_t     achievementId;
    std::string name;
    std::string description;
    int         requiredGoals;
    bool        isDefault;
    std::string createdAt;
};

// ---------------------------------------------------------------------------
// user_achievements  (bridge table)
// ---------------------------------------------------------------------------
struct UserAchievementRecord {
    int64_t     userId;
    int64_t     achievementId;
    std::string unlockedAt;
};

// ---------------------------------------------------------------------------
// Lightweight paging spec, offered as an optional parameter on list-style
// finder methods that could return unbounded row counts (tasks, goals,
// expenses, journal, notes). See architecture review, item "Pagination".
// ---------------------------------------------------------------------------
struct PageRequest {
    unsigned limit  = 50;
    unsigned offset = 0;
};

} // namespace revolif::dao
