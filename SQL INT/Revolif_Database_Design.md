# Revolif — MySQL 8 Database Design

Companion document to `revolif_schema.sql`. Explains **why** each table,
relationship, and column exists, then closes with a critical review of the
schema and suggested improvements before implementation.

No C++ is discussed here — this is a pure database design document.

---

## 1. Design principles applied

- **3rd Normal Form**: no repeating groups, every non-key column depends on
  the whole primary key and nothing but the key.
- **Surrogate keys everywhere** (`BIGINT UNSIGNED AUTO_INCREMENT` /
  `INT UNSIGNED AUTO_INCREMENT`): natural keys like `username` are unique
  but mutable, so they're kept as `UNIQUE` constraints, not primary keys.
- **InnoDB** for every table: required for foreign keys, transactions, and
  row-level locking.
- **`utf8mb4`**: full Unicode support (emoji, Urdu/Arabic script, etc.),
  matching the informal Urdu/English usage patterns of the app.
- **Explicit `ON DELETE` / `ON UPDATE` rules** on every foreign key — never
  left to default (`RESTRICT`), so cascade behavior is a conscious decision
  per relationship (see Section 3).
- **`CHECK` constraints** (MySQL 8.0.16+) enforce invariants the C++ code
  previously enforced only in input-validation loops (e.g. non-negative
  amounts) — the database now protects data integrity even if the
  application layer has a bug.

---

## 2. Why each table exists

### `achievements`
Global, shared catalog of unlockable milestones (default + admin-created
custom ones). It is **not** per-user — the same six default achievements
apply to every account, so it must be its own table rather than embedded
in `users`.

### `users`
The core account/profile table. Holds authentication data, profile
fields, and the gamification state (streaks, cached title, featured
achievement) that in the original code lived directly on the `User`
object.

### `categories`
A **new** table not present as a class in the original code (categories
were free-text string literals duplicated across three managers). Exists
to give Task/Goal/Expense category values a single, normalized source of
truth — supporting renaming, custom user categories, and preventing typos
like `"transprt"` slipping into the `expenses` table.

### `tasks`
One row per to-do item. Merges `AcademicTask` and `DailyTask` into a
single table with a `task_type` discriminator, since the two C++
subclasses had no divergent fields or behavior.

### `goals`
One row per longer-term objective. Structurally similar to `tasks` but
kept as a separate table because goals have different lifecycle
semantics (goal *completion* drives achievement unlocking and the title
system; task completion does not).

### `expenses`
One row per spend record.

### `category_budgets`
A **new** table, promoted from what was previously just a
`map<string,double>` living inside `ExpenseManager` with no independent
identity. Needs its own table because it has its own lifecycle (a user
can set/update/remove a budget independent of any expense row) and its
own uniqueness rule (one limit per user per category).

### `achievements` / `user_achievements`
Split into two tables because "which achievements exist" and "which user
unlocked which achievement" are two different many-to-many-shaped facts.
`user_achievements` is the resolving join table for a relationship that
was previously a raw `vector<int>` on `User` with no supporting table at
all.

### `habits` and `habit_history`
**New module**, not present in the original code, added because you asked
for it in the requirements. Split into two tables deliberately:
- `habits` holds the *definition* of a recurring behavior (what it is, how
  often it should happen, current/best streak).
- `habit_history` holds the *append-only log* of individual check-ins.

  This mirrors a mistake already flagged in the original `Task` recurrence
  logic, where completing a recurring task **overwrote** its deadline in
  place and destroyed history. Habits avoid repeating that mistake: every
  day's check-in is its own row, so streaks can always be recomputed or
  audited later, and nothing is silently lost.

### `journal`
**New module**. Kept separate from `notes` because journal entries are
inherently **date-anchored** (one entry ties to a specific day, optionally
with a mood rating) — a different shape of data from free-floating notes.

### `notes`
**New module**. General-purpose free text, not tied to a specific date,
optionally pinned. Kept separate from `journal` for the reason above; a
shared "notes" table for both would force nullable, awkwardly-overloaded
columns (`entry_date` meaningless for a random note, `is_pinned`
meaningless for a journal entry).

### `admins`
Kept as its own table because that was explicitly requested. See Section 6
(critical review) for why merging this into `users` via a `role` column is
still the recommendation for a production system.

---

## 3. Why each relationship exists (and why that cascade rule)

