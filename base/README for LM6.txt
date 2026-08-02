================================================================
                    LIFE MANAGER - README
================================================================

WHAT THIS IS
------------
A single C++ console program (LifeManager.cpp) that lets multiple
people register their own account and privately manage:
  - Tasks       (Academic / Daily, with deadlines)
  - Expenses    (spending log with categories and totals)
  - Goals       (with an achievement/title system)

Each account has its own separate tasks, expenses and goals.
Nothing is shared between accounts.

HOW TO COMPILE & RUN
---------------------
    g++ -std=c++17 -o LifeManager LifeManager.cpp
    ./LifeManager

================================================================
1. START SCREEN
================================================================
The program never jumps straight into account creation. It always
opens on this screen first:

    1. Register
    2. Login
    3. Exit

  REGISTER
    - Asks for: Name, Date of Birth, Password.
    - If the name is already taken, it tells you to log in
      instead (no duplicate accounts).
    - After registering you are returned to the start screen -
      you still have to log in separately.

  LOGIN
    - If no accounts exist yet, it tells you to register first
      instead of asking for a password.
    - Asks for Name, then Password.
    - If the name doesn't exist -> "NO ACCOUNT FOUND WITH THIS
      NAME!"
    - If the password is wrong -> "INVALID PASSWORD!"
    - On success:
        * Prints "LOGIN SUCCESSFUL! WELCOME <name>" (your earned
          title is shown in brackets after your name once you
          have one, e.g. "WELCOME Yazdaan (Goal Master)!")
        * Immediately shows a NOTIFICATIONS section listing all
          of YOUR pending tasks, so you see what's due without
          digging through menus.
        * Opens your personal USER MENU.

  EXIT
    - Closes the program.

================================================================
2. USER MENU (after login)
================================================================
    1. View Profile
    2. Update Profile
    3. Task Manager
    4. Expense Manager
    5. Goal Manager
    6. Change Password
    7. Logout

  Logout returns you to the START SCREEN (Register/Login/Exit),
  not to the operating system - so someone else can log in on
  the same run of the program.

  VIEW PROFILE
    Shows UID, Name (with title if earned), and Date of Birth.

  UPDATE PROFILE
    Choose to update your Name or your DOB.

  CHANGE PASSWORD
    Requires your current password before accepting a new one.

================================================================
3. TASK MANAGER
================================================================
    1. Add Task
    2. Display All Tasks
    3. Display Completed Tasks
    4. Display Pending Tasks
    5. Search Task
    6. Mark Task As Completed
    7. Update Task
    8. Delete Task
    9. Back

  ADD TASK
    - Choose Academic or Daily.
    - Enter a DEADLINE. The program checks this against your
      computer's actual current date - if you pick a date that
      has already passed, it refuses and makes you re-enter:
          "ERROR! THIS DATE HAS ALREADY PASSED.
           PLEASE ENTER TODAY'S DATE OR A FUTURE DATE."
      It also rejects impossible calendar dates (month 13,
      31st of February, etc.) and non-numeric input.
    - Academic tasks: Exam, Assignment, Project, Midterm, Test,
      Submission, Viva, Presentation, or a custom "Other" title.
    - Daily tasks: Laundry, Cleaning House, Washing Dishes,
      Exercise, Grocery Shopping, Cooking, Reading, or a custom
      "Other" title.
    - Every task gets a unique auto-incrementing Task ID and
      starts with status "Pending".

  SEARCH TASK
    By ID, Title, Category (Academic/Daily), Deadline, or Status.

  MARK COMPLETED / UPDATE / DELETE
    Operate on a task by its ID.

================================================================
4. EXPENSE MANAGER
================================================================
    1. Add Expense
    2. Display All Expenses
    3. Search Expense
    4. Update Expense
    5. Delete Expense
    6. Show Total Expense
    7. Back

  ADD EXPENSE
    - Title, Amount, Category (Food, Transport, Education,
      Shopping, Bills, Entertainment, Health, or custom
      "Other"), Date, Description.
    - Expense dates are NOT restricted to the future (you're
      usually logging money you already spent), so any valid
      calendar date is accepted.

  SEARCH EXPENSE
    By ID, Title, Category, or Date.

  SHOW TOTAL EXPENSE
    Adds up the amount of every expense you've logged.

================================================================
5. GOAL MANAGER
================================================================
    1. Add Goal
    2. Display All Goals
    3. Display Completed Goals
    4. Display Pending/Overdue Goals
    5. Search Goal
    6. Mark Goal As Completed
    7. View Achievements
    8. Update Goal
    9. Delete Goal
    10. Back

  ADD GOAL
    - Title, Category (Academic, Career, Health, Personal
      Development, Financial, or custom "Other"), Deadline,
      Description.
    - Same past-date rejection rule as Task deadlines applies.

  STATUS IS CHECKED AGAINST TODAY'S REAL DATE
    A goal's displayed status is one of:
      - "Completed"  -> you marked it done.
      - "Overdue"    -> not completed AND its deadline has
                        already passed (checked live against
                        the system clock every time you view it).
      - "Pending"    -> not completed and still has time left.
    This is calculated on the fly, so a goal can silently flip
    from Pending to Overdue just by the calendar moving forward
    - you don't have to do anything for that to update.

  MARK GOAL AS COMPLETED -> ACHIEVEMENT / TITLE SYSTEM
    Every completed goal increases an internal counter. When the
    counter crosses one of these milestones, you automatically
    unlock a new title, and it's announced immediately:

        5 goals completed  -> "Goal Beginner"
       10 goals completed  -> "Goal Enthusiast"
       20 goals completed  -> "Goal Achiever"
       30 goals completed  -> "Goal Master"
       40 goals completed  -> "Goal Champion"
       50 goals completed  -> "Goal Legend"   (highest title)

    - On your very first login you have NO title - only your
      plain name is shown.
    - Once you unlock one, your name is shown everywhere as
      "Name (Title)", e.g. "Yazdaan (Goal Achiever)".
    - Titles only ever upgrade forward (Beginner -> Enthusiast ->
      ... -> Legend); there's no going backward.

  VIEW ACHIEVEMENTS
    Shows your total completed-goal count and every title you've
    unlocked so far, in order.

================================================================
6. QUICK REFERENCE - VALIDATION RULES
================================================================
  - Task deadline    : cannot be a past date.
  - Goal deadline     : cannot be a past date.
  - Expense date      : any valid date allowed (past/present).
  - Date of Birth     : any valid date allowed.
  - All dates         : month must be 1-12, day must be valid
                         for that month/year (leap years handled),
                         year must be between 1900-2100.
  - Non-numeric date input (letters, symbols) is rejected and
    re-prompted instead of crashing the program.

================================================================
7. TYPICAL WALKTHROUGH (for a quick demo)
================================================================
  1. Start program -> Register (name/DOB/password).
  2. Login with that name/password.
  3. See "no pending tasks" notification (nothing added yet).
  4. Go to Task Manager -> Add Task -> try a past date -> see it
     get rejected -> enter a valid future date instead.
  5. Go to Goal Manager -> Add Goal -> Mark it Completed -> repeat
     until you've completed 5 goals -> watch the "Goal Beginner"
     title get announced and appear next to your name.
  6. Logout -> back at Register/Login/Exit screen.
  7. Login again -> notice the pending-tasks notification now
     lists the task you added, and your name shows your title.

================================================================
END OF README
================================================================
