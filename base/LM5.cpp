#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ============================================================
//                          DATE CLASS
// ============================================================

class Date
{
private:
    int day;
    int month;
    int year;

public:
    // DEFAULT CONSTRUCTOR
    Date() : day(1), month(1), year(2000) {}

    // PARAMETERIZED CONSTRUCTOR
    Date(int d, int m, int y)
        : day(d), month(m), year(y) {}

    // GETTERS

    int getDay() const
    {
        return day;
    }

    int getMonth() const
    {
        return month;
    }

    int getYear() const
    {
        return year;
    }

    // INPUT DATE

    void inputDate()
    {
        cout << "ENTER DAY: ";
        cin >> day;

        cout << "ENTER MONTH: ";
        cin >> month;

        cout << "ENTER YEAR: ";
        cin >> year;

        cin.ignore(1000, '\n');
    }

    // DISPLAY DATE

    void displayDate() const
    {
        cout << (day < 10 ? "0" : "")
             << day << "/"
             << (month < 10 ? "0" : "")
             << month << "/"
             << year;
    }

    friend ostream &operator<<(ostream &out, const Date &d)
    {
        out << (d.day < 10 ? "0" : "")
            << d.day << "/"
            << (d.month < 10 ? "0" : "")
            << d.month << "/"
            << d.year;

        return out;
    }

    // COMPARE DATE

    bool isEqual(Date d) const
    {
        return day == d.day &&
               month == d.month &&
               year == d.year;
    }
};

// ============================================================
//                         TASK CLASS
// ============================================================
// Parent class
// AcademicTask and DailyTask inherit from this class
// Stored using polymorphism through Task pointers
// ============================================================

class Task
{

protected:
    int taskID;

    static int nextTaskID;

    string title;

    Date deadline;

    string category;

    string status;

public:
    // CONSTRUCTOR

    Task(string t, Date d, string c)
    {
        taskID = nextTaskID++;

        title = t;

        deadline = d;

        category = c;

        status = "Pending";
    }

    // VIRTUAL DESTRUCTOR

    virtual ~Task() {}

    // GETTERS

    int getTaskID() const
    {
        return taskID;
    }

    string getTitle() const
    {
        return title;
    }

    Date getDeadline() const
    {
        return deadline;
    }

    string getCategory() const
    {
        return category;
    }

    string getStatus() const
    {
        return status;
    }

    // SETTERS

    void setTitle(string t)
    {
        title = t;
    }

    void setDeadline(Date d)
    {
        deadline = d;
    }

    // TASK STATUS FUNCTIONS

    void markCompleted()
    {
        status = "Completed";
    }

    void markPending()
    {
        status = "Pending";
    }

    // PURE VIRTUAL FUNCTION

    virtual void displayTask() const = 0;
};

// STATIC ID INITIALIZATION

int Task::nextTaskID = 1;

// ============================================================
//                       ACADEMIC TASK CLASS
// ============================================================

class AcademicTask : public Task
{

public:
    AcademicTask(string t, Date d)
        : Task(t, d, "Academic")
    {
    }

    // MENU FOR ACADEMIC TASKS

    static string getAcademicTitle()
    {

        int choice;

        cout << "\n========== ACADEMIC TASK ==========\n";

        cout << "1. Exam\n";
        cout << "2. Assignment\n";
        cout << "3. Project\n";
        cout << "4. Midterm\n";
        cout << "5. Test\n";
        cout << "6. Submission\n";
        cout << "7. Viva\n";
        cout << "8. Presentation\n";
        cout << "9. Other\n";

        cout << "ENTER CHOICE: ";
        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        case 1:
            return "Exam";

        case 2:
            return "Assignment";

        case 3:
            return "Project";

        case 4:
            return "Midterm";

        case 5:
            return "Test";

        case 6:
            return "Submission";

        case 7:
            return "Viva";

        case 8:
            return "Presentation";

        default:

            string other;

            cout << "ENTER TASK TITLE: ";
            getline(cin, other);

            return other;
        }
    }

    // DISPLAY OVERRIDE

    void displayTask() const override
    {

        cout << "\n====================================\n";

        cout << "TASK ID   : " << taskID << endl;

        cout << "CATEGORY  : " << category << endl;

        cout << "TITLE     : " << title << endl;

        cout << "DEADLINE  : " << deadline << endl;

        cout << "STATUS    : " << status << endl;

        cout << "====================================\n";
    }
};

// ============================================================
//                         DAILY TASK CLASS
// ============================================================

class DailyTask : public Task
{

public:
    DailyTask(string t, Date d)
        : Task(t, d, "Daily")
    {
    }

    // MENU FOR DAILY TASKS