| Relationship | Cardinality | ON DELETE | Why |
|---|---|---|---|
| `users` → `achievements` (`displayed_achievement_id`) | many-to-one, optional | `SET NULL` | If an achievement is ever removed, a user's profile shouldn't break — it just stops featuring anything. |
| `categories` → `users` (`created_by_user_id`) | many-to-one, optional | `CASCADE` | Custom categories belong to the user who made them; if the user is deleted, their custom categories should go too. Default (system) categories have `created_by_user_id = NULL` and are unaffected. |
| `tasks/goals/expenses/category_budgets/habits/journal/notes` → `users` | many-to-one, required | `CASCADE` | If a user account is deleted, all of their personal records should be deleted with it — there is no legitimate reason to keep orphaned tasks/goals/expenses for a user_id that no longer exists. |
| `tasks/goals/expenses/category_budgets` → `categories` | many-to-one, required | `RESTRICT` | A category must not be deletable while records still reference it — this forces an explicit re-categorization or soft-delete (`is_default`/archive flag) instead of silently orphaning financial/task data. |
| `tasks/goals/expenses/category_budgets` → `categories` (composite on `category_id, scope`) | — | — | Prevents a **logically invalid** reference such as a `tasks` row pointing at an `Expense`-scoped category. MySQL can't express "FK only if category.scope = 'Task'" directly, so each child table carries a fixed `category_scope` column and a composite FK against `categories(category_id, scope)`, which *is* enforceable. |
| `user_achievements` → `users`, `user_achievements` → `achievements` | many-to-many (resolved) | `CASCADE` / `CASCADE` | Deleting a user should remove their unlock records; deleting an achievement (a custom one — defaults are protected at the application layer) should remove any records of it being unlocked. |
| `habit_history` → `habits` | many-to-one, required | `CASCADE` | A check-in log entry is meaningless without its parent habit; deleting a habit should delete its whole history rather than leaving dangling logs. |

---

## 4. Why each column exists (grouped by concern)

**Identity & ownership** — every child table's `user_id` (or `habit_id` for
`habit_history`) exists purely to scope rows to their owner; this is what
replaces the old `TaskManager`/`GoalManager`/`ExpenseManager` C++ objects —
those classes disappear entirely at the DB layer and become nothing more
than a `WHERE user_id = ?` filter.

**Auditing** — `created_at` / `updated_at` on every table exist because the
original code tracked almost none of this (e.g. it never recorded *when* an
achievement was unlocked). These are cheap to add now and valuable for any
future analytics, support, or "recently added" UI.

**Denormalized/cached values** — `users.title`, `users.current_streak` /
`best_streak`, `habits.current_streak` / `best_streak` are intentionally
**stored** rather than always recomputed, mirroring the original code's own
choice to cache `User::title`. This is a deliberate normalization trade-off
(see Section 6, item 3) made for cheap reads on frequently-displayed
dashboard values — as opposed to the "Life Score," which the original code
always computed live and which is **not** persisted anywhere in this
schema, on purpose.

**Enums vs. free text** — `status`, `priority`, `task_type`, `frequency`,
`mood`, etc. use `ENUM` rather than `VARCHAR` because their value sets are
small, fixed, and drive branching business logic (sorting by priority,
filtering by status). Category *names*, by contrast, use `VARCHAR` in the
`categories` table because users can add custom ones.

**Nullable vs. required** — `last_login_at` is nullable (`NULL` = never
logged in), replacing the original code's magic sentinel date
(`20000101`). `completed_at` on `goals` is nullable because it's only set
once a goal transitions to `Completed`. `displayed_achievement_id` on
`users` and `mood`/`title` on `journal` are nullable because they are
genuinely optional, user-chosen values.

---

## 5. Seed data included

- The 6 default achievements from `System::loadDefaultAchievements()`.
- The default Task categories (`Academic`, `Daily`).
- The default Goal categories (`Academic`, `Career`, `Health`, `Personal
  Development`, `Financial`, `Other`).
- The default Expense categories (`Food`, `Transport`, `Education`,
  `Shopping`, `Bills`, `Entertainment`, `Health`, `Other`).
- One default `admin` row — **with a placeholder password hash**. The
  original code's default was the literal string `admin123` run through a
  weak, non-cryptographic hash; this schema does not seed a working
  password on purpose. Generate a real bcrypt/argon2 hash in your
  application layer at deploy time and update that row (see the comment in
  the SQL file).

---

## 6. Critical review — issues and improvements to consider before implementation

1. **`admins` as a separate table duplicates `users`.** Both tables now
   carry `username`, `password_hash`, `last_login_at`, `created_at`,
   `updated_at`. This was built exactly as requested, but the stronger
   recommendation is still to merge `admins` into `users` via a
   `role ENUM('user','admin')` column. That removes duplicate auth logic,
   duplicate uniqueness constraints, and lets a single login flow handle
   both cases (which is closer to what `Authentication::attemptLogin`
   already effectively does today, just against two separate stores).

2. **The composite-FK "scope" trick adds real complexity.** Enforcing that
   a `tasks` row can only reference a `Task`-scoped category (via the
   `category_id, category_scope` composite foreign key) is the
   textbook-correct way to do it in MySQL, but it means every child table
   carries a redundant `category_scope` column that always holds one fixed
   value. If your team would rather keep the schema simpler and enforce
   this rule at the application layer instead (or with a `BEFORE INSERT/
   UPDATE` trigger), that's a reasonable trade-off — just don't drop the
   check entirely, or a bug could silently mix Task and Expense categories.

