#pragma once
// ============================================================================
// TaskDAO.h
//
// Responsibilities
// -----------------
// Owns all persistence access to the `tasks` table. Maps rows to a flat
// TaskRecord regardless of task_type ("ACADEMIC"/"DAILY"); deciding which
// concrete Task/AcademicTask/DailyTask object to construct from a
// TaskRecord is a TaskManager concern, not TaskDAO's. TaskDAO does not
// decide what "recurring" means operationally (e.g. spawning the next
// occurrence) — it only stores the flags it is given.
//
// SQL operations (conceptual)
// ---------------------------------------------------------------------------
//   create                     INSERT INTO tasks (...) VALUES (...)
//   findById                   SELECT * FROM tasks WHERE task_id = ?
//   findByUser                 SELECT * FROM tasks WHERE user_id = ? ORDER BY deadline
//   findByUserAndStatus        SELECT * FROM tasks WHERE user_id=? AND status=?
//   findByUserAndCategory      SELECT * FROM tasks WHERE user_id=? AND category_id=?
//   findByUserAndTitle         SELECT * FROM tasks WHERE user_id=? AND title LIKE CONCAT('%',?,'%')
//   findByUserAndDeadline      SELECT * FROM tasks WHERE user_id=? AND DATE(deadline)=?
//   findRecurringByUser        SELECT * FROM tasks WHERE user_id=? AND is_recurring=TRUE
//   update                     UPDATE tasks SET <mutable columns> WHERE task_id=?
//   updateStatus               UPDATE tasks SET status=? WHERE task_id=?
//   remove                     DELETE FROM tasks WHERE task_id=?
//   countByUserAndStatus       SELECT COUNT(*) FROM tasks WHERE user_id=? AND status=?
//
// Return values
// -------------
//   create                 -> int64_t                  new task_id
//   findById                -> std::optional<TaskRecord>
//   findByUser / ...        -> std::vector<TaskRecord>  empty when none, never null
//   update / updateStatus /
//   remove                  -> bool                     true iff affectedRows() > 0
//   countByUserAndStatus     -> uint64_t
//
// Error handling
// --------------
// Same contract as every DAO in this architecture: DatabaseException
// subtypes propagate unchanged. Of particular note here — inserting or
// updating a task with a category_id that doesn't exist (or whose
// category_type isn't 'TASK') is only guarded at the FK level
// (fk_tasks_category, ON DELETE RESTRICT), so an invalid category_id
// surfaces as DBConstraintViolationException; TaskManager is expected to
// validate the category_id against a CategoryDAO lookup *before* calling
// TaskDAO::create/update so the user gets a clean validation message
// instead of a raw SQL error (see architecture review — CategoryDAO gap).
//
// Interaction with managers
// -------------------------
// TaskManager owns one TaskDAO and is the only caller. TaskManager is
// responsible for: choosing task_type/category based on user input,
// converting a TaskRecord to/from the existing Task/AcademicTask/DailyTask
// class hierarchy for display, and orchestrating recurrence (marking the
// current row COMPLETED via updateStatus and creating the next occurrence
// via create — two DAO calls, wrapped by TaskManager in a
// db::TransactionGuard so the pair is atomic).
// ============================================================================

#include "DaoTypes.h"
#include "Database.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace revolif::dao {

class TaskDAO {
public:
    explicit TaskDAO(db::Database& database);
    ~TaskDAO() = default;

    TaskDAO(const TaskDAO&) = delete;
    TaskDAO& operator=(const TaskDAO&) = delete;
    TaskDAO(TaskDAO&&) = delete;
    TaskDAO& operator=(TaskDAO&&) = delete;

    // ---- Create ----
    int64_t create(const TaskRecord& task);

    // ---- Read ----
    std::optional<TaskRecord> findById(int64_t taskId);
    std::vector<TaskRecord>   findByUser(int64_t userId, const PageRequest& page = {});
    std::vector<TaskRecord>   findByUserAndStatus(int64_t userId, const std::string& status);
    std::vector<TaskRecord>   findByUserAndCategory(int64_t userId, int64_t categoryId);
    std::vector<TaskRecord>   findByUserAndTitle(int64_t userId, const std::string& titleFragment);
    std::vector<TaskRecord>   findByUserAndDeadline(int64_t userId, const std::string& deadlineDateIso);
    std::vector<TaskRecord>   findRecurringByUser(int64_t userId);
    uint64_t                  countByUserAndStatus(int64_t userId, const std::string& status);

    // ---- Update ----
    bool update(const TaskRecord& task);
    bool updateStatus(int64_t taskId, const std::string& status);

    // ---- Delete ----
    bool remove(int64_t taskId);

private:
    db::Database& db_;
};

} // namespace revolif::dao