    static string getDailyTitle()
    {

        int choice;

        cout << "\n========== DAILY TASK ==========\n";

        cout << "1. Laundry\n";

        cout << "2. Cleaning House\n";

        cout << "3. Washing Dishes\n";

        cout << "4. Exercise\n";

        cout << "5. Grocery Shopping\n";

        cout << "6. Cooking\n";

        cout << "7. Reading\n";

        cout << "8. Other\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        case 1:
            return "Laundry";

        case 2:
            return "Cleaning House";

        case 3:
            return "Washing Dishes";

        case 4:
            return "Exercise";

        case 5:
            return "Grocery Shopping";

        case 6:
            return "Cooking";

        case 7:
            return "Reading";

        default:

            string other;

            cout << "ENTER TASK TITLE: ";

            getline(cin, other);

            return other;
        }
    }

    // DISPLAY OVERRIDE

    void displayTask() const override
    {

        cout << "\n====================================\n";

        cout << "TASK ID   : " << taskID << endl;

        cout << "CATEGORY  : " << category << endl;

        cout << "TITLE     : " << title << endl;

        cout << "DEADLINE  : " << deadline << endl;

        cout << "STATUS    : " << status << endl;

        cout << "====================================\n";
    }
};

// ============================================================
//                       TASK MANAGER CLASS
// ============================================================
// Manages all tasks of a single user
// Stores AcademicTask and DailyTask using polymorphism
// ============================================================

class TaskManager
{

private:
    vector<Task *> tasks;

public:
    // ========================================================
    //                         DESTRUCTOR
    // ========================================================
    // Deletes dynamically created tasks
    // ========================================================

    ~TaskManager()
    {

        for (int i = 0; i < tasks.size(); i++)
        {
            delete tasks[i];
        }
    }

    // ========================================================
    //                         ADD TASK
    // ========================================================

    void addTask()
    {

        int choice;

        cout << "\n========== ADD TASK ==========\n";

        cout << "1. Academic Task\n";

        cout << "2. Daily Task\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        Date deadline;

        cout << "\nENTER DEADLINE:\n";

        deadline.inputDate();

        if (choice == 1)
        {

            string title = AcademicTask::getAcademicTitle();

            Task *task = new AcademicTask(title, deadline);

            tasks.push_back(task);

            cout << "ACADEMIC TASK ADDED SUCCESSFULLY!\n";
        }

        else if (choice == 2)
        {

            string title = DailyTask::getDailyTitle();

            Task *task = new DailyTask(title, deadline);

            tasks.push_back(task);

            cout << "DAILY TASK ADDED SUCCESSFULLY!\n";
        }

        else
        {
            cout << "INVALID CHOICE!\n";
        }
    }

    // ========================================================
    //                    DISPLAY ALL TASKS
    // ========================================================

    void displayAllTasks() const
    {

        if (tasks.empty())
        {
            cout << "NO TASKS AVAILABLE!\n";
            return;
        }

        cout << "\n========== ALL TASKS ==========\n";

        for (int i = 0; i < tasks.size(); i++)
        {

            tasks[i]->displayTask();
        }
    }

    // ========================================================
    //                 DISPLAY COMPLETED TASKS
    // ========================================================