3. **Denormalized streak/title columns can drift from reality.** `users.
   current_streak`, `users.best_streak`, `users.title`,
   `habits.current_streak`, and `habits.best_streak` are all cached values
   that must be kept in sync by application logic (or triggers/stored
   procedures) every time a login or habit check-in happens. If that
   update logic is ever skipped (e.g. a bulk import, a failed transaction),
   these values go stale. Consider either (a) wrapping every write that
   touches them in a single transaction with the triggering event, or (b)
   accepting the risk and adding a periodic reconciliation job that
   recomputes them from `habit_history`/login logs.

4. **No login/session table.** The schema persists *accounts* but has no
   table for active sessions, refresh tokens, or "remember me" state. If
   the eventual app is web/API-based rather than a single console session,
   you'll want a `sessions` (or `auth_tokens`) table — deliberately left
   out here since the original C++ app has no concept of a session beyond
   "currently running program."

5. **No audit/history log for sensitive changes.** Password changes, admin
   status toggles (`adminToggleUserStatus` in the original code), and
   achievement removals currently leave no trace. Worth adding a generic
   `audit_log` table (actor, action, target table/id, timestamp) if
   accountability matters, especially for admin actions.

6. **`journal` allows multiple entries per day; consider whether that's
   intended.** No `UNIQUE(user_id, entry_date)` constraint was added,
   because many journaling apps intentionally allow several entries in one
   day. If your product intends "one entry per day," add that unique
   constraint; otherwise the current design is correct.

7. **No tagging/search support yet.** `notes` and `journal` are plain
   `TEXT` blobs with no tagging, and no `FULLTEXT` index was added. If
   free-text search across notes/journal/tasks becomes a feature, add
   `FULLTEXT` indexes (MySQL supports these on InnoDB in 5.6+) on the
   relevant `content`/`description`/`title` columns, and consider a
   `tags` + `note_tags`/`journal_tags` join table rather than a
   comma-separated tags column.

8. **`habit_history.status` enum (`Done`/`Missed`/`Skipped`) implies you
   need a row for days a habit was *not* done, not just days it was.**
   That only works if something (a scheduled job) proactively inserts
   `Missed` rows for days the user didn't check in — otherwise "missed"
   days simply won't exist as rows at all, and streak-breaking logic will
   need to detect *gaps* in `log_date` rather than relying on an explicit
   `Missed` status. Decide which approach you want before writing the
   application logic that reads this table.

9. **`category_budgets` is scoped monthly with no explicit month/year
   column.** The current design (one `monthly_limit` per user+category)
   matches the original C++ behavior (a single ongoing limit, not
   per-calendar-month tracking). If you want historical budget-vs-actual
   comparisons across different months, you'll eventually want a
   `budget_period` (or `year`, `month`) column added to make each row
   represent one specific month rather than an evergreen limit — flagging
   this now since it's a schema-shape decision, not just an app change.

10. **Soft-delete was intentionally not used anywhere.** Every table uses
    hard deletes with `CASCADE`/`RESTRICT`. This matches the original
    C++ behavior (`delete` + `vector::erase`, no "undo"), but if you want
    recoverable deletes (e.g. "restore my account within 30 days"), you'd
    need to add a `deleted_at` nullable column pattern instead — a
    deliberate decision to make before go-live, since retrofitting
    soft-deletes onto a schema with `ON DELETE CASCADE` everywhere is
    disruptive.

11. **Password reset flow has no supporting table.** `Authentication::
    changeUserPassword` requires knowing the *current* password — there is
    no "forgot password" flow in the original code, and correspondingly no
    `password_reset_tokens` table here. Add one if a self-service reset
    flow (email link, OTP, etc.) is planned.

12. **Consider partitioning or archiving `habit_history` long-term.** It's
    an append-only log that grows by roughly one row per habit per day,
    per user. For a course project this is a non-issue, but if this ever
    runs at scale, partitioning by date range or periodically archiving
    old rows would keep the table performant.

---

## 7. Summary of what changed vs. a literal 1:1 mapping of the C++ classes

- Added `categories` (previously just string literals).
- Added `category_budgets` as a real table (previously an untyped map).
- Added `unlocked_at` timestamp to achievement unlocking (previously
  untracked).
- Added `habits`, `habit_history`, `journal`, `notes` as brand-new modules
  per your requirements — none of these existed in the C++ code.
- Collapsed `Task`/`AcademicTask`/`DailyTask` into one table with a
  discriminator, instead of mirroring the inheritance 1:1.
- Replaced the "never logged in" magic date with a nullable
  `last_login_at`.
- Kept `Life Score` entirely out of the schema — it stays a computed,
  read-time value, never stored, matching the original code's own
  (correct) approach.
