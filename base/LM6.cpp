#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <utility>

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

    // ================= LEAP YEAR CHECK =================

    static bool isLeapYear(int y)
    {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    // ================= DAYS IN MONTH =================

    static int daysInMonth(int m, int y)
    {
        int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if (m == 2 && isLeapYear(y))
        {
            return 29;
        }

        return days[m - 1];
    }

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

    // ========================================================
    //                 GET CURRENT SYSTEM DATE
    // ========================================================

    static Date getToday()
    {
        time_t now = time(nullptr);

        tm *ltm = localtime(&now);

        return Date(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);
    }

    // CONVERT DATE TO A COMPARABLE NUMBER (YYYYMMDD)

    long toComparable() const
    {
        return (long)year * 10000 + (long)month * 100 + (long)day;
    }

    // CHECK IF THIS DATE IS BEFORE TODAY'S SYSTEM DATE

    bool isPastDate() const
    {
        return toComparable() < getToday().toComparable();
    }

    // ========================================================
    //   CONVERT TO A SERIAL DAY NUMBER (DAYS SINCE A FIXED
    //   EPOCH) SO WE CAN MEASURE "HOW MANY DAYS AWAY" A DATE
    //   IS - USED BY THE DASHBOARD'S "DUE SOON" CALCULATION
    // ========================================================

    long toDayNumber() const
    {
        int y = year;
        int m = month;
        int d = day;

        y -= (m <= 2) ? 1 : 0;

        long era = (y >= 0 ? y : y - 399) / 400;

        long yoe = y - era * 400;

        long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;

        long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;

        return era * 146097 + doe - 719468;
    }

    // NUMBER OF DAYS FROM THIS DATE TO ANOTHER DATE (CAN BE NEGATIVE)

    long daysUntil(const Date &other) const
    {
        return other.toDayNumber() - toDayNumber();
    }

    // ========================================================
    //                       INPUT DATE
    // ========================================================
    // restrictPast = true  ->  date must be TODAY or a FUTURE date
    //                          (used for task/goal deadlines so the
    //                           user cannot pick an already-passed date)
    // restrictPast = false ->  any valid calendar date is accepted
    //                          (used for DOB, expense date, etc.)
    // ========================================================

    void inputDate(bool restrictPast = false)
    {
        while (true)
        {
            int d, m, y;

            cout << "ENTER DAY(1-31): ";
            cin >> d;

            cout << "ENTER MONTH(1-12): ";
            cin >> m;

            cout << "ENTER YEAR: ";
            cin >> y;

            // HANDLE NON-NUMERIC INPUT

            if (cin.fail())
            {
                // IF THE INPUT STREAM HAS ENDED, STOP RETRYING
                // AND FALL BACK TO TODAY'S DATE TO AVOID HANGING

                if (cin.eof())
                {
                    cin.clear();

                    Date fallback = getToday();

                    day = fallback.day;
                    month = fallback.month;
                    year = fallback.year;

                    cout << "\nINPUT STREAM ENDED! DEFAULTING TO TODAY'S DATE.\n";

                    return;
                }

                cin.clear();
                cin.ignore(1000, '\n');

                cout << "INVALID INPUT! PLEASE ENTER NUMBERS ONLY.\n";

                continue;
            }

            cin.ignore(1000, '\n');

            // VALIDATE MONTH

            if (m < 1 || m > 12)
            {
                cout << "INVALID MONTH! MONTH MUST BE BETWEEN 1-12.\n";

                continue;
            }

            // VALIDATE YEAR

            if (y < 1900 || y > 2100)
            {
                cout << "INVALID YEAR! YEAR MUST BE BETWEEN 1900-2100.\n";

                continue;
            }

            // VALIDATE DAY (based on month/year)

            int maxDay = daysInMonth(m, y);

            if (d < 1 || d > maxDay)
            {
                cout << "INVALID DAY! DAY MUST BE BETWEEN 1-" << maxDay
                     << " FOR THE SELECTED MONTH.\n";

                continue;
            }

            // TEMPORARILY COMMIT VALUES SO isPastDate() CAN CHECK THEM

            day = d;
            month = m;
            year = y;

            // VALIDATE AGAINST PAST DATE (ONLY WHEN REQUIRED)

            if (restrictPast && isPastDate())
            {
                cout << "ERROR! THIS DATE HAS ALREADY PASSED.\n";
                cout << "PLEASE ENTER TODAY'S DATE OR A FUTURE DATE.\n";

                continue;
            }

            break;
        }
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

        // TRUE -> DEADLINE CANNOT BE AN ALREADY PASSED DATE

        deadline.inputDate(true);

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
    //           DASHBOARD SUPPORT - COUNTING HELPERS
    // ========================================================

    int countPendingTasks() const
    {
        int count = 0;

        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->getStatus() == "Pending")
            {
                count++;
            }
        }

        return count;
    }

    int countOverdueTasks() const
    {
        int count = 0;

        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->getStatus() == "Pending" &&
                tasks[i]->getDeadline().isPastDate())
            {
                count++;
            }
        }

        return count;
    }

    // TASKS STILL PENDING WITH A DEADLINE WITHIN THE NEXT "days" DAYS
    // (NOT ALREADY OVERDUE)

    int countDueSoonTasks(int days) const
    {
        int count = 0;

        Date today = Date::getToday();

        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->getStatus() == "Pending")
            {
                long diff = today.daysUntil(tasks[i]->getDeadline());

                if (diff >= 0 && diff <= days)
                {
                    count++;
                }
            }
        }

        return count;
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

    // ========================================================
    //   DASHBOARD SUPPORT - HIGHEST-SPENDING CATEGORY
    // ========================================================
    // Returns a pair of (category name, total amount spent in
    // that category). Returns ("", 0) if there are no expenses.
    // ========================================================

    pair<string, double> getTopCategoryInfo() const
    {

        if (expenses.empty())
        {
            return make_pair("", 0.0);
        }

        vector<string> categories;

        vector<double> totals;

        for (int i = 0; i < (int)expenses.size(); i++)
        {

            string cat = expenses[i]->getCategory();

            double amt = expenses[i]->getAmount();

            bool found = false;

            for (int j = 0; j < (int)categories.size(); j++)
            {

                if (categories[j] == cat)
                {

                    totals[j] += amt;

                    found = true;

                    break;
                }
            }

            if (!found)
            {
                categories.push_back(cat);

                totals.push_back(amt);
            }
        }

        int bestIndex = 0;

        for (int i = 1; i < (int)totals.size(); i++)
        {

            if (totals[i] > totals[bestIndex])
            {

                bestIndex = i;
            }
        }

        return make_pair(categories[bestIndex], totals[bestIndex]);
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

    // ========================================================
    //   EFFECTIVE / DISPLAY STATUS - CHECKED AGAINST TODAY'S
    //   ACTUAL SYSTEM DATE INSTEAD OF JUST THE STORED FLAG
    // ========================================================

    string getDisplayStatus() const
    {
        if (status == "Completed")
        {
            return "Completed";
        }

        if (deadline.isPastDate())
        {
            return "Overdue";
        }

        return "Pending";
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

        cout << "STATUS       : " << getDisplayStatus() << endl;

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

        // TRUE -> DEADLINE CANNOT BE AN ALREADY PASSED DATE

        deadline.inputDate(true);

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
    //             DISPLAY INCOMPLETE / PENDING GOALS
    // ========================================================
    // Uses the actual system date (via getDisplayStatus) so a
    // goal whose deadline has passed shows up as "Overdue"
    // rather than plain "Pending"
    // ========================================================

    void displayIncompleteGoals() const
    {

        bool found = false;

        cout << "\n========== PENDING / OVERDUE GOALS ==========\n";

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getStatus() != "Completed")
            {

                goals[i]->displayGoal();

                found = true;
            }
        }

        if (found == false)
        {

            cout << "NO PENDING GOALS!\n";
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
    //               COUNT COMPLETED GOALS (COUNTER)
    // ========================================================

    int countCompletedGoals() const
    {

        int count = 0;

        for (int i = 0; i < goals.size(); i++)
        {

            if (goals[i]->getStatus() == "Completed")
            {

                count++;
            }
        }

        return count;
    }

    // ========================================================
    //           DASHBOARD SUPPORT - COUNTING HELPERS
    // ========================================================

    int countPendingGoals() const
    {
        int count = 0;

        for (int i = 0; i < (int)goals.size(); i++)
        {
            if (goals[i]->getStatus() != "Completed" &&
                !goals[i]->getDeadline().isPastDate())
            {
                count++;
            }
        }

        return count;
    }

    int countOverdueGoals() const
    {
        int count = 0;

        for (int i = 0; i < (int)goals.size(); i++)
        {
            if (goals[i]->getStatus() != "Completed" &&
                goals[i]->getDeadline().isPastDate())
            {
                count++;
            }
        }

        return count;
    }

    // HOW MANY MORE COMPLETED GOALS ARE NEEDED TO UNLOCK THE NEXT
    // TITLE (0 IF THE MAXIMUM TITLE HAS ALREADY BEEN REACHED)

    int goalsUntilNextTitle() const
    {
        int count = countCompletedGoals();

        static const int milestoneCounts[] = {5, 10, 20, 30, 40, 50};

        for (int i = 0; i < 6; i++)
        {
            if (count < milestoneCounts[i])
            {
                return milestoneCounts[i] - count;
            }
        }

        return 0;
    }

    // NAME OF THE NEXT TITLE TO BE UNLOCKED ("" IF ALREADY MAXED OUT)

    string getNextTitleName() const
    {
        int count = countCompletedGoals();

        static const int milestoneCounts[] = {5, 10, 20, 30, 40, 50};

        static const string milestoneTitles[] = {
            "Goal Beginner",
            "Goal Enthusiast",
            "Goal Achiever",
            "Goal Master",
            "Goal Champion",
            "Goal Legend"};

        for (int i = 0; i < 6; i++)
        {
            if (count < milestoneCounts[i])
            {
                return milestoneTitles[i];
            }
        }

        return "";
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
    //                  TITLE / BADGE SYSTEM
    // ========================================================
    // A new title is unlocked at 5, 10, 20, 30, 40 and finally
    // 50 completed goals (50 is the maximum / final title).
    // Driven by the completed-goals counter above.
    // ========================================================

    void updateBadges()
    {

        int count = countCompletedGoals();

        static const int milestoneCounts[] = {5, 10, 20, 30, 40, 50};

        static const string milestoneTitles[] = {
            "Goal Beginner",
            "Goal Enthusiast",
            "Goal Achiever",
            "Goal Master",
            "Goal Champion",
            "Goal Legend"};

        int totalMilestones = 6;

        if ((int)badges.size() < totalMilestones)
        {

            int nextIndex = badges.size();

            if (count >= milestoneCounts[nextIndex])
            {

                badges.push_back(milestoneTitles[nextIndex]);
            }
        }
    }

    // ========================================================
    //          GET THE MOST RECENTLY UNLOCKED TITLE
    // ========================================================

    string getLatestTitle() const
    {

        if (badges.empty())
        {
            return "";
        }

        return badges.back();
    }

    // ========================================================
    //                 VIEW ACHIEVEMENTS
    // ========================================================

    void viewAchievements() const
    {

        cout << "\n========== ACHIEVEMENTS ==========\n";

        cout << "GOALS COMPLETED : " << countCompletedGoals() << endl;

        if (badges.empty())
        {

            cout << "NO TITLES EARNED YET! COMPLETE 5 GOALS TO EARN YOUR FIRST TITLE.\n";

            return;
        }

        for (int i = 0; i < badges.size(); i++)
        {

            cout << "[TITLE UNLOCKED] " << badges[i] << endl;
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

    // Title earned through completing goals (empty until 5 goals done)

    string title;

    // Composition
    TaskManager taskManager;

    ExpenseManager expenseManager;

    GoalManager goalManager;

    friend class Authentication;

public:
    // DEFAULT CONSTRUCTOR

    User()
        : UID(next_id++),
          name("NONE"),
          password(""),
          title("")
    {
    }

    // PARAMETERIZED CONSTRUCTOR

    User(string n, Date dob, string pass)
        : name(n),
          UID(next_id++),
          DOB(dob),
          password(pass),
          title("")
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

    string getTitle() const
    {
        return title;
    }

    // NAME AS DISPLAYED - INCLUDES EARNED TITLE, E.G. "yazdaan (Goal Master)"
    // ON FIRST LOGIN THERE IS NO TITLE YET, SO ONLY THE NAME IS SHOWN

    string getDisplayName() const
    {

        if (title.empty())
        {
            return name;
        }

        return name + " (" + title + ")";
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

    // ACCESS GOAL MANAGER

    GoalManager &getGoalManager()
    {
        return goalManager;
    }

    // ========================================================
    //    CHECK GOAL-COMPLETION COUNTER AND UNLOCK NEW TITLE
    // ========================================================
    // Called every time a goal is marked completed. Compares the
    // latest badge/title unlocked in GoalManager against the
    // user's current title and updates + announces it if changed.
    // ========================================================

    void checkAndUpdateTitle()
    {

        string latest = goalManager.getLatestTitle();

        if (!latest.empty() && latest != title)
        {

            title = latest;

            cout << "\n*******************************************\n";
            cout << "  CONGRATULATIONS! NEW TITLE UNLOCKED: " << title << "\n";
            cout << "*******************************************\n";
        }
    }

    // DISPLAY PROFILE

    void displayProfile() const
    {

        cout << "\n================ USER PROFILE ================\n";

        cout << "UID   : " << UID << endl;

        cout << "NAME  : " << getDisplayName() << endl;

        cout << "DOB   : " << DOB << endl;

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

            cout << "WELCOME " << user.getDisplayName() << "!\n";

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
//                    GOAL MANAGER MENU
// ============================================================

void goalMenu(User &user)
{

    int choice;

    do
    {

        cout << "\n\n================ GOAL MANAGER ================\n";

        cout << "1. Add Goal\n";

        cout << "2. Display All Goals\n";

        cout << "3. Display Completed Goals\n";

        cout << "4. Display Pending/Overdue Goals\n";

        cout << "5. Search Goal\n";

        cout << "6. Mark Goal As Completed\n";

        cout << "7. View Achievements\n";

        cout << "8. Update Goal\n";

        cout << "9. Delete Goal\n";

        cout << "10. Back\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        // ADD GOAL
        case 1:

            user.getGoalManager().addGoal();

            break;

        // DISPLAY ALL
        case 2:

            user.getGoalManager().displayAllGoals();

            break;

        // COMPLETED GOALS
        case 3:

            user.getGoalManager().displayCompletedGoals();

            break;

        // PENDING/OVERDUE GOALS
        case 4:

            user.getGoalManager().displayIncompleteGoals();

            break;

        // SEARCH MENU
        case 5:
        {

            int searchChoice;

            cout << "\n========== SEARCH GOAL ==========\n";

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

                cout << "ENTER GOAL ID: ";

                cin >> id;

                cin.ignore(1000, '\n');

                Goal *goal =
                    user.getGoalManager().searchByID(id);

                if (goal)
                    goal->displayGoal();

                else
                    cout << "GOAL NOT FOUND!\n";

                break;
            }

            case 2:
            {

                string title;

                cout << "ENTER TITLE: ";

                getline(cin, title);

                vector<Goal *> result =
                    user.getGoalManager().searchByTitle(title);

                user.getGoalManager()
                    .displaySearchResults(result);

                break;
            }

            case 3:
            {

                string category;

                cout << "ENTER CATEGORY: ";

                getline(cin, category);

                vector<Goal *> result =
                    user.getGoalManager().searchByCategory(category);

                user.getGoalManager()
                    .displaySearchResults(result);

                break;
            }

            case 4:
            {

                Date date;

                cout << "ENTER DEADLINE:\n";

                date.inputDate();

                vector<Goal *> result =
                    user.getGoalManager().searchByDeadline(date);

                user.getGoalManager()
                    .displaySearchResults(result);

                break;
            }

            case 5:
            {

                string status;

                cout << "ENTER STATUS (Incomplete/Completed): ";

                getline(cin, status);

                vector<Goal *> result =
                    user.getGoalManager().searchByStatus(status);

                user.getGoalManager()
                    .displaySearchResults(result);

                break;
            }

            default:

                cout << "INVALID SEARCH OPTION!\n";
            }

            break;
        }

        // MARK COMPLETE + CHECK FOR NEW TITLE
        case 6:
        {

            int id;

            cout << "ENTER GOAL ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getGoalManager().completeGoal(id);

            user.checkAndUpdateTitle();

            break;
        }

        // ACHIEVEMENTS
        case 7:

            user.getGoalManager().viewAchievements();

            break;

        // UPDATE
        case 8:
        {

            int id;

            cout << "ENTER GOAL ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getGoalManager().updateGoal(id);

            break;
        }

        // DELETE
        case 9:
        {

            int id;

            cout << "ENTER GOAL ID: ";

            cin >> id;

            cin.ignore(1000, '\n');

            user.getGoalManager().deleteGoal(id);

            break;
        }

        case 10:

            cout << "RETURNING TO LIFE MANAGER...\n";

            break;

        default:

            cout << "INVALID CHOICE!\n";
        }

    } while (choice != 10);
}

// ============================================================
//                        DASHBOARD
// ============================================================
// Shown right after a successful login. Pulls a one-screen
// summary from the Task, Goal and Expense managers so the user
// sees the state of their whole "life" at a glance, instead of
// having to open three separate menus.
// ============================================================

void showDashboard(User &user)
{

    TaskManager &tm = user.getTaskManager();

    GoalManager &gm = user.getGoalManager();

    ExpenseManager &em = user.getExpenseManager();

    cout << "\n================ YOUR DASHBOARD ================\n";

    cout << "WELCOME BACK, " << user.getDisplayName() << "\n";

    // ---------------- TASKS ----------------

    int overdueTasks = tm.countOverdueTasks();

    int dueSoonTasks = tm.countDueSoonTasks(3);

    int pendingTasks = tm.countPendingTasks();

    cout << "\nTASKS\n";

    cout << "  - " << overdueTasks << " OVERDUE, "
         << dueSoonTasks << " DUE WITHIN THE NEXT 3 DAYS\n";

    cout << "  - " << pendingTasks << " PENDING TOTAL\n";

    // ---------------- GOALS ----------------

    int overdueGoals = gm.countOverdueGoals();

    int pendingGoals = gm.countPendingGoals();

    int completedGoals = gm.countCompletedGoals();

    cout << "\nGOALS\n";

    cout << "  - " << overdueGoals << " OVERDUE, "
         << pendingGoals << " PENDING\n";

    cout << "  - " << completedGoals << " COMPLETED LIFETIME";

    string nextTitle = gm.getNextTitleName();

    if (!nextTitle.empty())
    {

        int remaining = gm.goalsUntilNextTitle();

        cout << " -- " << remaining << " MORE TO REACH \""
             << nextTitle << "\"";
    }

    else
    {

        cout << " -- MAXIMUM TITLE ALREADY UNLOCKED!";
    }

    cout << "\n";

    // ---------------- EXPENSES ----------------

    double totalSpent = em.calculateTotalExpense();

    cout << "\nEXPENSES\n";

    cout << "  - TOTAL SPENT: " << totalSpent << "\n";

    pair<string, double> topCategory = em.getTopCategoryInfo();

    if (!topCategory.first.empty())
    {

        cout << "  - HIGHEST CATEGORY: " << topCategory.first
             << " (" << topCategory.second << ")\n";
    }

    cout << "==================================================\n";

    // FULL PENDING-TASK LIST STILL SHOWN BELOW THE SUMMARY

    if (pendingTasks > 0)
    {

        cout << "\nFOLLOWING ARE YOUR PENDING TASKS:\n";

        tm.displayPendingTasks();
    }
}

// ============================================================
//                     LOGGED-IN USER MENU
// ============================================================
// Shown only AFTER a successful login. "Logout" returns control
// back to the Register/Login start screen in main().
// ============================================================

void loggedInMenu(User &user, Authentication &auth)
{

    int choice;

    do
    {

        cout << "\n================ USER MENU (" << user.getDisplayName() << ") ================\n";

        cout << "1. View Profile\n";

        cout << "2. Update Profile\n";

        cout << "3. Task Manager\n";

        cout << "4. Expense Manager\n";

        cout << "5. Goal Manager\n";

        cout << "6. Change Password\n";

        cout << "7. Logout\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        case 1:

            user.displayProfile();

            break;

        case 2:

            user.updateProfile();

            break;

        case 3:

            taskMenu(user);

            break;

        case 4:

            expenseMenu(user);

            break;

        case 5:

            goalMenu(user);

            break;

        case 6:

            auth.changePassword(user);

            break;

        case 7:

            cout << "LOGGING OUT...\n";

            break;

        default:

            cout << "INVALID CHOICE!\n";
        }

    } while (choice != 7);
}

// ============================================================
//                           MAIN
// ============================================================
// Starts with a Register / Login / Exit screen (supports
// multiple accounts). Only after a successful login does the
// person see the welcome message + pending-task notification
// and enter their personal Task/Expense/Goal menus.
// ============================================================

int main()
{

    vector<User> users;

    Authentication auth;

    int choice;

    do
    {

        cout << "\n================ LIFE MANAGER ================\n";

        cout << "1. Register\n";

        cout << "2. Login\n";

        cout << "3. Exit\n";

        cout << "ENTER CHOICE: ";

        cin >> choice;

        cin.ignore(1000, '\n');

        switch (choice)
        {

        // ========================================================
        //                        REGISTER
        // ========================================================

        case 1:
        {

            cout << "\n========== CREATE ACCOUNT ==========\n";

            string name;

            cout << "ENTER NAME: ";

            getline(cin, name);

            // PREVENT DUPLICATE ACCOUNT NAMES

            bool exists = false;

            for (int i = 0; i < (int)users.size(); i++)
            {

                if (users[i].getName() == name)
                {

                    exists = true;

                    break;
                }
            }

            if (exists)
            {

                cout << "AN ACCOUNT WITH THIS NAME ALREADY EXISTS! PLEASE LOGIN INSTEAD.\n";

                break;
            }

            Date dob;

            cout << "ENTER DATE OF BIRTH:\n";

            dob.inputDate();

            string password;

            cout << "SET PASSWORD: ";

            getline(cin, password);

            users.push_back(User(name, dob, password));

            cout << "ACCOUNT CREATED SUCCESSFULLY! YOU CAN NOW LOGIN.\n";

            break;
        }

        // ========================================================
        //                          LOGIN
        // ========================================================

        case 2:
        {

            if (users.empty())
            {

                cout << "NO ACCOUNTS REGISTERED YET! PLEASE REGISTER FIRST.\n";

                break;
            }

            string name;

            cout << "ENTER NAME: ";

            getline(cin, name);

            User *foundUser = nullptr;

            for (int i = 0; i < (int)users.size(); i++)
            {

                if (users[i].getName() == name)
                {

                    foundUser = &users[i];

                    break;
                }
            }

            if (foundUser == nullptr)
            {

                cout << "NO ACCOUNT FOUND WITH THIS NAME!\n";

                break;
            }

            string pass;

            cout << "ENTER PASSWORD: ";

            getline(cin, pass);

            // ON SUCCESSFUL LOGIN, WELCOME THE USER (WITH TITLE IF ANY),
            // SHOW THEIR DASHBOARD, AND OPEN THEIR PERSONAL MENU

            if (auth.authenticate(*foundUser, pass))
            {

                showDashboard(*foundUser);

                loggedInMenu(*foundUser, auth);
            }

            break;
        }

        // ========================================================
        //                          EXIT
        // ========================================================

        case 3:

            cout << "EXITING LIFE MANAGER...\n";

            break;

        default:

            cout << "INVALID CHOICE!\n";
        }

    } while (choice != 3);

    return 0;
}