    void displayCompletedTasks() const
    {

        cout << "\n========== COMPLETED TASKS ==========\n";

        bool found = false;

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getStatus() == "Completed")
            {

                tasks[i]->displayTask();

                found = true;
            }
        }

        if (found == false)
        {
            cout << "NO COMPLETED TASKS!\n";
        }
    }

    // ========================================================
    //                  DISPLAY PENDING TASKS
    // ========================================================

    void displayPendingTasks() const
    {

        cout << "\n========== PENDING TASKS ==========\n";

        bool found = false;

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getStatus() == "Pending")
            {

                tasks[i]->displayTask();

                found = true;
            }
        }

        if (found == false)
        {
            cout << "NO PENDING TASKS!\n";
        }
    }

    // ========================================================
    //                       SEARCH BY ID
    // ========================================================

    Task *searchByID(int id)
    {

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getTaskID() == id)
            {
                return tasks[i];
            }
        }

        return nullptr;
    }

    // ========================================================
    //                    SEARCH BY TITLE
    // ========================================================

    vector<Task *> searchByTitle(string title)
    {

        vector<Task *> result;

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getTitle() == title)
            {

                result.push_back(tasks[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                 SEARCH BY CATEGORY
    // ========================================================

    vector<Task *> searchByCategory(string category)
    {

        vector<Task *> result;

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getCategory() == category)
            {

                result.push_back(tasks[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                 SEARCH BY DEADLINE
    // ========================================================

    vector<Task *> searchByDeadline(Date date)
    {

        vector<Task *> result;

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getDeadline().isEqual(date))
            {

                result.push_back(tasks[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                  SEARCH BY STATUS
    // ========================================================

    vector<Task *> searchByStatus(string status)
    {

        vector<Task *> result;

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getStatus() == status)
            {

                result.push_back(tasks[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                  DISPLAY SEARCH RESULTS
    // ========================================================

    void displaySearchResults(vector<Task *> result)
    {

        if (result.empty())
        {
            cout << "NO TASK FOUND!\n";
            return;
        }

        for (int i = 0; i < result.size(); i++)
        {

            result[i]->displayTask();
        }
    }

    // ========================================================
    //                 MARK TASK COMPLETED
    // ========================================================

    void markTaskCompleted(int id)
    {

        Task *task = searchByID(id);

        if (task)
        {

            task->markCompleted();

            cout << "TASK MARKED AS COMPLETED!\n";
        }

        else
        {

            cout << "TASK NOT FOUND!\n";
        }
    }

    // ========================================================
    //                      DELETE TASK
    // ========================================================

    void deleteTask(int id)
    {

        for (int i = 0; i < tasks.size(); i++)
        {

            if (tasks[i]->getTaskID() == id)
            {

                delete tasks[i];

                tasks.erase(tasks.begin() + i);

                cout << "TASK DELETED SUCCESSFULLY!\n";

                return;
            }
        }

        cout << "TASK NOT FOUND!\n";
    }

    // ========================================================
    //                       UPDATE TASK
    // ========================================================

    void updateTask(int id)
    {

        Task *task = searchByID(id);

        if (task == nullptr)
        {

            cout << "TASK NOT FOUND!\n";

            return;
        }

        string newTitle;

        cout << "ENTER NEW TITLE: ";

        getline(cin, newTitle);

        task->setTitle(newTitle);

        cout << "TASK UPDATED SUCCESSFULLY!\n";
    }
};

// ============================================================
//                         EXPENSE CLASS
// ============================================================

class Expense
{

private:
    int expenseID;

    static int nextExpenseID;

    string title;

    double amount;

    string category;

    Date date;

    string description;

public:
    // CONSTRUCTOR

    Expense(string t, double a, string c, Date d, string desc)
    {

        expenseID = nextExpenseID++;

        title = t;

        amount = a;

        category = c;

        date = d;

        description = desc;
    }

    // ================= GETTERS =================

    int getExpenseID() const
    {
        return expenseID;
    }

    string getTitle() const
    {
        return title;
    }

    double getAmount() const
    {
        return amount;
    }

    string getCategory() const
    {
        return category;
    }

    Date getDate() const
    {
        return date;
    }

    string getDescription() const
    {
        return description;
    }

    // ================= SETTERS =================

    void setTitle(string t)
    {
        title = t;
    }

    void setAmount(double a)
    {
        amount = a;
    }

    void setDescription(string d)
    {
        description = d;
    }

    // ================= DISPLAY =================

    void displayExpense() const
    {

        cout << "\n====================================\n";

        cout << "EXPENSE ID  : " << expenseID << endl;

        cout << "TITLE       : " << title << endl;

        cout << "AMOUNT      : " << amount << endl;

        cout << "CATEGORY    : " << category << endl;

        cout << "DATE        : " << date << endl;

        cout << "DESCRIPTION : " << description << endl;

        cout << "====================================\n";
    }
};

int Expense::nextExpenseID = 1;

// ============================================================
//                     EXPENSE MANAGER CLASS
// ============================================================

class ExpenseManager
{

private:
    vector<Expense *> expenses;

public:
    // ================= DESTRUCTOR =================

    ~ExpenseManager()
    {

        for (int i = 0; i < expenses.size(); i++)
        {

            delete expenses[i];
        }
    }

    // ========================================================
    //                  CATEGORY SELECTION
    // ========================================================

    string selectCategory()
    {

        int choice;

        cout << "\n========== EXPENSE CATEGORY ==========\n";

        cout << "1. Food\n";

        cout << "2. Transport\n";

        cout << "3. Education\n";

        cout << "4. Shopping\n";

        cout << "5. Bills\n";

        cout << "6. Entertainment\n";

        cout << "7. Health\n";

        cout << "8. Other\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        case 1:

            return "Food";

        case 2:

            return "Transport";

        case 3:

            return "Education";

        case 4:

            return "Shopping";

        case 5:

            return "Bills";

        case 6:

            return "Entertainment";

        case 7:

            return "Health";

        default:
        {

            string other;

            cout << "ENTER CATEGORY NAME: ";

            getline(cin, other);

            return other;
        }
        }
    }

    // ========================================================
    //                       ADD EXPENSE
    // ========================================================

    void addExpense()
    {

        string title;

        double amount;

        string description;

        cout << "\n========== ADD EXPENSE ==========\n";

        cout << "ENTER TITLE: ";

        getline(cin, title);

        cout << "ENTER AMOUNT: ";

        cin >> amount;

        cin.ignore(1000, '\n');

        string category = selectCategory();

        Date date;

        cout << "ENTER DATE:\n";

        date.inputDate();

        cout << "ENTER DESCRIPTION: ";

        getline(cin, description);

        Expense *expense =
            new Expense(title,
                        amount,
                        category,
                        date,
                        description);

        expenses.push_back(expense);

        cout << "EXPENSE ADDED SUCCESSFULLY!\n";
    }

    // ========================================================
    //                 DISPLAY ALL EXPENSES
    // ========================================================

    void displayAllExpenses() const
    {

        if (expenses.empty())
        {

            cout << "NO EXPENSES AVAILABLE!\n";

            return;
        }

        cout << "\n========== ALL EXPENSES ==========\n";

        for (int i = 0; i < expenses.size(); i++)
        {

            expenses[i]->displayExpense();
        }
    }

    // ========================================================
    //                     SEARCH BY ID
    // ========================================================

    Expense *searchByID(int id)
    {

        for (int i = 0; i < expenses.size(); i++)
        {

            if (expenses[i]->getExpenseID() == id)
            {

                return expenses[i];
            }
        }

        return nullptr;
    }

    // ========================================================
    //                  SEARCH BY TITLE
    // ========================================================

    vector<Expense *> searchByTitle(string title)
    {

        vector<Expense *> result;

        for (int i = 0; i < expenses.size(); i++)
        {

            if (expenses[i]->getTitle() == title)
            {

                result.push_back(expenses[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                SEARCH BY CATEGORY
    // ========================================================

    vector<Expense *> searchByCategory(string category)
    {

        vector<Expense *> result;

        for (int i = 0; i < expenses.size(); i++)
        {

            if (expenses[i]->getCategory() == category)
            {

                result.push_back(expenses[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                    SEARCH BY DATE
    // ========================================================

    vector<Expense *> searchByDate(Date date)
    {

        vector<Expense *> result;

        for (int i = 0; i < expenses.size(); i++)
        {

            if (expenses[i]->getDate().isEqual(date))
            {

                result.push_back(expenses[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                 DISPLAY SEARCH RESULTS
    // ========================================================

    void displaySearchResults(vector<Expense *> result)
    {

        if (result.empty())
        {

            cout << "NO EXPENSE FOUND!\n";

            return;
        }

        for (int i = 0; i < result.size(); i++)
        {

            result[i]->displayExpense();
        }
    }

    // ========================================================
    //                       DELETE EXPENSE
    // ========================================================

    void deleteExpense(int id)
    {

        for (int i = 0; i < expenses.size(); i++)
        {

            if (expenses[i]->getExpenseID() == id)
            {

                delete expenses[i];

                expenses.erase(expenses.begin() + i);

                cout << "EXPENSE DELETED SUCCESSFULLY!\n";

                return;
            }
        }

        cout << "EXPENSE NOT FOUND!\n";
    }

    // ========================================================
    //                      UPDATE EXPENSE
    // ========================================================

    void updateExpense(int id)
    {

        Expense *expense = searchByID(id);

        if (expense == nullptr)
        {

            cout << "EXPENSE NOT FOUND!\n";

            return;
        }

        string title;

        cout << "ENTER NEW TITLE: ";

        getline(cin, title);

        expense->setTitle(title);

        double amount;

        cout << "ENTER NEW AMOUNT: ";

        cin >> amount;

        cin.ignore(1000, '\n');

        expense->setAmount(amount);

        string description;

        cout << "ENTER NEW DESCRIPTION: ";

        getline(cin, description);

        expense->setDescription(description);

        cout << "EXPENSE UPDATED SUCCESSFULLY!\n";
    }

    // ========================================================
    //                 TOTAL EXPENSE CALCULATION
    // ========================================================

    double calculateTotalExpense()
    {

        double total = 0;

        for (int i = 0; i < expenses.size(); i++)
        {

            total += expenses[i]->getAmount();
        }

        return total;
    }
};

// ============================================================
//                         GOAL CLASS
// ============================================================

class Goal
{

private:
    int goalID;

    static int nextGoalID;

    string title;

    string description;

    string category;

    Date deadline;

    string status;

public:
    // CONSTRUCTOR

    Goal(string t, string desc, string c, Date d)
    {

        goalID = nextGoalID++;

        title = t;

        description = desc;

        category = c;

        deadline = d;

        status = "Incomplete";
    }

    // ================= GETTERS =================

    int getGoalID() const
    {
        return goalID;
    }

    string getTitle() const
    {
        return title;
    }

    string getDescription() const
    {
        return description;
    }

    string getCategory() const
    {
        return category;
    }

    Date getDeadline() const
    {
        return deadline;
    }

    string getStatus() const
    {
        return status;
    }

    // ================= SETTERS =================

    void setTitle(string t)
    {
        title = t;
    }

    void setDescription(string d)
    {
        description = d;
    }

    void setCategory(string c)
    {
        category = c;
    }

    void setDeadline(Date d)
    {
        deadline = d;
    }

    // ================= COMPLETE GOAL =================

    void completeGoal()
    {
        status = "Completed";
    }

    // ================= DISPLAY =================

    void displayGoal() const
    {

        cout << "\n====================================\n";

        cout << "GOAL ID      : " << goalID << endl;

        cout << "TITLE        : " << title << endl;

        cout << "CATEGORY     : " << category << endl;

        cout << "DEADLINE     : " << deadline << endl;

        cout << "STATUS       : " << status << endl;

        cout << "DESCRIPTION  : " << description << endl;

        cout << "====================================\n";
    }
};

int Goal::nextGoalID = 1;

// ============================================================
//                      GOAL MANAGER CLASS
// ============================================================

class GoalManager
{

private:
    vector<Goal *> goals;

    vector<string> badges;

public:
    // ================= DESTRUCTOR =================

    ~GoalManager()
    {

        for (int i = 0; i < goals.size(); i++)
        {

            delete goals[i];
        }
    }

    // ========================================================
    //                 CATEGORY SELECTION
    // ========================================================

    string selectCategory()
    {

        int choice;

        cout << "\n========== GOAL CATEGORY ==========\n";

        cout << "1. Academic\n";

        cout << "2. Career\n";

        cout << "3. Health\n";

        cout << "4. Personal Development\n";

        cout << "5. Financial\n";

        cout << "6. Other\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        case 1:

            return "Academic";

        case 2:

            return "Career";

        case 3:

            return "Health";

        case 4:

            return "Personal Development";

        case 5:

            return "Financial";

        default:
        {

            string other;

            cout << "ENTER CATEGORY NAME: ";

            getline(cin, other);

            return other;
        }
        }
    }

    // ========================================================
    //                       ADD GOAL
    // ========================================================

    void addGoal()
    {

        string title;

        string description;

        cout << "\n========== ADD GOAL ==========\n";

        cout << "ENTER GOAL TITLE: ";

        getline(cin, title);

        string category = selectCategory();

        Date deadline;

        cout << "ENTER DEADLINE:\n";

        deadline.inputDate();

        cout << "ENTER DESCRIPTION: ";

        getline(cin, description);

        Goal *goal =
            new Goal(title,
                     description,
                     category,
                     deadline);

        goals.push_back(goal);

        cout << "GOAL ADDED SUCCESSFULLY!\n";
    }

    // ========================================================
    //                  DISPLAY ALL GOALS
    // ========================================================

    void displayAllGoals() const
    {

        if (goals.empty())
        {

            cout << "NO GOALS AVAILABLE!\n";

            return;
        }

        cout << "\n========== ALL GOALS ==========\n";

        for (int i = 0; i < goals.size(); i++)
        {

            goals[i]->displayGoal();
        }
    }

    // ========================================================
    //              DISPLAY COMPLETED GOALS
    // ========================================================

    void displayCompletedGoals() const
    {

        bool found = false;

        cout << "\n========== COMPLETED GOALS ==========\n";

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getStatus() == "Completed")
            {

                goals[i]->displayGoal();

                found = true;
            }
        }

        if (found == false)
        {

            cout << "NO COMPLETED GOALS!\n";
        }
    }

    // ========================================================
    //             DISPLAY INCOMPLETE GOALS
    // ========================================================

    void displayIncompleteGoals() const
    {

        bool found = false;

        cout << "\n========== INCOMPLETE GOALS ==========\n";

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getStatus() == "Incomplete")
            {

                goals[i]->displayGoal();

                found = true;
            }
        }

        if (found == false)
        {

            cout << "NO INCOMPLETE GOALS!\n";
        }
    }

    // ========================================================
    //                      SEARCH BY ID
    // ========================================================

    Goal *searchByID(int id)
    {

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getGoalID() == id)
            {

                return goals[i];
            }
        }

        return nullptr;
    }

    // ========================================================
    //                    SEARCH BY TITLE
    // ========================================================

    vector<Goal *> searchByTitle(string title)
    {

        vector<Goal *> result;

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getTitle() == title)
            {

                result.push_back(goals[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                  SEARCH BY CATEGORY
    // ========================================================

    vector<Goal *> searchByCategory(string category)
    {

        vector<Goal *> result;

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getCategory() == category)
            {

                result.push_back(goals[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                    SEARCH BY DEADLINE
    // ========================================================

    vector<Goal *> searchByDeadline(Date date)
    {

        vector<Goal *> result;

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getDeadline().isEqual(date))
            {

                result.push_back(goals[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                     SEARCH BY STATUS
    // ========================================================

    vector<Goal *> searchByStatus(string status)
    {

        vector<Goal *> result;

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getStatus() == status)
            {

                result.push_back(goals[i]);
            }
        }

        return result;
    }

    // ========================================================
    //                 DISPLAY SEARCH RESULTS
    // ========================================================

    void displaySearchResults(vector<Goal *> result)
    {

        if (result.empty())
        {

            cout << "NO GOAL FOUND!\n";

            return;
        }

        for (int i = 0; i < result.size(); i++)
        {

            result[i]->displayGoal();
        }
    }

    // ========================================================
    //                    COMPLETE GOAL
    // ========================================================

    void completeGoal(int id)
    {

        Goal *goal = searchByID(id);

        if (goal == nullptr)
        {

            cout << "GOAL NOT FOUND!\n";

            return;
        }

        if (goal->getStatus() == "Completed")
        {

            cout << "GOAL ALREADY COMPLETED!\n";

            return;
        }

        goal->completeGoal();

        updateBadges();

        cout << "GOAL COMPLETED SUCCESSFULLY!\n";
    }

    // ========================================================
    //                  BADGE SYSTEM
    // ========================================================

    void updateBadges()
    {

        int count = 0;

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getStatus() == "Completed")
            {

                count++;
            }
        }

        if (count >= 1 && badges.size() == 0)
        {

            badges.push_back("First Goal Completed Badge");
        }

        if (count >= 10 && badges.size() == 1)
        {

            badges.push_back("10 Goals Completed Badge");
        }

        if (count >= 25 && badges.size() == 2)
        {

            badges.push_back("25 Goals Completed Badge");
        }
    }

    // ========================================================
    //                 VIEW ACHIEVEMENTS
    // ========================================================

    void viewAchievements() const
    {

        cout << "\n========== ACHIEVEMENTS ==========\n";

        if (badges.empty())
        {

            cout << "NO BADGES EARNED YET!\n";

            return;
        }

        for (int i = 0; i < badges.size(); i++)
        {

            cout << "[BADGE] " << badges[i] << endl;
        }
    }

    // ========================================================
    //                    UPDATE GOAL
    // ========================================================

    void updateGoal(int id)
    {

        Goal *goal = searchByID(id);

        if (goal == nullptr)
        {

            cout << "GOAL NOT FOUND!\n";

            return;
        }

        string title;

        cout << "ENTER NEW TITLE: ";

        getline(cin, title);

        goal->setTitle(title);

        string description;

        cout << "ENTER NEW DESCRIPTION: ";

        getline(cin, description);

        goal->setDescription(description);

        cout << "GOAL UPDATED SUCCESSFULLY!\n";
    }

    // ========================================================
    //                    DELETE GOAL
    // ========================================================

    void deleteGoal(int id)
    {

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getGoalID() == id)
            {

                delete goals[i];

                goals.erase(goals.begin() + i);

                cout << "GOAL DELETED SUCCESSFULLY!\n";

                return;
            }
        }

        cout << "GOAL NOT FOUND!\n";
    }
};

// ============================================================
//                          USER CLASS
// ============================================================
// User owns TaskManager, ExpenseManager and GoalManager (Composition)
// Every user has their own separate tasks, expenses and goals
// ============================================================

class Authentication;

class User
{

private:
    string name;

    int UID;

    static int next_id;

    Date DOB;

    string password;

    // Composition
    TaskManager taskManager;

    ExpenseManager expenseManager;

    friend class Authentication;

public:
    // DEFAULT CONSTRUCTOR

    User()
        : UID(next_id++),
          name("NONE"),
          password("")
    {
    }

    // PARAMETERIZED CONSTRUCTOR

    User(string n, Date dob, string pass)
        : name(n),
          UID(next_id++),
          DOB(dob),
          password(pass)
    {
    }

    // GETTERS

    string getName() const
    {
        return name;
    }

    int getUID() const
    {
        return UID;
    }

    Date getDOB() const
    {
        return DOB;
    }

    // ACCESS TASK MANAGER

    TaskManager &getTaskManager()
    {
        return taskManager;
    }

    // ACCESS EXPENSE MANAGER

    ExpenseManager &getExpenseManager()
    {
        return expenseManager;
    }

    // DISPLAY PROFILE

    void displayProfile() const
    {

        cout << "\n================ USER PROFILE ================\n";

        cout << "UID  : " << UID << endl;

        cout << "NAME : " << name << endl;

        cout << "DOB  : " << DOB << endl;

        cout << "==============================================\n";
    }

    // UPDATE PROFILE

    void updateProfile()
    {

        int choice;

        cout << "\n1. Update Name\n";

        cout << "2. Update DOB\n";

        cout << "3. Cancel\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        if (choice == 1)
        {

            cout << "ENTER NEW NAME: ";

            getline(cin, name);

            cout << "NAME UPDATED!\n";
        }

        else if (choice == 2)
        {

            cout << "ENTER NEW DOB:\n";

            DOB.inputDate();

            cout << "DOB UPDATED!\n";
        }

        else
        {

            cout << "UPDATE CANCELLED!\n";
        }
    }

    // LOGIN CHECK

    bool login(string pass) const
    {

        return password == pass;
    }
};

int User::next_id = 1;

// ============================================================
//                   AUTHENTICATION CLASS
// ============================================================

class Authentication
{

public:
    bool authenticate(User &user, string pass)
    {

        if (user.login(pass))
        {

            cout << "\nLOGIN SUCCESSFUL!\n";

            cout << "WELCOME " << user.getName() << endl;

            return true;
        }

        cout << "INVALID PASSWORD!\n";

        return false;
    }

    void changePassword(User &user)
    {

        string oldPass;

        cout << "ENTER CURRENT PASSWORD: ";

        getline(cin, oldPass);

        if (!user.login(oldPass))
        {

            cout << "WRONG PASSWORD!\n";

            return;
        }

        string newPass;

        cout << "ENTER NEW PASSWORD: ";

        getline(cin, newPass);

        user.password = newPass;

        cout << "PASSWORD CHANGED SUCCESSFULLY!\n";
    }
};

// ============================================================
//                    TASK MANAGER MENU
// ============================================================

void taskMenu(User &user)
{

    int choice;

    do
    {

        cout << "\n\n================ TASK MANAGER ================\n";

        cout << "1. Add Task\n";

        cout << "2. Display All Tasks\n";

        cout << "3. Display Completed Tasks\n";

        cout << "4. Display Pending Tasks\n";

        cout << "5. Search Task\n";

        cout << "6. Mark Task As Completed\n";

        cout << "7. Update Task\n";

        cout << "8. Delete Task\n";

        cout << "9. Back\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        // ADD TASK
        case 1:

            user.getTaskManager().addTask();

            break;

        // DISPLAY ALL
        case 2:

            user.getTaskManager().displayAllTasks();

            break;

        // COMPLETED TASKS
        case 3:

            user.getTaskManager().displayCompletedTasks();

            break;

        // PENDING TASKS
        case 4:

            user.getTaskManager().displayPendingTasks();

            break;

        // SEARCH MENU
        case 5:
        {

            int searchChoice;

            cout << "\n========== SEARCH TASK ==========\n";

            cout << "1. Search By ID\n";

            cout << "2. Search By Title\n";

            cout << "3. Search By Category\n";

            cout << "4. Search By Deadline\n";

            cout << "5. Search By Status\n";

            cout << "ENTER CHOICE: ";

            cin >> searchChoice;

            cin.ignore(1000, '\n');

            switch (searchChoice)
            {

            case 1:
            {

                int id;

                cout << "ENTER TASK ID: ";

                cin >> id;

                cin.ignore(1000, '\n');

                Task *task =
                    user.getTaskManager().searchByID(id);

                if (task)
                    task->displayTask();

                else
                    cout << "TASK NOT FOUND!\n";

                break;
            }

            case 2:
            {

                string title;

                cout << "ENTER TITLE: ";

                getline(cin, title);

                vector<Task *> result =
                    user.getTaskManager().searchByTitle(title);

                user.getTaskManager()
                    .displaySearchResults(result);

                break;
            }

            case 3:
            {

                string category;

                cout << "ENTER CATEGORY (Academic/Daily): ";

                getline(cin, category);

                vector<Task *> result =
                    user.getTaskManager().searchByCategory(category);

                user.getTaskManager()
                    .displaySearchResults(result);

                break;
            }

            case 4:
            {

                Date date;

                cout << "ENTER DEADLINE:\n";

                date.inputDate();

                vector<Task *> result =
                    user.getTaskManager().searchByDeadline(date);

                user.getTaskManager()
                    .displaySearchResults(result);

                break;
            }

            case 5:
            {

                string status;

                cout << "ENTER STATUS (Pending/Completed): ";

                getline(cin, status);

                vector<Task *> result =
                    user.getTaskManager().searchByStatus(status);

                user.getTaskManager()
                    .displaySearchResults(result);

                break;
            }

            default:

                cout << "INVALID SEARCH OPTION!\n";
            }

            break;
        }

        // MARK COMPLETE
        case 6:
        {

            int id;

            cout << "ENTER TASK ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getTaskManager()
                .markTaskCompleted(id);

            break;
        }

        // UPDATE
        case 7:
        {

            int id;

            cout << "ENTER TASK ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getTaskManager()
                .updateTask(id);

            break;
        }

        // DELETE
        case 8:
        {

            int id;

            cout << "ENTER TASK ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getTaskManager()
                .deleteTask(id);

            break;
        }

        case 9:

            cout << "RETURNING TO LIFE MANAGER...\n";

            break;

        default:

            cout << "INVALID CHOICE!\n";
        }

    } while (choice != 9);
}

// ============================================================
//                   EXPENSE MANAGER MENU
// ============================================================

void expenseMenu(User &user)
{

    int choice;

    do
    {

        cout << "\n\n================ EXPENSE MANAGER ================\n";

        cout << "1. Add Expense\n";

        cout << "2. Display All Expenses\n";

        cout << "3. Search Expense\n";

        cout << "4. Update Expense\n";

        cout << "5. Delete Expense\n";

        cout << "6. Show Total Expense\n";

        cout << "7. Back\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        // ADD EXPENSE
        case 1:

            user.getExpenseManager().addExpense();

            break;

        // DISPLAY ALL
        case 2:

            user.getExpenseManager().displayAllExpenses();

            break;

        // SEARCH MENU
        case 3:
        {

            int searchChoice;

            cout << "\n========== SEARCH EXPENSE ==========\n";

            cout << "1. Search By ID\n";

            cout << "2. Search By Title\n";

            cout << "3. Search By Category\n";

            cout << "4. Search By Date\n";

            cout << "ENTER CHOICE: ";

            cin >> searchChoice;

            cin.ignore(1000, '\n');

            switch (searchChoice)
            {

            case 1:
            {

                int id;

                cout << "ENTER EXPENSE ID: ";

                cin >> id;

                cin.ignore(1000, '\n');

                Expense *expense =
                    user.getExpenseManager().searchByID(id);

                if (expense)
                    expense->displayExpense();

                else
                    cout << "EXPENSE NOT FOUND!\n";

                break;
            }

            case 2:
            {

                string title;

                cout << "ENTER TITLE: ";

                getline(cin, title);

                vector<Expense *> result =
                    user.getExpenseManager().searchByTitle(title);

                user.getExpenseManager()
                    .displaySearchResults(result);

                break;
            }

            case 3:
            {

                string category;

                cout << "ENTER CATEGORY: ";

                getline(cin, category);

                vector<Expense *> result =
                    user.getExpenseManager().searchByCategory(category);

                user.getExpenseManager()
                    .displaySearchResults(result);

                break;
            }

            case 4:
            {

                Date date;

                cout << "ENTER DATE:\n";

                date.inputDate();

                vector<Expense *> result =
                    user.getExpenseManager().searchByDate(date);

                user.getExpenseManager()
                    .displaySearchResults(result);

                break;
            }

            default:

                cout << "INVALID SEARCH OPTION!\n";
            }

            break;
        }

        // UPDATE
        case 4:
        {

            int id;

            cout << "ENTER EXPENSE ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getExpenseManager()
                .updateExpense(id);

            break;
        }

        // DELETE
        case 5:
        {

            int id;

            cout << "ENTER EXPENSE ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getExpenseManager()
                .deleteExpense(id);

            break;
        }

        // TOTAL EXPENSE
        case 6:

            cout << "TOTAL EXPENSE: "
                 << user.getExpenseManager().calculateTotalExpense()
                 << endl;

            break;

        case 7:

            cout << "RETURNING TO LIFE MANAGER...\n";

            break;

        default:

            cout << "INVALID CHOICE!\n";
        }

    } while (choice != 7);
}

// ============================================================
//                           MAIN
// ============================================================

int main()
{

    cout << "========== CREATE ACCOUNT ==========\n";

    string name;

    cout << "ENTER NAME: ";

    getline(cin, name);

    Date dob;

    cout << "ENTER DATE OF BIRTH:\n";

    dob.inputDate();

    string password;

    cout << "SET PASSWORD: ";

    getline(cin, password);

    User user(name, dob, password);

    Authentication auth;

    int choice;

    do
    {

        cout << "\n================ LIFE MANAGER ================\n";

        cout << "1. Login\n";

        cout << "2. View Profile\n";

        cout << "3. Update Profile\n";

        cout << "4. Task Manager\n";

        cout << "5. Expense Manager\n";

        cout << "6. Change Password\n";

        cout << "7. Exit\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        case 1:
        {

            string pass;

            cout << "ENTER PASSWORD: ";

            getline(cin, pass);

            auth.authenticate(user, pass);

            break;
        }

        case 2:

            user.displayProfile();

            break;

        case 3:

            user.updateProfile();

            break;

        case 4:

            taskMenu(user);

            break;

        case 5:

            expenseMenu(user);

            break;

        case 6:

            auth.changePassword(user);

            break;

        case 7:

            cout << "EXITING LIFE MANAGER...\n";

            break;

        default:

            cout << "INVALID CHOICE!\n";
        }

    } while (choice != 7);

    return 0;
}