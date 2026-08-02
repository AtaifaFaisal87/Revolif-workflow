-- ============================================================================
-- REVOLIF DATABASE SCHEMA
-- MySQL 8.0+
-- Generated from: REVOLIF - Final Database Design Decisions
-- ============================================================================
-- This script is idempotent-safe for a fresh install: it drops and recreates
-- the database. Remove the DROP DATABASE line if you are running this against
-- an environment where existing data must be preserved.
-- ============================================================================

DROP DATABASE IF EXISTS revolif;

CREATE DATABASE revolif
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_0900_ai_ci;

USE revolif;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 1;
SET sql_mode = 'STRICT_TRANS_TABLES,NO_ZERO_DATE,NO_ZERO_IN_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- ============================================================================
-- 1. ACHIEVEMENTS
-- Global catalog of achievements. Created before Users because Users holds an
-- optional "featured achievement" foreign key.
-- ============================================================================

CREATE TABLE achievements (
    achievement_id   INT UNSIGNED    NOT NULL AUTO_INCREMENT,
    name             VARCHAR(100)    NOT NULL,
    description      VARCHAR(255)    NOT NULL,
    required_goals    SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    is_default       BOOLEAN         NOT NULL DEFAULT FALSE,
    created_at       DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (achievement_id),
    UNIQUE KEY uq_achievements_name (name),
    CONSTRAINT chk_achievements_required_goals CHECK (required_goals >= 0)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 2. USERS
-- Single table for both regular users and administrators, distinguished by
-- the "role" discriminator column (see Section 3 of the design doc).
-- ============================================================================

CREATE TABLE users (
    user_id                  INT UNSIGNED    NOT NULL AUTO_INCREMENT,
    username                 VARCHAR(50)     NOT NULL,
    name                     VARCHAR(100)    NOT NULL,
    email                    VARCHAR(150)    NULL,
    password_hash            VARCHAR(255)    NOT NULL,
    role                     ENUM('USER','ADMIN') NOT NULL DEFAULT 'USER',
    date_of_birth            DATE            NULL,
    registration_date        DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_login               DATETIME        NULL,
    title                    VARCHAR(100)    NULL,
    is_active                BOOLEAN         NOT NULL DEFAULT FALSE,
    current_streak           SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    best_streak              SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    displayed_achievement_id INT UNSIGNED    NULL,

    PRIMARY KEY (user_id),
    UNIQUE KEY uq_users_username (username),
    UNIQUE KEY uq_users_email (email),
    KEY idx_users_role (role),

    CONSTRAINT fk_users_displayed_achievement
        FOREIGN KEY (displayed_achievement_id) REFERENCES achievements (achievement_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE,

    CONSTRAINT chk_users_streaks CHECK (best_streak >= current_streak),
    CONSTRAINT chk_users_current_streak CHECK (current_streak >= 0),
    CONSTRAINT chk_users_best_streak CHECK (best_streak >= 0)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 3. CATEGORIES
-- Replaces hard-coded category strings across Tasks, Goals, Expenses, and
-- Category Budgets (see Section 5 of the design doc).
-- ============================================================================

CREATE TABLE categories (
    category_id    INT UNSIGNED   NOT NULL AUTO_INCREMENT,
    name           VARCHAR(100)   NOT NULL,
    category_type  ENUM('TASK','GOAL','EXPENSE') NOT NULL,
    is_default     BOOLEAN        NOT NULL DEFAULT FALSE,
    created_at     DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (category_id),
    UNIQUE KEY uq_categories_name_type (name, category_type),
    KEY idx_categories_type (category_type)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 4. TASKS
-- AcademicTask and DailyTask collapse into one table with a task_type
-- discriminator (see Section 4 of the design doc).
-- ============================================================================

CREATE TABLE tasks (
    task_id               INT UNSIGNED   NOT NULL AUTO_INCREMENT,
    user_id               INT UNSIGNED   NOT NULL,
    category_id           INT UNSIGNED   NOT NULL,
    task_type             ENUM('ACADEMIC','DAILY') NOT NULL,
    title                 VARCHAR(150)   NOT NULL,
    description           VARCHAR(1000)  NULL,
    deadline              DATETIME       NOT NULL,
    status                ENUM('PENDING','COMPLETED') NOT NULL DEFAULT 'PENDING',
    priority              ENUM('LOW','MEDIUM','HIGH') NOT NULL DEFAULT 'MEDIUM',
    is_recurring          BOOLEAN        NOT NULL DEFAULT FALSE,
    recurrence_interval   ENUM('NONE','DAILY','WEEKLY','MONTHLY') NOT NULL DEFAULT 'NONE',
    created_at            DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (task_id),

    CONSTRAINT fk_tasks_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT fk_tasks_category
        FOREIGN KEY (category_id) REFERENCES categories (category_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE,

    CONSTRAINT chk_tasks_recurrence
        CHECK (is_recurring = FALSE OR recurrence_interval <> 'NONE'),

    KEY idx_tasks_status (status),
    KEY idx_tasks_deadline (deadline),
    KEY idx_tasks_user (user_id),
    KEY idx_tasks_category (category_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 5. GOALS
-- ============================================================================

CREATE TABLE goals (
    goal_id       INT UNSIGNED   NOT NULL AUTO_INCREMENT,
    user_id       INT UNSIGNED   NOT NULL,
    category_id   INT UNSIGNED   NOT NULL,
    title         VARCHAR(150)   NOT NULL,
    description   VARCHAR(1000)  NULL,
    deadline      DATETIME       NOT NULL,
    status        ENUM('INCOMPLETE','COMPLETED') NOT NULL DEFAULT 'INCOMPLETE',
    created_at    DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,
    completed_at  DATETIME       NULL,

    PRIMARY KEY (goal_id),

    CONSTRAINT fk_goals_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT fk_goals_category
        FOREIGN KEY (category_id) REFERENCES categories (category_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE,

    KEY idx_goals_status (status),
    KEY idx_goals_user (user_id),
    KEY idx_goals_category (category_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 6. EXPENSES
-- ============================================================================

CREATE TABLE expenses (
    expense_id     INT UNSIGNED    NOT NULL AUTO_INCREMENT,
    user_id        INT UNSIGNED    NOT NULL,
    category_id    INT UNSIGNED    NOT NULL,
    title          VARCHAR(150)    NOT NULL,
    amount         DECIMAL(10,2)   NOT NULL,
    expense_date   DATETIME        NOT NULL,
    description    VARCHAR(1000)   NULL,
    created_at     DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (expense_id),

    CONSTRAINT fk_expenses_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT fk_expenses_category
        FOREIGN KEY (category_id) REFERENCES categories (category_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE,

    CONSTRAINT chk_expenses_amount_positive CHECK (amount > 0),

    KEY idx_expenses_date (expense_date),
    KEY idx_expenses_category (category_id),
    KEY idx_expenses_user (user_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 7. CATEGORY BUDGETS
-- One budget per user per category (see Section 6 of the design doc).
-- ============================================================================

CREATE TABLE category_budgets (
    budget_id      INT UNSIGNED   NOT NULL AUTO_INCREMENT,
    user_id        INT UNSIGNED   NOT NULL,
    category_id    INT UNSIGNED   NOT NULL,
    monthly_limit  DECIMAL(10,2)  NOT NULL,
    created_at     DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at     DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    PRIMARY KEY (budget_id),
    UNIQUE KEY uq_budget_user_category (user_id, category_id),

    CONSTRAINT fk_budgets_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT fk_budgets_category
        FOREIGN KEY (category_id) REFERENCES categories (category_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE,

    CONSTRAINT chk_budgets_limit_positive CHECK (monthly_limit > 0)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 8. USER ACHIEVEMENTS
-- Many-to-many bridge between Users and Achievements (see Section 7).
-- ============================================================================

CREATE TABLE user_achievements (
    user_id           INT UNSIGNED   NOT NULL,
    achievement_id    INT UNSIGNED   NOT NULL,
    unlocked_at       DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (user_id, achievement_id),

    CONSTRAINT fk_user_achievements_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT fk_user_achievements_achievement
        FOREIGN KEY (achievement_id) REFERENCES achievements (achievement_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    KEY idx_user_achievements_achievement (achievement_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 9. HABITS
-- ============================================================================

CREATE TABLE habits (
    habit_id      INT UNSIGNED   NOT NULL AUTO_INCREMENT,
    user_id       INT UNSIGNED   NOT NULL,
    name          VARCHAR(150)   NOT NULL,
    description   VARCHAR(500)   NULL,
    frequency     ENUM('DAILY','WEEKLY','MONTHLY') NOT NULL DEFAULT 'DAILY',
    is_active     BOOLEAN        NOT NULL DEFAULT TRUE,
    created_at    DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (habit_id),

    CONSTRAINT fk_habits_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    KEY idx_habits_frequency (frequency),
    KEY idx_habits_user (user_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 10. HABIT HISTORY
-- Append-only completion log; never overwritten (see Section 8).
-- ============================================================================

CREATE TABLE habit_history (
    history_id            BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    habit_id              INT UNSIGNED    NOT NULL,
    completion_datetime   DATETIME        NOT NULL,
    status                ENUM('COMPLETED','MISSED') NOT NULL DEFAULT 'COMPLETED',

    PRIMARY KEY (history_id),

    CONSTRAINT fk_habit_history_habit
        FOREIGN KEY (habit_id) REFERENCES habits (habit_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    KEY idx_habit_history_habit (habit_id),
    KEY idx_habit_history_datetime (completion_datetime)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 11. JOURNAL
-- Date-oriented entries with an optional mood rating (see Section 9).
-- ============================================================================

CREATE TABLE journal (
    journal_id     INT UNSIGNED   NOT NULL AUTO_INCREMENT,
    user_id        INT UNSIGNED   NOT NULL,
    entry_date     DATETIME       NOT NULL,
    content        TEXT           NOT NULL,
    mood_rating    TINYINT UNSIGNED NULL,
    created_at     DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (journal_id),

    CONSTRAINT fk_journal_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT chk_journal_mood_rating CHECK (mood_rating IS NULL OR mood_rating BETWEEN 1 AND 5),

    KEY idx_journal_date (entry_date),
    KEY idx_journal_user (user_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- 12. NOTES
-- Free-form notes with optional pinned status (see Section 9).
-- ============================================================================

CREATE TABLE notes (
    note_id      INT UNSIGNED   NOT NULL AUTO_INCREMENT,
    user_id      INT UNSIGNED   NOT NULL,
    title        VARCHAR(150)   NOT NULL,
    content      TEXT           NOT NULL,
    is_pinned    BOOLEAN        NOT NULL DEFAULT FALSE,
    created_at   DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at   DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    PRIMARY KEY (note_id),

    CONSTRAINT fk_notes_user
        FOREIGN KEY (user_id) REFERENCES users (user_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    KEY idx_notes_user_pinned (user_id, is_pinned)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- ============================================================================
-- SEED DATA
-- ============================================================================

-- ---- 13.1 Default Administrator Account ----
-- Username: admin | Password: admin123 (bcrypt hash below)
-- CHANGE THIS PASSWORD IMMEDIATELY AFTER FIRST LOGIN.
INSERT INTO users (username, name, email, password_hash, role, is_active)
VALUES (
    'admin',
    'System Administrator',
    NULL,
    '$2b$12$2hD05UD5FbgtGEYpWxJpDOvQKD5yLajgfNl22AcJwUyhJMtMZyoD6',
    'ADMIN',
    TRUE
);

-- ---- 13.2 Default Achievements ----
INSERT INTO achievements (name, description, required_goals, is_default) VALUES
    ('First Step',        'Complete your very first goal.',            1,  TRUE),
    ('Goal Getter',        'Complete 5 goals.',                          5,  TRUE),
    ('Consistency Champion','Complete 15 goals.',                       15, TRUE),
    ('Overachiever',       'Complete 30 goals.',                        30, TRUE),
    ('Legend',             'Complete 50 goals.',                        50, TRUE);

-- ---- 13.3 Default Task Categories ----
INSERT INTO categories (name, category_type, is_default) VALUES
    ('Exam',         'TASK', TRUE),
    ('Assignment',   'TASK', TRUE),
    ('Project',      'TASK', TRUE),
    ('Midterm',      'TASK', TRUE),
    ('Test',         'TASK', TRUE),
    ('Submission',   'TASK', TRUE),
    ('Viva',         'TASK', TRUE),
    ('Presentation', 'TASK', TRUE),
    ('Laundry',          'TASK', TRUE),
    ('Cleaning House',   'TASK', TRUE),
    ('Washing Dishes',   'TASK', TRUE),
    ('Exercise',         'TASK', TRUE),
    ('Grocery Shopping', 'TASK', TRUE),
    ('Cooking',          'TASK', TRUE),
    ('Reading',          'TASK', TRUE),
    ('Other',            'TASK', TRUE);

-- ---- 13.4 Default Goal Categories ----
INSERT INTO categories (name, category_type, is_default) VALUES
    ('Academic',      'GOAL', TRUE),
    ('Career',        'GOAL', TRUE),
    ('Health',        'GOAL', TRUE),
    ('Finance',       'GOAL', TRUE),
    ('Personal',      'GOAL', TRUE),
    ('Relationships', 'GOAL', TRUE),
    ('Other',         'GOAL', TRUE);

-- ---- 13.5 Default Expense Categories ----
INSERT INTO categories (name, category_type, is_default) VALUES
    ('Food',           'EXPENSE', TRUE),
    ('Transportation', 'EXPENSE', TRUE),
    ('Rent',           'EXPENSE', TRUE),
    ('Utilities',      'EXPENSE', TRUE),
    ('Education',      'EXPENSE', TRUE),
    ('Entertainment',  'EXPENSE', TRUE),
    ('Healthcare',     'EXPENSE', TRUE),
    ('Other',          'EXPENSE', TRUE);

-- ============================================================================
-- END OF SCRIPT
-- ============================================================================
