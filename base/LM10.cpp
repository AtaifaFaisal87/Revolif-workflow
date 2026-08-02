// ============================================================
//              LIFE MANAGER - UPDATED VERSION
// ============================================================
// This version includes:
//   - System class as central controller
//   - Admin class with separate authentication
//   - Achievement system with default + custom achievements
//   - User activity tracking (active/inactive status)
//   - Featured achievement display on welcome screen
//   - User privacy rules (admin cannot access private data)
//   - Hidden admin login inside normal login flow
// ============================================================

// Username: admin
// Password: admin123

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <utility>
#include <algorithm>
#include <cctype>

using namespace std;

// ============================================================
//                     GLOBAL HELPER FUNCTIONS
// ============================================================

string toLowerStr(string s)
{
    for (int i = 0; i < (int)s.size(); i++)
    {
        s[i] = tolower((unsigned char)s[i]);
    }
    return s;
}

bool containsIgnoreCase(const string &haystack, const string &needle)
{
    string h = toLowerStr(haystack);
    string n = toLowerStr(needle);
    return h.find(n) != string::npos;
}

string getNonEmptyLine(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);
        if (value.empty())
        {
            cout << "THIS FIELD CANNOT BE EMPTY! PLEASE TRY AGAIN.\n";
            continue;
        }
        return value;
    }
}

double getNonNegativeAmount(string prompt)
{
    double amount;
    while (true)
    {
        cout << prompt;
        cin >> amount;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "INVALID AMOUNT! PLEASE ENTER A NUMBER.\n";
            continue;
        }
        cin.ignore(1000, '\n');
        if (amount < 0)
        {
            cout << "AMOUNT CANNOT BE NEGATIVE! PLEASE TRY AGAIN.\n";
            continue;
        }
        return amount;
    }
}

bool confirmAction(string message)
{
    int choice;
    while (true)
    {
        cout << "\n" << message << "\n";
        cout << "1. Yes\n";
        cout << "2. No\n";
        cout << "ENTER CHOICE: ";
        cin >> choice;
        cin.ignore(1000, '\n');
        if (choice == 1) return true;
        if (choice == 2) return false;
        cout << "INVALID CHOICE!\n";
    }
}

// ============================================================
//                          DATE CLASS
// ============================================================

class Date
{
private:
    int day;
    int month;
    int year;

    static bool isLeapYear(int y)
    {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    static int daysInMonth(int m, int y)
    {
        int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (m == 2 && isLeapYear(y)) return 29;
        return days[m - 1];
    }

public:
    Date() : day(1), month(1), year(2000) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    static Date getToday()
    {
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);
        return Date(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);
    }

    long toComparable() const
    {
        return (long)year * 10000 + (long)month * 100 + (long)day;
    }

    bool isPastDate() const
    {
        return toComparable() < getToday().toComparable();
    }

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

    long daysUntil(const Date &other) const
    {
        return other.toDayNumber() - toDayNumber();
    }

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

            if (cin.fail())
            {
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

            if (m < 1 || m > 12)
            {
                cout << "INVALID MONTH! MONTH MUST BE BETWEEN 1-12.\n";
                continue;
            }
            if (y < 1900 || y > 2100)
            {
                cout << "INVALID YEAR! YEAR MUST BE BETWEEN 1900-2100.\n";
                continue;
            }
            int maxDay = daysInMonth(m, y);
            if (d < 1 || d > maxDay)
            {
                cout << "INVALID DAY! DAY MUST BE BETWEEN 1-" << maxDay
                     << " FOR THE SELECTED MONTH.\n";
                continue;
            }

            day = d;
            month = m;
            year = y;

            if (restrictPast && isPastDate())
            {
                cout << "ERROR! THIS DATE HAS ALREADY PASSED.\n";
                cout << "PLEASE ENTER TODAY'S DATE OR A FUTURE DATE.\n";
                continue;
            }
            break;
        }
    }

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

    bool isEqual(Date d) const
    {
        return day == d.day && month == d.month && year == d.year;
    }
};

// ============================================================
//                          TIME CLASS
// ============================================================

class Time
{
private:
    int hour;
    int minute;
    string meridiem;

public:
    Time() : hour(12), minute(0), meridiem("PM") {}
    Time(int h, int m, string mer) : hour(h), minute(m), meridiem(mer) {}

    int getHour() const { return hour; }
    int getMinute() const { return minute; }
    string getMeridiem() const { return meridiem; }

    void inputTime()
    {
        while (true)
        {
            int h, m;
            string mer;
            cout << "ENTER HOUR(1-12): ";
            cin >> h;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "INVALID INPUT! PLEASE ENTER NUMBERS ONLY.\n";
                continue;
            }
            cout << "ENTER MINUTE(0-59): ";
            cin >> m;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "INVALID INPUT! PLEASE ENTER NUMBERS ONLY.\n";
                continue;
            }
            cin.ignore(1000, '\n');
            if (h < 1 || h > 12)
            {
                cout << "INVALID HOUR! HOUR MUST BE BETWEEN 1-12.\n";
                continue;
            }
            if (m < 0 || m > 59)
            {
                cout << "INVALID MINUTE! MINUTE MUST BE BETWEEN 0-59.\n";
                continue;
            }
            cout << "SELECT AM/PM:\n";
            cout << "1. AM\n";
            cout << "2. PM\n";
            cout << "ENTER CHOICE: ";
            int merChoice;
            cin >> merChoice;
            cin.ignore(1000, '\n');
            if (merChoice == 1) mer = "AM";
            else if (merChoice == 2) mer = "PM";
            else
            {
                cout << "INVALID CHOICE!\n";
                continue;
            }
            hour = h;
            minute = m;
            meridiem = mer;
            break;
        }
    }

    friend ostream &operator<<(ostream &out, const Time &t)
    {
        out << (t.hour < 10 ? "0" : "")
            << t.hour << ":"
            << (t.minute < 10 ? "0" : "")
            << t.minute << " "
            << t.meridiem;
        return out;
    }
};

// ============================================================
//                         TASK CLASS
// ============================================================
// Parent class - AcademicTask and DailyTask inherit from this.
// Stored using polymorphism through Task pointers.
// ============================================================

class Task
{
protected:
    int taskID;
    static int nextTaskID;
    string title;
    string description;
    Date deadline;
    Time deadlineTime;
    string category;
    string status;

public:
    Task(string t, string desc, Date d, Time tm, string c)
    {
        taskID = nextTaskID++;
        title = t;
        description = desc;
        deadline = d;
        deadlineTime = tm;
        category = c;
        status = "Pending";
    }

    virtual ~Task() {}

    int getTaskID() const { return taskID; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    Date getDeadline() const { return deadline; }
    Time getDeadlineTime() const { return deadlineTime; }
    string getCategory() const { return category; }
    string getStatus() const { return status; }

    void setTitle(string t) { title = t; }
    void setDescription(string d) { description = d; }
    void setDeadline(Date d) { deadline = d; }
    void setDeadlineTime(Time t) { deadlineTime = t; }

    void markCompleted() { status = "Completed"; }
    void markPending() { status = "Pending"; }

    virtual void displayTask() const = 0;
};

int Task::nextTaskID = 1;

// ============================================================
//                       ACADEMIC TASK CLASS
// ============================================================

class AcademicTask : public Task
{
public:
    AcademicTask(string t, string desc, Date d, Time tm)
        : Task(t, desc, d, tm, "Academic") {}

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
        case 1: return "Exam";
        case 2: return "Assignment";
        case 3: return "Project";
        case 4: return "Midterm";
        case 5: return "Test";
        case 6: return "Submission";
        case 7: return "Viva";
        case 8: return "Presentation";
        default:
            string other;
            cout << "ENTER TASK TITLE: ";
            getline(cin, other);
            return other;
        }
    }

    void displayTask() const override
    {
        cout << "\n====================================\n";
        cout << "TASK ID     : " << taskID << endl;
        cout << "CATEGORY    : " << category << endl;
        cout << "TITLE       : " << title << endl;
        cout << "DESCRIPTION : " << description << endl;
        cout << "DEADLINE    : " << deadline << "  " << deadlineTime << endl;
        cout << "STATUS      : " << status << endl;
        cout << "====================================\n";
    }
};

// ============================================================
//                         DAILY TASK CLASS
// ============================================================

class DailyTask : public Task
{
public:
    DailyTask(string t, string desc, Date d, Time tm)
        : Task(t, desc, d, tm, "Daily") {}

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
        case 1: return "Laundry";
        case 2: return "Cleaning House";
        case 3: return "Washing Dishes";
        case 4: return "Exercise";
        case 5: return "Grocery Shopping";
        case 6: return "Cooking";
        case 7: return "Reading";
        default:
            string other;
            cout << "ENTER TASK TITLE: ";
            getline(cin, other);
            return other;
        }
    }

    void displayTask() const override
    {
        cout << "\n====================================\n";
        cout << "TASK ID     : " << taskID << endl;
        cout << "CATEGORY    : " << category << endl;
        cout << "TITLE       : " << title << endl;
        cout << "DESCRIPTION : " << description << endl;
        cout << "DEADLINE    : " << deadline << "  " << deadlineTime << endl;
        cout << "STATUS      : " << status << endl;
        cout << "====================================\n";
    }
};

// ============================================================
//                       TASK MANAGER CLASS
// ============================================================
// Manages all tasks of a single user using polymorphism.
// ============================================================

class TaskManager
{
private:
    vector<Task *> tasks;

public:
    ~TaskManager()
    {
        for (int i = 0; i < tasks.size(); i++)
            delete tasks[i];
    }

    void addTask()
    {
        int choice;
        cout << "\n========== ADD TASK ==========\n";
        cout << "1. Academic Task\n";
        cout << "2. Daily Task\n";
        cout << "ENTER CHOICE: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        if (choice != 1 && choice != 2)
        {
            cout << "INVALID CHOICE!\n";
            return;
        }

        string title = (choice == 1) ? AcademicTask::getAcademicTitle()
                                      : DailyTask::getDailyTitle();
        string description = getNonEmptyLine("ENTER DESCRIPTION: ");
        Date deadline;
        cout << "\nENTER DEADLINE DATE:\n";
        deadline.inputDate(true);
        Time deadlineTime;
        cout << "\nENTER DEADLINE TIME:\n";
        deadlineTime.inputTime();

        if (choice == 1)
        {
            Task *task = new AcademicTask(title, description, deadline, deadlineTime);
            tasks.push_back(task);
            cout << "ACADEMIC TASK ADDED SUCCESSFULLY!\n";
        }
        else
        {
            Task *task = new DailyTask(title, description, deadline, deadlineTime);
            tasks.push_back(task);
            cout << "DAILY TASK ADDED SUCCESSFULLY!\n";
        }
    }

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
            cout << "\n" << (i + 1) << ".\n";
            tasks[i]->displayTask();
        }
    }

    void displayCompletedTasks() const
    {
        cout << "\n========== COMPLETED TASKS ==========\n";
        bool found = false;
        int number = 0;
        for (int i = 0; i < tasks.size(); i++)
        {
            if (tasks[i]->getStatus() == "Completed")
            {
                cout << "\n" << (++number) << ".\n";
                tasks[i]->displayTask();
                found = true;
            }
        }
        if (!found) cout << "NO COMPLETED TASKS!\n";
    }

    void displayPendingTasks() const
    {
        cout << "\n========== PENDING TASKS ==========\n";
        bool found = false;
        int number = 0;
        for (int i = 0; i < tasks.size(); i++)
        {
            if (tasks[i]->getStatus() == "Pending")
            {
                cout << "\n" << (++number) << ".\n";
                tasks[i]->displayTask();
                found = true;
            }
        }
        if (!found) cout << "NO PENDING TASKS!\n";
    }

    int countPendingTasks() const
    {
        int count = 0;
        for (int i = 0; i < (int)tasks.size(); i++)
            if (tasks[i]->getStatus() == "Pending") count++;
        return count;
    }

    int countOverdueTasks() const
    {
        int count = 0;
        for (int i = 0; i < (int)tasks.size(); i++)
            if (tasks[i]->getStatus() == "Pending" &&
                tasks[i]->getDeadline().isPastDate()) count++;
        return count;
    }

    int countCompletedTasks() const
    {
        int count = 0;
        for (int i = 0; i < (int)tasks.size(); i++)
            if (tasks[i]->getStatus() == "Completed") count++;
        return count;
    }

    int countDueSoonTasks(int days) const
    {
        int count = 0;
        Date today = Date::getToday();
        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->getStatus() == "Pending")
            {
                long diff = today.daysUntil(tasks[i]->getDeadline());
                if (diff >= 0 && diff <= days) count++;
            }
        }
        return count;
    }

    Task *searchByID(int id)
    {
        for (int i = 0; i < tasks.size(); i++)
            if (tasks[i]->getTaskID() == id) return tasks[i];
        return nullptr;
    }

    vector<Task *> searchByTitle(string title)
    {
        vector<Task *> result;
        for (int i = 0; i < tasks.size(); i++)
            if (containsIgnoreCase(tasks[i]->getTitle(), title))
                result.push_back(tasks[i]);
        return result;
    }

    vector<Task *> searchByCategory(string category)
    {
        vector<Task *> result;
        for (int i = 0; i < tasks.size(); i++)
            if (containsIgnoreCase(tasks[i]->getCategory(), category))
                result.push_back(tasks[i]);
        return result;
    }

    vector<Task *> searchByDeadline(Date date)
    {
        vector<Task *> result;
        for (int i = 0; i < tasks.size(); i++)
            if (tasks[i]->getDeadline().isEqual(date))
                result.push_back(tasks[i]);
        return result;
    }

    vector<Task *> searchByStatus(string status)
    {
        vector<Task *> result;
        for (int i = 0; i < tasks.size(); i++)
            if (containsIgnoreCase(tasks[i]->getStatus(), status))
                result.push_back(tasks[i]);
        return result;
    }

    void displaySearchResults(vector<Task *> result)
    {
        if (result.empty())
        {
            cout << "NO TASK FOUND!\n";
            return;
        }
        for (int i = 0; i < result.size(); i++)
            result[i]->displayTask();
    }

    void completeTaskFlow()
    {
        displayPendingTasks();
        if (countPendingTasks() == 0) return;
        int id;
        cout << "\nENTER TASK ID TO MARK AS COMPLETED: ";
        cin >> id;
        cin.ignore(1000, '\n');
        Task *task = searchByID(id);
        if (task == nullptr)
        {
            cout << "TASK NOT FOUND!\n";
            return;
        }
        task->markCompleted();
        cout << "TASK MARKED AS COMPLETED!\n";
    }

    void deleteTaskFlow()
    {
        displayAllTasks();
        if (tasks.empty()) return;
        int id;
        cout << "\nENTER TASK ID TO DELETE: ";
        cin >> id;
        cin.ignore(1000, '\n');
        for (int i = 0; i < tasks.size(); i++)
        {
            if (tasks[i]->getTaskID() == id)
            {
                if (!confirmAction("ARE YOU SURE YOU WANT TO DELETE THIS ITEM?"))
                {
                    cout << "DELETE CANCELLED!\n";
                    return;
                }
                delete tasks[i];
                tasks.erase(tasks.begin() + i);
                cout << "TASK DELETED SUCCESSFULLY!\n";
                return;
            }
        }
        cout << "TASK NOT FOUND!\n";
    }

    void updateTaskFlow()
    {
        displayAllTasks();
        if (tasks.empty()) return;
        int id;
        cout << "\nENTER TASK ID TO UPDATE: ";
        cin >> id;
        cin.ignore(1000, '\n');
        Task *task = searchByID(id);
        if (task == nullptr)
        {
            cout << "TASK NOT FOUND!\n";
            return;
        }
        int fieldChoice;
        cout << "\n========== UPDATE TASK ==========\n";
        cout << "1. Title\n";
        cout << "2. Description\n";
        cout << "3. Deadline\n";
        cout << "4. Time\n";
        cout << "5. Status\n";
        cout << "6. Cancel\n";
        cout << "ENTER CHOICE: ";
        cin >> fieldChoice;
        cin.ignore(1000, '\n');

        switch (fieldChoice)
        {
        case 1:
            task->setTitle(getNonEmptyLine("ENTER NEW TITLE: "));
            break;
        case 2:
            task->setDescription(getNonEmptyLine("ENTER NEW DESCRIPTION: "));
            break;
        case 3:
        {
            Date newDeadline;
            cout << "ENTER NEW DEADLINE DATE:\n";
            newDeadline.inputDate(true);
            task->setDeadline(newDeadline);
            break;
        }
        case 4:
        {
            Time newTime;
            cout << "ENTER NEW DEADLINE TIME:\n";
            newTime.inputTime();
            task->setDeadlineTime(newTime);
            break;
        }
        case 5:
        {
            int statusChoice;
            cout << "1. Pending\n";
            cout << "2. Completed\n";
            cout << "ENTER CHOICE: ";
            cin >> statusChoice;
            cin.ignore(1000, '\n');
            if (statusChoice == 1) task->markPending();
            else if (statusChoice == 2) task->markCompleted();
            else
            {
                cout << "INVALID CHOICE!\n";
                return;
            }
            break;
        }
        case 6:
            cout << "UPDATE CANCELLED!\n";
            return;
        default:
            cout << "INVALID CHOICE!\n";
            return;
        }
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
    Expense(string t, double a, string c, Date d, string desc)
    {
        expenseID = nextExpenseID++;
        title = t;
        amount = a;
        category = c;
        date = d;
        description = desc;
    }

    int getExpenseID() const { return expenseID; }
    string getTitle() const { return title; }
    double getAmount() const { return amount; }
    string getCategory() const { return category; }
    Date getDate() const { return date; }
    string getDescription() const { return description; }

    void setTitle(string t) { title = t; }
    void setAmount(double a) { amount = a; }
    void setCategory(string c) { category = c; }
    void setDate(Date d) { date = d; }
    void setDescription(string d) { description = d; }

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
    ~ExpenseManager()
    {
        for (int i = 0; i < expenses.size(); i++)
            delete expenses[i];
    }

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
        case 1: return "Food";
        case 2: return "Transport";
        case 3: return "Education";
        case 4: return "Shopping";
        case 5: return "Bills";
        case 6: return "Entertainment";
        case 7: return "Health";
        default:
            string other;
            cout << "ENTER CATEGORY NAME: ";
            getline(cin, other);
            return other;
        }
    }

    void addExpense()
    {
        cout << "\n========== ADD EXPENSE ==========\n";
        string category = selectCategory();
        string title = getNonEmptyLine("ENTER TITLE (e.g. \"KFC Dinner\", \"Uber Ride\"): ");
        double amount = getNonNegativeAmount("ENTER AMOUNT: ");
        Date date;
        cout << "ENTER DATE:\n";
        date.inputDate();
        string description = getNonEmptyLine("ENTER DESCRIPTION: ");
        Expense *expense = new Expense(title, amount, category, date, description);
        expenses.push_back(expense);
        cout << "EXPENSE ADDED SUCCESSFULLY!\n";
    }

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
            cout << "\n" << (i + 1) << ".\n";
            expenses[i]->displayExpense();
        }
    }

    Expense *searchByID(int id)
    {
        for (int i = 0; i < expenses.size(); i++)
            if (expenses[i]->getExpenseID() == id) return expenses[i];
        return nullptr;
    }

    vector<Expense *> searchByTitle(string title)
    {
        vector<Expense *> result;
        for (int i = 0; i < expenses.size(); i++)
            if (containsIgnoreCase(expenses[i]->getTitle(), title))
                result.push_back(expenses[i]);
        return result;
    }

    vector<Expense *> searchByCategory(string category)
    {
        vector<Expense *> result;
        for (int i = 0; i < expenses.size(); i++)
            if (containsIgnoreCase(expenses[i]->getCategory(), category))
                result.push_back(expenses[i]);
        return result;
    }

    vector<Expense *> searchByDate(Date date)
    {
        vector<Expense *> result;
        for (int i = 0; i < expenses.size(); i++)
            if (expenses[i]->getDate().isEqual(date))
                result.push_back(expenses[i]);
        return result;
    }

    void displaySearchResults(vector<Expense *> result)
    {
        if (result.empty())
        {
            cout << "NO EXPENSE FOUND!\n";
            return;
        }
        for (int i = 0; i < result.size(); i++)
            result[i]->displayExpense();
    }

    void deleteExpenseFlow()
    {
        displayAllExpenses();
        if (expenses.empty()) return;
        int id;
        cout << "\nENTER EXPENSE ID TO DELETE: ";
        cin >> id;
        cin.ignore(1000, '\n');
        for (int i = 0; i < expenses.size(); i++)
        {
            if (expenses[i]->getExpenseID() == id)
            {
                if (!confirmAction("ARE YOU SURE YOU WANT TO DELETE THIS ITEM?"))
                {
                    cout << "DELETE CANCELLED!\n";
                    return;
                }
                delete expenses[i];
                expenses.erase(expenses.begin() + i);
                cout << "EXPENSE DELETED SUCCESSFULLY!\n";
                return;
            }
        }
        cout << "EXPENSE NOT FOUND!\n";
    }

    void updateExpenseFlow()
    {
        displayAllExpenses();
        if (expenses.empty()) return;
        int id;
        cout << "\nENTER EXPENSE ID TO UPDATE: ";
        cin >> id;
        cin.ignore(1000, '\n');
        Expense *expense = searchByID(id);
        if (expense == nullptr)
        {
            cout << "EXPENSE NOT FOUND!\n";
            return;
        }
        int fieldChoice;
        cout << "\n========== UPDATE EXPENSE ==========\n";
        cout << "1. Title\n";
        cout << "2. Amount\n";
        cout << "3. Category\n";
        cout << "4. Date\n";
        cout << "5. Description\n";
        cout << "6. Cancel\n";
        cout << "ENTER CHOICE: ";
        cin >> fieldChoice;
        cin.ignore(1000, '\n');

        switch (fieldChoice)
        {
        case 1: expense->setTitle(getNonEmptyLine("ENTER NEW TITLE: ")); break;
        case 2: expense->setAmount(getNonNegativeAmount("ENTER NEW AMOUNT: ")); break;
        case 3: expense->setCategory(selectCategory()); break;
        case 4:
        {
            Date newDate;
            cout << "ENTER NEW DATE:\n";
            newDate.inputDate();
            expense->setDate(newDate);
            break;
        }
        case 5: expense->setDescription(getNonEmptyLine("ENTER NEW DESCRIPTION: ")); break;
        case 6: cout << "UPDATE CANCELLED!\n"; return;
        default: cout << "INVALID CHOICE!\n"; return;
        }
        cout << "EXPENSE UPDATED SUCCESSFULLY!\n";
    }

    double calculateTotalExpense()
    {
        double total = 0;
        for (int i = 0; i < expenses.size(); i++)
            total += expenses[i]->getAmount();
        return total;
    }

    pair<string, double> getTopCategoryInfo() const
    {
        if (expenses.empty()) return make_pair("", 0.0);

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
            if (totals[i] > totals[bestIndex]) bestIndex = i;
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
    Goal(string t, string desc, string c, Date d)
    {
        goalID = nextGoalID++;
        title = t;
        description = desc;
        category = c;
        deadline = d;
        status = "Incomplete";
    }

    int getGoalID() const { return goalID; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    string getCategory() const { return category; }
    Date getDeadline() const { return deadline; }
    string getStatus() const { return status; }

    string getDisplayStatus() const
    {
        if (status == "Completed") return "Completed";
        if (deadline.isPastDate()) return "Overdue";
        return "Pending";
    }

    void setTitle(string t) { title = t; }
    void setDescription(string d) { description = d; }
    void setCategory(string c) { category = c; }
    void setDeadline(Date d) { deadline = d; }

    void completeGoal() { status = "Completed"; }

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
    ~GoalManager()
    {
        for (int i = 0; i < goals.size(); i++)
            delete goals[i];
    }

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
        case 1: return "Academic";
        case 2: return "Career";
        case 3: return "Health";
        case 4: return "Personal Development";
        case 5: return "Financial";
        default:
            string other;
            cout << "ENTER CATEGORY NAME: ";
            getline(cin, other);
            return other;
        }
    }

    void addGoal()
    {
        string title, description;
        cout << "\n========== ADD GOAL ==========\n";
        cout << "ENTER GOAL TITLE: ";
        getline(cin, title);
        string category = selectCategory();
        Date deadline;
        cout << "ENTER DEADLINE:\n";
        deadline.inputDate(true);
        cout << "ENTER DESCRIPTION: ";
        getline(cin, description);
        Goal *goal = new Goal(title, description, category, deadline);
        goals.push_back(goal);
        cout << "GOAL ADDED SUCCESSFULLY!\n";
    }

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
            cout << "\n" << (i + 1) << ".\n";
            goals[i]->displayGoal();
        }
    }

    void displayCompletedGoals() const
    {
        bool found = false;
        int number = 0;
        cout << "\n========== COMPLETED GOALS ==========\n";
        for (int i = 0; i < goals.size(); i++)
        {
            if (goals[i]->getStatus() == "Completed")
            {
                cout << "\n" << (++number) << ".\n";
                goals[i]->displayGoal();
                found = true;
            }
        }
        if (!found) cout << "NO COMPLETED GOALS!\n";
    }

    void displayIncompleteGoals() const
    {
        bool found = false;
        int number = 0;
        cout << "\n========== PENDING / OVERDUE GOALS ==========\n";
        for (int i = 0; i < goals.size(); i++)
        {
            if (goals[i]->getStatus() != "Completed")
            {
                cout << "\n" << (++number) << ".\n";
                goals[i]->displayGoal();
                found = true;
            }
        }
        if (!found) cout << "NO PENDING GOALS!\n";
    }

    Goal *searchByID(int id)
    {
        for (int i = 0; i < goals.size(); i++)
            if (goals[i]->getGoalID() == id) return goals[i];
        return nullptr;
    }

    vector<Goal *> searchByTitle(string title)
    {
        vector<Goal *> result;
        for (int i = 0; i < goals.size(); i++)
            if (containsIgnoreCase(goals[i]->getTitle(), title))
                result.push_back(goals[i]);
        return result;
    }

    vector<Goal *> searchByCategory(string category)
    {
        vector<Goal *> result;
        for (int i = 0; i < goals.size(); i++)
            if (containsIgnoreCase(goals[i]->getCategory(), category))
                result.push_back(goals[i]);
        return result;
    }

    vector<Goal *> searchByDeadline(Date date)
    {
        vector<Goal *> result;
        for (int i = 0; i < goals.size(); i++)
            if (goals[i]->getDeadline().isEqual(date))
                result.push_back(goals[i]);
        return result;
    }

    vector<Goal *> searchByStatus(string status)
    {
        vector<Goal *> result;
        for (int i = 0; i < goals.size(); i++)
            if (containsIgnoreCase(goals[i]->getStatus(), status))
                result.push_back(goals[i]);
        return result;
    }

    void displaySearchResults(vector<Goal *> result)
    {
        if (result.empty())
        {
            cout << "NO GOAL FOUND!\n";
            return;
        }
        for (int i = 0; i < result.size(); i++)
            result[i]->displayGoal();
    }

    int countCompletedGoals() const
    {
        int count = 0;
        for (int i = 0; i < goals.size(); i++)
            if (goals[i]->getStatus() == "Completed") count++;
        return count;
    }

    int countPendingGoals() const
    {
        int count = 0;
        for (int i = 0; i < (int)goals.size(); i++)
            if (goals[i]->getStatus() != "Completed" &&
                !goals[i]->getDeadline().isPastDate()) count++;
        return count;
    }

    int countOverdueGoals() const
    {
        int count = 0;
        for (int i = 0; i < (int)goals.size(); i++)
            if (goals[i]->getStatus() != "Completed" &&
                goals[i]->getDeadline().isPastDate()) count++;
        return count;
    }

    int goalsUntilNextTitle() const
    {
        int count = countCompletedGoals();
        static const int milestoneCounts[] = {5, 10, 20, 30, 40, 50};
        for (int i = 0; i < 6; i++)
            if (count < milestoneCounts[i]) return milestoneCounts[i] - count;
        return 0;
    }

    string getNextTitleName() const
    {
        int count = countCompletedGoals();
        static const int milestoneCounts[] = {5, 10, 20, 30, 40, 50};
        static const string milestoneTitles[] = {
            "Goal Beginner", "Goal Enthusiast", "Goal Achiever",
            "Goal Master", "Goal Champion", "Goal Legend"};
        for (int i = 0; i < 6; i++)
            if (count < milestoneCounts[i]) return milestoneTitles[i];
        return "";
    }

    // ========================================================
    //                    COMPLETE GOAL
    // ========================================================
    // MODIFIED: Returns true if goal was actually completed
    // so System can trigger achievement checking.
    // ========================================================

    bool completeGoal(int id)
    {
        Goal *goal = searchByID(id);
        if (goal == nullptr)
        {
            cout << "GOAL NOT FOUND!\n";
            return false;
        }
        if (goal->getStatus() == "Completed")
        {
            cout << "GOAL ALREADY COMPLETED!\n";
            return false;
        }
        goal->completeGoal();
        updateBadges();
        cout << "GOAL COMPLETED SUCCESSFULLY!\n";
        return true;
    }

    // ========================================================
    //          COMPLETE WORKFLOW: DISPLAY -> SELECT -> MARK
    // ========================================================
    // MODIFIED: Returns true if goal was completed so System
    // can check achievements after this call.
    // ========================================================

    bool completeGoalFlow()
    {
        displayIncompleteGoals();
        if (goals.empty()) return false;
        int id;
        cout << "\nENTER GOAL ID TO MARK AS COMPLETED: ";
        cin >> id;
        cin.ignore(1000, '\n');
        return completeGoal(id);
    }

    void updateBadges()
    {
        int count = countCompletedGoals();
        static const int milestoneCounts[] = {5, 10, 20, 30, 40, 50};
        static const string milestoneTitles[] = {
            "Goal Beginner", "Goal Enthusiast", "Goal Achiever",
            "Goal Master", "Goal Champion", "Goal Legend"};
        int totalMilestones = 6;
        if ((int)badges.size() < totalMilestones)
        {
            int nextIndex = badges.size();
            if (count >= milestoneCounts[nextIndex])
                badges.push_back(milestoneTitles[nextIndex]);
        }
    }

    string getLatestTitle() const
    {
        if (badges.empty()) return "";
        return badges.back();
    }

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
            cout << "[TITLE UNLOCKED] " << badges[i] << endl;
    }

    void updateGoalFlow()
    {
        displayAllGoals();
        if (goals.empty()) return;
        int id;
        cout << "\nENTER GOAL ID TO UPDATE: ";
        cin >> id;
        cin.ignore(1000, '\n');
        Goal *goal = searchByID(id);
        if (goal == nullptr)
        {
            cout << "GOAL NOT FOUND!\n";
            return;
        }
        int fieldChoice;
        cout << "\n========== UPDATE GOAL ==========\n";
        cout << "1. Title\n";
        cout << "2. Description\n";
        cout << "3. Category\n";
        cout << "4. Deadline\n";
        cout << "5. Cancel\n";
        cout << "ENTER CHOICE: ";
        cin >> fieldChoice;
        cin.ignore(1000, '\n');

        switch (fieldChoice)
        {
        case 1: goal->setTitle(getNonEmptyLine("ENTER NEW TITLE: ")); break;
        case 2: goal->setDescription(getNonEmptyLine("ENTER NEW DESCRIPTION: ")); break;
        case 3: goal->setCategory(selectCategory()); break;
        case 4:
        {
            Date newDeadline;
            cout << "ENTER NEW DEADLINE:\n";
            newDeadline.inputDate(true);
            goal->setDeadline(newDeadline);
            break;
        }
        case 5: cout << "UPDATE CANCELLED!\n"; return;
        default: cout << "INVALID CHOICE!\n"; return;
        }
        cout << "GOAL UPDATED SUCCESSFULLY!\n";
    }

    void deleteGoalFlow()
    {
        displayAllGoals();
        if (goals.empty()) return;
        int id;
        cout << "\nENTER GOAL ID TO DELETE: ";
        cin >> id;
        cin.ignore(1000, '\n');
        for (int i = 0; i < goals.size(); i++)
        {
            if (goals[i]->getGoalID() == id)
            {
                if (!confirmAction("ARE YOU SURE YOU WANT TO DELETE THIS ITEM?"))
                {
                    cout << "DELETE CANCELLED!\n";
                    return;
                }
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
// ============================================================
//                   NEW / UPDATED CLASSES
// ============================================================
// ============================================================

// ============================================================
//                     ACHIEVEMENT CLASS
// ============================================================
// Represents an achievement definition in the system.
// System owns a vector of Achievement objects.
// Users only store achievement IDs (int), not full objects.
//
// isDefault = true  -> cannot be deleted or modified by admin
// isDefault = false -> admin can edit and remove
// ============================================================

class Achievement
{
private:
    int achievementID;
    static int nextAchievementID;
    string name;
    string description;
    int requiredGoals;      // Number of completed goals required to unlock
    bool isDefault;         // true = hardcoded default, false = admin-created

public:
    // Constructor for default achievements (hardcoded)
    Achievement(string n, string desc, int reqGoals, bool def = false)
    {
        achievementID = nextAchievementID++;
        name = n;
        description = desc;
        requiredGoals = reqGoals;
        isDefault = def;
    }

    // Getters
    int getAchievementID() const { return achievementID; }
    string getName() const { return name; }
    string getDescription() const { return description; }
    int getRequiredGoals() const { return requiredGoals; }
    bool getIsDefault() const { return isDefault; }

    // Setters (only for non-default achievements)
    void setName(string n) { name = n; }
    void setDescription(string d) { description = d; }
    void setRequiredGoals(int req) { requiredGoals = req; }

    void displayAchievement() const
    {
        cout << "\n====================================\n";
        cout << "ACHIEVEMENT ID : " << achievementID << endl;
        cout << "NAME           : " << name << endl;
        cout << "DESCRIPTION    : " << description << endl;
        cout << "REQUIREMENT    : Complete " << requiredGoals << " Goals" << endl;
        cout << "TYPE           : " << (isDefault ? "Default (Protected)" : "Custom") << endl;
        cout << "====================================\n";
    }

    void displayBrief() const
    {
        cout << "[" << achievementID << "] " << name
             << " (Complete " << requiredGoals << " Goals)";
        if (isDefault) cout << " [DEFAULT]";
        cout << endl;
    }
};

int Achievement::nextAchievementID = 1;

// ============================================================
//                      ADMIN CLASS
// ============================================================
// Admin has its own authentication credentials.
// Admin does NOT own or directly manage users.
// System owns users. Admin only provides administrative
// functionality like viewing user summaries and managing
// custom achievements.
//
// PRIVACY RULE: Admin cannot access private user data:
//   - Tasks, Expenses, Goals, Health records, Notes, AI chats
// Admin CAN view:
//   - Username, Full Name, Email, Registration Date,
//     Last Login, Account Status, Achievements Earned,
//     Number of Goals Completed
// ============================================================

class Admin
{
private:
    string username;
    string password;

public:
    Admin(string u = "admin", string p = "admin123")
        : username(u), password(p) {}

    bool authenticate(string u, string p) const
    {
        return (username == u && password == p);
    }

    string getUsername() const { return username; }

    void changePassword()
    {
        string oldPass = getNonEmptyLine("ENTER CURRENT PASSWORD: ");
        if (oldPass != password)
        {
            cout << "WRONG PASSWORD!\n";
            return;
        }
        string newPass = getNonEmptyLine("ENTER NEW PASSWORD: ");
        password = newPass;
        cout << "ADMIN PASSWORD CHANGED SUCCESSFULLY!\n";
    }
};

// Forward declaration of System (needed for User class)
class System;

// ============================================================
//                     USER CLASS (UPDATED)
// ============================================================
// User owns TaskManager, ExpenseManager, GoalManager (Composition).
// NEW: Added activity tracking (isActive, lastLogin).
// NEW: Added achievement storage (unlockedAchievementIDs).
// NEW: Added displayedAchievementID for featured achievement.
//
// Relationships:
//   User -> TaskManager, ExpenseManager, GoalManager
//   User -> vector<int> unlockedAchievementIDs
//   User -> int displayedAchievementID
// ============================================================

class User
{
private:
    string name;
    string username;
    int UID;
    static int next_id;
    Date DOB;
    string password;
    string email;
    Date registrationDate;

    // Title earned through completing goals (old badge system)
    string title;

    // ========== NEW: ACTIVITY TRACKING ==========
    bool isActive;          // true = active user, false = inactive
    Date lastLogin;         // Date of last successful login

    // ========== NEW: ACHIEVEMENT STORAGE ==========
    // Users store only achievement IDs, NOT full Achievement objects
    vector<int> unlockedAchievementIDs;
    int displayedAchievementID;   // -1 = none selected

    // Composition: each user has their own managers
    TaskManager taskManager;
    ExpenseManager expenseManager;
    GoalManager goalManager;

    friend class System;    // System needs access to private fields

public:
    // Default constructor
    User()
        : UID(next_id++),
          name("NONE"),
          username(""),
          password(""),
          email(""),
          title(""),
          isActive(false),
          displayedAchievementID(-1)
    {
        registrationDate = Date::getToday();
        lastLogin = Date(); // Default date (1/1/2000) = never logged in
    }

    // Parameterized constructor
    User(string n, string uname, Date dob, string pass, string em = "")
        : name(n),
          username(uname),
          UID(next_id++),
          DOB(dob),
          password(pass),
          email(em),
          title(""),
          isActive(false),
          displayedAchievementID(-1)
    {
        registrationDate = Date::getToday();
        lastLogin = Date(); // Default date = never logged in
    }

    // ================= GETTERS =================
    string getName() const { return name; }
    string getUsername() const { return username; }
    int getUID() const { return UID; }
    Date getDOB() const { return DOB; }
    string getEmail() const { return email; }
    Date getRegistrationDate() const { return registrationDate; }
    string getTitle() const { return title; }
    bool getIsActive() const { return isActive; }
    Date getLastLogin() const { return lastLogin; }
    int getDisplayedAchievementID() const { return displayedAchievementID; }
    vector<int> getUnlockedAchievementIDs() const { return unlockedAchievementIDs; }

    // ================= SETTERS =================
    void setName(string n) { name = n; }
    void setEmail(string e) { email = e; }
    void setActive(bool active) { isActive = active; }
    void setLastLogin(Date d) { lastLogin = d; }
    void setDisplayedAchievementID(int id) { displayedAchievementID = id; }

    // ================= ACTIVITY TRACKING =================
    void recordLogin()
    {
        isActive = true;
        lastLogin = Date::getToday();
    }

    // ================= ACHIEVEMENT METHODS =================
    void addUnlockedAchievement(int achID)
    {
        // Check if already unlocked
        for (int i = 0; i < (int)unlockedAchievementIDs.size(); i++)
            if (unlockedAchievementIDs[i] == achID) return;
        unlockedAchievementIDs.push_back(achID);
    }

    bool hasAchievement(int achID) const
    {
        for (int i = 0; i < (int)unlockedAchievementIDs.size(); i++)
            if (unlockedAchievementIDs[i] == achID) return true;
        return false;
    }

    int getUnlockedCount() const
    {
        return unlockedAchievementIDs.size();
    }

    // ================= DISPLAY NAME =================
    string getDisplayName() const
    {
        if (title.empty()) return name;
        return name + " (" + title + ")";
    }

    // ================= MANAGER ACCESS =================
    TaskManager &getTaskManager() { return taskManager; }
    ExpenseManager &getExpenseManager() { return expenseManager; }
    GoalManager &getGoalManager() { return goalManager; }

    // ================= LOGIN CHECK =================
    bool login(string pass) const { return password == pass; }

    // ================= PROFILE UPDATE =================
    void updateProfile()
    {
        int choice;
        cout << "\n========== UPDATE PROFILE ==========\n";
        cout << "1. Update Name\n";
        cout << "2. Update Email\n";
        cout << "3. Update DOB\n";
        cout << "4. Cancel\n";
        cout << "ENTER CHOICE: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice)
        {
        case 1:
            name = getNonEmptyLine("ENTER NEW NAME: ");
            cout << "NAME UPDATED!\n";
            break;
        case 2:
            email = getNonEmptyLine("ENTER NEW EMAIL: ");
            cout << "EMAIL UPDATED!\n";
            break;
        case 3:
            cout << "ENTER NEW DOB:\n";
            DOB.inputDate();
            cout << "DOB UPDATED!\n";
            break;
        case 4:
            cout << "UPDATE CANCELLED!\n";
            break;
        default:
            cout << "INVALID CHOICE!\n";
        }
    }

    // ================= DISPLAY PROFILE =================
    void displayProfile() const
    {
        cout << "\n================ USER PROFILE ================\n";
        cout << "UID              : " << UID << endl;
        cout << "USERNAME         : " << username << endl;
        cout << "NAME             : " << getDisplayName() << endl;
        cout << "EMAIL            : " << (email.empty() ? "N/A" : email) << endl;
        cout << "DOB              : " << DOB << endl;
        cout << "REGISTRATION DATE: " << registrationDate << endl;
        cout << "LAST LOGIN       : "; if (lastLogin.toComparable() == 20000101) cout << "Never"; else cout << lastLogin; cout << endl;
        cout << "ACCOUNT STATUS   : " << (isActive ? "Active" : "Inactive") << endl;
        cout << "ACHIEVEMENTS     : " << unlockedAchievementIDs.size() << " unlocked" << endl;
        cout << "==============================================\n";
    }

    // ================= WELCOME SCREEN =================
    // Displays username + featured achievement (if selected)
    void showWelcome(const vector<Achievement> &achievements) const
    {
        cout << "\n========== WELCOME " << name << " ==========\n";

        if (displayedAchievementID != -1)
        {
            // Find the achievement name from the system library
            string achName = "Unknown Achievement";
            for (int i = 0; i < (int)achievements.size(); i++)
            {
                if (achievements[i].getAchievementID() == displayedAchievementID)
                {
                    achName = achievements[i].getName();
                    break;
                }
            }
            cout << "\nFeatured Achievement:\n";
            cout << "   " << char(4) << " " << achName << "\n";
        }
        else
        {
            cout << "\nNo featured achievement selected.\n";
            cout << "Complete goals and select an achievement to display.\n";
        }

        cout << "\nGoals Completed: " << goalManager.countCompletedGoals() << endl;
        cout << "Tasks Pending: " << taskManager.countPendingTasks() << endl;
        cout << "==============================================\n";
    }

    // ================= CHANGE PASSWORD =================
    void changePassword()
    {
        string oldPass = getNonEmptyLine("ENTER CURRENT PASSWORD: ");
        if (!login(oldPass))
        {
            cout << "WRONG PASSWORD!\n";
            return;
        }
        string newPass = getNonEmptyLine("ENTER NEW PASSWORD: ");
        password = newPass;
        cout << "PASSWORD CHANGED SUCCESSFULLY!\n";
    }

    // ================= CHECK AND UPDATE TITLE (OLD SYSTEM) =================
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
};

int User::next_id = 1;

// ============================================================
//                      SYSTEM CLASS
// ============================================================
// Central controller of the application.
// Owns: vector<User*>, Admin object, vector<Achievement>
//
// Responsibilities:
//   - Register users (creates User objects)
//   - Login users (hidden admin check first)
//   - Manage achievements (default + custom)
//   - Check achievements when goals are completed
//   - Handle system-level operations
//
// Relationship:
//   System
//   |---- vector<User*>
//   |---- Admin
//   |---- vector<Achievement>
// ============================================================

class System
{
private:
    vector<User*> users;
    Admin admin;
    vector<Achievement> achievements;

public:
    // Constructor: loads default achievements automatically
    System()
    {
        loadDefaultAchievements();
    }

    // Destructor: clean up dynamically allocated users
    ~System()
    {
        for (int i = 0; i < (int)users.size(); i++)
            delete users[i];
    }

    // ========================================================
    //              LOAD DEFAULT ACHIEVEMENTS
    // ========================================================
    // Called automatically when System is created.
    // Hardcoded achievements that cannot be deleted/modified.
    // ========================================================

    void loadDefaultAchievements()
    {
        // ID 1: First Goal - Complete 1 Goal
        achievements.push_back(Achievement("First Goal", "Complete your very first goal!", 1, true));
        // ID 2: Rising Star - Complete 5 Goals
        achievements.push_back(Achievement("Rising Star", "Complete 5 goals to become a rising star!", 5, true));
        // ID 3: Goal Hunter - Complete 10 Goals
        achievements.push_back(Achievement("Goal Hunter", "Complete 10 goals - you're on a roll!", 10, true));
        // ID 4: Silver Achiever - Complete 25 Goals
        achievements.push_back(Achievement("Silver Achiever", "Complete 25 goals - impressive dedication!", 25, true));
        // ID 5: Gold Achiever - Complete 50 Goals
        achievements.push_back(Achievement("Gold Achiever", "Complete 50 goals - truly golden!", 50, true));
        // ID 6: Legend - Complete 100 Goals
        achievements.push_back(Achievement("Legend", "Complete 100 goals - you are a legend!", 100, true));
    }

    // ========================================================
    //              GET ACHIEVEMENT BY ID
    // ========================================================

    Achievement *getAchievementByID(int id)
    {
        for (int i = 0; i < (int)achievements.size(); i++)
            if (achievements[i].getAchievementID() == id)
                return &achievements[i];
        return nullptr;
    }

    // ========================================================
    //              GET ALL ACHIEVEMENTS (const ref)
    // ========================================================

    const vector<Achievement> &getAchievements() const
    {
        return achievements;
    }

    // ========================================================
    //              REGISTER USER
    // ========================================================
    // System creates User objects and stores them.
    // Registration belongs to System, not User.
    // ========================================================

    void registerUser()
    {
        cout << "\n========== CREATE ACCOUNT ==========\n";

        // Full name
        string name = getNonEmptyLine("ENTER FULL NAME: ");

        // Username - must be unique
        string username;
        while (true)
        {
            username = getNonEmptyLine("ENTER USERNAME: ");
            if (findUserByUsername(username) != nullptr)
            {
                cout << "THIS USERNAME IS ALREADY TAKEN! PLEASE CHOOSE ANOTHER.\n";
                continue;
            }
            break;
        }

        // Email
        string email = getNonEmptyLine("ENTER EMAIL: ");

        // Date of birth
        Date dob;
        cout << "ENTER DATE OF BIRTH:\n";
        dob.inputDate();

        // Password + confirm
        string password, confirmPassword;
        while (true)
        {
            password = getNonEmptyLine("SET PASSWORD: ");
            confirmPassword = getNonEmptyLine("CONFIRM PASSWORD: ");
            if (password != confirmPassword)
            {
                cout << "PASSWORDS DO NOT MATCH! PLEASE TRY AGAIN.\n";
                continue;
            }
            break;
        }

        // Create and store new user
        User *newUser = new User(name, username, dob, password, email);
        users.push_back(newUser);

        cout << "ACCOUNT CREATED SUCCESSFULLY! YOU CAN NOW LOGIN.\n";
    }

    // ========================================================
    //              FIND USER BY USERNAME
    // ========================================================

    User *findUserByUsername(string username)
    {
        for (int i = 0; i < (int)users.size(); i++)
            if (users[i]->getUsername() == username)
                return users[i];
        return nullptr;
    }

    // ========================================================
    //              LOGIN FLOW
    // ========================================================
    // Hidden admin check first.
    // If admin credentials -> open admin menu.
    // Otherwise -> authenticate normal user -> open user menu.
    // ========================================================

    void loginFlow()
    {
        if (users.empty())
        {
            cout << "NO ACCOUNTS REGISTERED YET! PLEASE REGISTER FIRST.\n";
            return;
        }

        string username = getNonEmptyLine("ENTER USERNAME: ");
        string pass = getNonEmptyLine("ENTER PASSWORD: ");

        // STEP 1: Check if credentials belong to Admin
        if (admin.authenticate(username, pass))
        {
            cout << "\nADMIN LOGIN SUCCESSFUL!\n";
            adminMenu();
            return;
        }

        // STEP 2: Authenticate normal user
        User *user = findUserByUsername(username);
        if (user == nullptr)
        {
            cout << "NO ACCOUNT FOUND WITH THIS USERNAME!\n";
            return;
        }

        if (!user->login(pass))
        {
            cout << "INVALID PASSWORD!\n";
            return;
        }

        // Successful user login
        cout << "\nLOGIN SUCCESSFUL!\n";
        user->recordLogin();            // Update activity tracking
        user->checkAndUpdateTitle();    // Check old title system
        user->showWelcome(achievements); // Show welcome with featured achievement
        showDashboard(*user);
        userMenu(*user);
    }

    // ========================================================
    //              CHECK ACHIEVEMENTS FOR USER
    // ========================================================
    // Called after a goal is completed.
    // System checks all achievements against user's completed goals.
    // Unlocks any newly earned achievements.
    // ========================================================

    void checkAchievements(User *user)
    {
        if (user == nullptr) return;

        int completedGoals = user->getGoalManager().countCompletedGoals();

        for (int i = 0; i < (int)achievements.size(); i++)
        {
            int achID = achievements[i].getAchievementID();
            int reqGoals = achievements[i].getRequiredGoals();

            // If user hasn't unlocked this achievement yet
            if (!user->hasAchievement(achID))
            {
                // Check if requirement is met
                if (completedGoals >= reqGoals)
                {
                    user->addUnlockedAchievement(achID);
                    cout << "\n" << char(4) << " ACHIEVEMENT UNLOCKED: "
                         << achievements[i].getName() << "!\n";
                }
            }
        }
    }

    // ========================================================
    //              VIEW USER ACHIEVEMENTS
    // ========================================================
    // Displays all achievements with unlock status.
    // ========================================================

    void viewUserAchievements(User *user)
    {
        if (user == nullptr) return;

        cout << "\n========== YOUR ACHIEVEMENTS ==========\n";
        cout << "Goals Completed: " << user->getGoalManager().countCompletedGoals() << "\n\n";

        for (int i = 0; i < (int)achievements.size(); i++)
        {
            int achID = achievements[i].getAchievementID();
            bool unlocked = user->hasAchievement(achID);

            cout << (unlocked ? "[UNLOCKED]   " : "[LOCKED]     ");
            cout << achievements[i].getName();
            cout << " (Complete " << achievements[i].getRequiredGoals() << " Goals)";
            if (achievements[i].getIsDefault()) cout << " [DEFAULT]";
            cout << endl;
        }
        cout << "=======================================\n";
    }

    // ========================================================
    //              CHOOSE DISPLAY ACHIEVEMENT
    // ========================================================
    // User selects ONE unlocked achievement to display
    // beside their name on the welcome/profile screen.
    // Only unlocked achievements can be selected.
    // ========================================================

    void chooseDisplayAchievement(User *user)
    {
        if (user == nullptr) return;

        vector<int> unlocked = user->getUnlockedAchievementIDs();

        if (unlocked.empty())
        {
            cout << "\nNO ACHIEVEMENTS UNLOCKED YET!\n";
            cout << "Complete goals to unlock achievements.\n";
            return;
        }

        cout << "\n========== CHOOSE ACHIEVEMENT ==========\n";
        cout << "Your unlocked achievements:\n\n";

        for (int i = 0; i < (int)unlocked.size(); i++)
        {
            Achievement *ach = getAchievementByID(unlocked[i]);
            if (ach != nullptr)
            {
                cout << (i + 1) << ". " << ach->getName() << endl;
            }
        }

        cout << "\n0. Clear Selection\n";
        cout << "\nSELECT: ";

        int choice;
        cin >> choice;
        cin.ignore(1000, '\n');

        if (choice == 0)
        {
            user->setDisplayedAchievementID(-1);
            cout << "Featured achievement cleared.\n";
            return;
        }

        if (choice < 1 || choice > (int)unlocked.size())
        {
            cout << "INVALID CHOICE!\n";
            return;
        }

        int selectedID = unlocked[choice - 1];
        user->setDisplayedAchievementID(selectedID);

        Achievement *ach = getAchievementByID(selectedID);
        if (ach != nullptr)
        {
            cout << "Achievement selected: " << ach->getName() << "\n";
        }
    }

    // ========================================================
    //              ADMIN: SEARCH USERS
    // ========================================================
    // Admin can search by username (partial match, case-insensitive).
    // Only shows public info per privacy rules.
    // ========================================================

    void adminSearchUsers()
    {
        string query = getNonEmptyLine("ENTER USERNAME TO SEARCH: ");
        cout << "\n========== SEARCH RESULTS ==========\n";

        bool found = false;
        for (int i = 0; i < (int)users.size(); i++)
        {
            if (containsIgnoreCase(users[i]->getUsername(), query))
            {
                displayUserSummary(users[i]);
                found = true;
            }
        }

        if (!found) cout << "NO USERS FOUND.\n";
    }

    // ========================================================
    //              ADMIN: DISPLAY USER SUMMARY
    // ========================================================
    // PRIVACY RULE: Only public info shown.
    // No access to tasks, expenses, goals, health, notes.
    // ========================================================

    void displayUserSummary(User *user)
    {
        if (user == nullptr) return;

        cout << "\n------------------------------------\n";
        cout << "USERNAME          : " << user->getUsername() << endl;
        cout << "FULL NAME         : " << user->getName() << endl;
        cout << "EMAIL             : " << (user->getEmail().empty() ? "N/A" : user->getEmail()) << endl;
        cout << "REGISTRATION DATE : " << user->getRegistrationDate() << endl;
        cout << "LAST LOGIN        : "; if (user->getLastLogin().toComparable() == 20000101) cout << "Never"; else cout << user->getLastLogin(); cout << endl;
        cout << "ACCOUNT STATUS    : " << (user->getIsActive() ? "Active" : "Inactive") << endl;
        cout << "ACHIEVEMENTS      : " << user->getUnlockedCount() << " unlocked" << endl;
        cout << "GOALS COMPLETED   : " << user->getGoalManager().countCompletedGoals() << endl;
        cout << "------------------------------------\n";
    }

    // ========================================================
    //              ADMIN: VIEW ACTIVE USERS
    // ========================================================

    void adminViewActiveUsers()
    {
        cout << "\n========== ACTIVE USERS ==========\n";
        bool found = false;
        for (int i = 0; i < (int)users.size(); i++)
        {
            if (users[i]->getIsActive())
            {
                displayUserSummary(users[i]);
                found = true;
            }
        }
        if (!found) cout << "NO ACTIVE USERS.\n";
    }

    // ========================================================
    //              ADMIN: VIEW INACTIVE USERS
    // ========================================================

    void adminViewInactiveUsers()
    {
        cout << "\n========== INACTIVE USERS ==========\n";
        bool found = false;
        for (int i = 0; i < (int)users.size(); i++)
        {
            if (!users[i]->getIsActive())
            {
                displayUserSummary(users[i]);
                found = true;
            }
        }
        if (!found) cout << "NO INACTIVE USERS.\n";
    }

    // ========================================================
    //              ADMIN: ACTIVATE / DEACTIVATE USER
    // ========================================================

    void adminToggleUserStatus()
    {
        string username = getNonEmptyLine("ENTER USERNAME: ");
        User *user = findUserByUsername(username);
        if (user == nullptr)
        {
            cout << "USER NOT FOUND!\n";
            return;
        }

        bool newStatus = !user->getIsActive();
        user->setActive(newStatus);
        cout << "USER " << username << " IS NOW "
             << (newStatus ? "ACTIVE" : "INACTIVE") << ".\n";
    }

    // ========================================================
    //              ADMIN: VIEW SYSTEM STATISTICS
    // ========================================================

    void adminViewStatistics()
    {
        int totalUsers = users.size();
        int activeUsers = 0;
        int totalGoalsCompleted = 0;

        for (int i = 0; i < totalUsers; i++)
        {
            if (users[i]->getIsActive()) activeUsers++;
            totalGoalsCompleted += users[i]->getGoalManager().countCompletedGoals();
        }

        cout << "\n========== SYSTEM STATISTICS ==========\n";
        cout << "TOTAL USERS          : " << totalUsers << endl;
        cout << "ACTIVE USERS         : " << activeUsers << endl;
        cout << "INACTIVE USERS       : " << (totalUsers - activeUsers) << endl;
        cout << "TOTAL ACHIEVEMENTS   : " << achievements.size() << endl;
        cout << "DEFAULT ACHIEVEMENTS : ";
        int defCount = 0;
        for (int i = 0; i < (int)achievements.size(); i++)
            if (achievements[i].getIsDefault()) defCount++;
        cout << defCount << endl;
        cout << "CUSTOM ACHIEVEMENTS  : " << ((int)achievements.size() - defCount) << endl;
        cout << "TOTAL GOALS COMPLETED: " << totalGoalsCompleted << endl;
        cout << "=======================================\n";
    }

    // ========================================================
    //              ADMIN: ADD CUSTOM ACHIEVEMENT
    // ========================================================

    void adminAddAchievement()
    {
        cout << "\n========== ADD CUSTOM ACHIEVEMENT ==========\n";
        string name = getNonEmptyLine("ENTER ACHIEVEMENT NAME: ");
        string desc = getNonEmptyLine("ENTER DESCRIPTION: ");
        int reqGoals;
        cout << "ENTER REQUIRED GOALS TO UNLOCK: ";
        cin >> reqGoals;
        cin.ignore(1000, '\n');

        if (reqGoals < 1)
        {
            cout << "REQUIRED GOALS MUST BE AT LEAST 1!\n";
            return;
        }

        achievements.push_back(Achievement(name, desc, reqGoals, false));
        cout << "CUSTOM ACHIEVEMENT ADDED SUCCESSFULLY!\n";
    }

    // ========================================================
    //              ADMIN: REMOVE CUSTOM ACHIEVEMENT
    // ========================================================
    // Cannot remove default achievements.
    // ========================================================

    void adminRemoveAchievement()
    {
        cout << "\n========== REMOVE ACHIEVEMENT ==========\n";
        cout << "Available achievements:\n";
        for (int i = 0; i < (int)achievements.size(); i++)
        {
            achievements[i].displayBrief();
        }

        int id;
        cout << "\nENTER ACHIEVEMENT ID TO REMOVE: ";
        cin >> id;
        cin.ignore(1000, '\n');

        Achievement *ach = getAchievementByID(id);
        if (ach == nullptr)
        {
            cout << "ACHIEVEMENT NOT FOUND!\n";
            return;
        }

        if (ach->getIsDefault())
        {
            cout << "CANNOT REMOVE DEFAULT ACHIEVEMENTS!\n";
            return;
        }

        if (!confirmAction("ARE YOU SURE YOU WANT TO REMOVE THIS ACHIEVEMENT?"))
        {
            cout << "REMOVAL CANCELLED!\n";
            return;
        }

        // Remove from vector
        for (int i = 0; i < (int)achievements.size(); i++)
        {
            if (achievements[i].getAchievementID() == id)
            {
                achievements.erase(achievements.begin() + i);
                cout << "ACHIEVEMENT REMOVED SUCCESSFULLY!\n";
                return;
            }
        }
    }

    // ========================================================
    //              ADMIN: UPDATE CUSTOM ACHIEVEMENT
    // ========================================================
    // Cannot update default achievements.
    // ========================================================

    void adminUpdateAchievement()
    {
        cout << "\n========== UPDATE ACHIEVEMENT ==========\n";
        cout << "Available achievements:\n";
        for (int i = 0; i < (int)achievements.size(); i++)
        {
            achievements[i].displayBrief();
        }

        int id;
        cout << "\nENTER ACHIEVEMENT ID TO UPDATE: ";
        cin >> id;
        cin.ignore(1000, '\n');

        Achievement *ach = getAchievementByID(id);
        if (ach == nullptr)
        {
            cout << "ACHIEVEMENT NOT FOUND!\n";
            return;
        }

        if (ach->getIsDefault())
        {
            cout << "CANNOT UPDATE DEFAULT ACHIEVEMENTS!\n";
            return;
        }

        int fieldChoice;
        cout << "\n1. Update Name\n";
        cout << "2. Update Description\n";
        cout << "3. Update Required Goals\n";
        cout << "4. Cancel\n";
        cout << "ENTER CHOICE: ";
        cin >> fieldChoice;
        cin.ignore(1000, '\n');

        switch (fieldChoice)
        {
        case 1:
            ach->setName(getNonEmptyLine("ENTER NEW NAME: "));
            cout << "NAME UPDATED!\n";
            break;
        case 2:
            ach->setDescription(getNonEmptyLine("ENTER NEW DESCRIPTION: "));
            cout << "DESCRIPTION UPDATED!\n";
            break;
        case 3:
        {
            int req;
            cout << "ENTER NEW REQUIRED GOALS: ";
            cin >> req;
            cin.ignore(1000, '\n');
            if (req < 1)
            {
                cout << "MUST BE AT LEAST 1!\n";
                return;
            }
            ach->setRequiredGoals(req);
            cout << "REQUIREMENT UPDATED!\n";
            break;
        }
        case 4:
            cout << "UPDATE CANCELLED!\n";
            return;
        default:
            cout << "INVALID CHOICE!\n";
            return;
        }
    }

    // ========================================================
    //              ADMIN: VIEW ALL ACHIEVEMENTS
    // ========================================================

    void adminViewAchievements()
    {
        cout << "\n========== ALL ACHIEVEMENTS ==========\n";
        for (int i = 0; i < (int)achievements.size(); i++)
        {
            achievements[i].displayAchievement();
        }
        if (achievements.empty())
            cout << "NO ACHIEVEMENTS DEFINED.\n";
    }

    // ========================================================
    //              ADMIN MENU
    // ========================================================

    void adminMenu()
    {
        int choice;
        do
        {
            cout << "\n================ ADMIN MENU ================\n";
            cout << "1. Search Users\n";
            cout << "2. View Active Users\n";
            cout << "3. View Inactive Users\n";
            cout << "4. Activate/Deactivate User\n";
            cout << "5. View System Statistics\n";
            cout << "6. View All Achievements\n";
            cout << "7. Add Custom Achievement\n";
            cout << "8. Remove Custom Achievement\n";
            cout << "9. Update Custom Achievement\n";
            cout << "10. Change Admin Password\n";
            cout << "11. Logout\n";
            cout << "ENTER CHOICE: ";
            cin >> choice;
            cin.ignore(1000, '\n');

            switch (choice)
            {
            case 1: adminSearchUsers(); break;
            case 2: adminViewActiveUsers(); break;
            case 3: adminViewInactiveUsers(); break;
            case 4: adminToggleUserStatus(); break;
            case 5: adminViewStatistics(); break;
            case 6: adminViewAchievements(); break;
            case 7: adminAddAchievement(); break;
            case 8: adminRemoveAchievement(); break;
            case 9: adminUpdateAchievement(); break;
            case 10: admin.changePassword(); break;
            case 11: cout << "LOGGING OUT...\n"; break;
            default: cout << "INVALID CHOICE!\n";
            }
        } while (choice != 11);
    }

    // ========================================================
    //              DASHBOARD (for logged-in user)
    // ========================================================

    void showDashboard(User &user)
    {
        TaskManager &tm = user.getTaskManager();
        GoalManager &gm = user.getGoalManager();
        ExpenseManager &em = user.getExpenseManager();

        cout << "\n================ YOUR DASHBOARD ================\n";
        cout << "WELCOME BACK, " << user.getDisplayName() << "\n";

        // Tasks
        int overdueTasks = tm.countOverdueTasks();
        int dueSoonTasks = tm.countDueSoonTasks(3);
        int pendingTasks = tm.countPendingTasks();
        int completedTasks = tm.countCompletedTasks();

        cout << "\nTASKS\n";
        cout << "  - " << overdueTasks << " OVERDUE, "
             << dueSoonTasks << " DUE WITHIN THE NEXT 3 DAYS\n";
        cout << "  - " << pendingTasks << " PENDING TOTAL, "
             << completedTasks << " COMPLETED TOTAL\n";

        // Goals
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

        // Expenses
        double totalSpent = em.calculateTotalExpense();
        cout << "\nEXPENSES\n";
        cout << "  - TOTAL SPENT: " << totalSpent << "\n";

        pair<string, double> topCategory = em.getTopCategoryInfo();
        if (!topCategory.first.empty())
        {
            cout << "  - HIGHEST CATEGORY: " << topCategory.first
                 << " (" << topCategory.second << ")\n";
        }

        // Recently earned title
        if (!user.getTitle().empty())
        {
            cout << "\nRECENTLY EARNED TITLE: " << user.getTitle() << "\n";
        }

        cout << "==================================================\n";

        if (pendingTasks > 0)
        {
            cout << "\nFOLLOWING ARE YOUR PENDING TASKS:\n";
            tm.displayPendingTasks();
        }
    }

    // ========================================================
    //              USER MENU
    // ========================================================

    void userMenu(User &user)
    {
        int choice;
        do
        {
            cout << "\n================ USER MENU (" << user.getDisplayName() << ") ================\n";
            cout << "1. View Profile\n";
            cout << "2. Update Profile\n";
            cout << "3. Task Manager\n";
            cout << "4. Goal Manager\n";
            cout << "5. Expense Manager\n";
            cout << "6. View Achievements\n";
            cout << "7. Choose Display Achievement\n";
            cout << "8. Change Password\n";
            cout << "9. Logout\n";
            cout << "ENTER CHOICE: ";
            cin >> choice;
            cin.ignore(1000, '\n');

            switch (choice)
            {
            case 1: user.displayProfile(); break;
            case 2: user.updateProfile(); break;
            case 3: taskMenu(user); break;
            case 4: goalMenu(user); break;
            case 5: expenseMenu(user); break;
            case 6: viewUserAchievements(&user); break;
            case 7: chooseDisplayAchievement(&user); break;
            case 8: user.changePassword(); break;
            case 9: cout << "LOGGING OUT...\n"; break;
            default: cout << "INVALID CHOICE!\n";
            }
        } while (choice != 9);
    }

    // ========================================================
    //              TASK MANAGER MENU (for user)
    // ========================================================

    void taskMenu(User &user)
    {
        int choice;
        do
        {
            cout << "\n================ TASK MANAGER ================\n";
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
            case 1: user.getTaskManager().addTask(); break;
            case 2: user.getTaskManager().displayAllTasks(); break;
            case 3: user.getTaskManager().displayCompletedTasks(); break;
            case 4: user.getTaskManager().displayPendingTasks(); break;
            case 5: taskSearchMenu(user); break;
            case 6: user.getTaskManager().completeTaskFlow(); break;
            case 7: user.getTaskManager().updateTaskFlow(); break;
            case 8: user.getTaskManager().deleteTaskFlow(); break;
            case 9: cout << "RETURNING TO USER MENU...\n"; break;
            default: cout << "INVALID CHOICE!\n";
            }
        } while (choice != 9);
    }

    void taskSearchMenu(User &user)
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
            Task *task = user.getTaskManager().searchByID(id);
            if (task) task->displayTask();
            else cout << "TASK NOT FOUND!\n";
            break;
        }
        case 2:
        {
            string title;
            cout << "ENTER TITLE: ";
            getline(cin, title);
            vector<Task *> result = user.getTaskManager().searchByTitle(title);
            user.getTaskManager().displaySearchResults(result);
            break;
        }
        case 3:
        {
            string category;
            cout << "ENTER CATEGORY (Academic/Daily): ";
            getline(cin, category);
            vector<Task *> result = user.getTaskManager().searchByCategory(category);
            user.getTaskManager().displaySearchResults(result);
            break;
        }
        case 4:
        {
            Date date;
            cout << "ENTER DEADLINE:\n";
            date.inputDate();
            vector<Task *> result = user.getTaskManager().searchByDeadline(date);
            user.getTaskManager().displaySearchResults(result);
            break;
        }
        case 5:
        {
            string status;
            cout << "ENTER STATUS (Pending/Completed): ";
            getline(cin, status);
            vector<Task *> result = user.getTaskManager().searchByStatus(status);
            user.getTaskManager().displaySearchResults(result);
            break;
        }
        default: cout << "INVALID SEARCH OPTION!\n";
        }
    }

    // ========================================================
    //              GOAL MANAGER MENU (for user)
    // ========================================================
    // MODIFIED: After completing a goal, System checks achievements.
    // ========================================================

    void goalMenu(User &user)
    {
        int choice;
        do
        {
            cout << "\n================ GOAL MANAGER ================\n";
            cout << "1. Add Goal\n";
            cout << "2. Display All Goals\n";
            cout << "3. Display Completed Goals\n";
            cout << "4. Display Pending/Overdue Goals\n";
            cout << "5. Search Goal\n";
            cout << "6. Mark Goal As Completed\n";
            cout << "7. View Titles (Old System)\n";
            cout << "8. Update Goal\n";
            cout << "9. Delete Goal\n";
            cout << "10. Back\n";
            cout << "ENTER CHOICE: ";
            cin >> choice;
            cin.ignore(1000, '\n');

            switch (choice)
            {
            case 1: user.getGoalManager().addGoal(); break;
            case 2: user.getGoalManager().displayAllGoals(); break;
            case 3: user.getGoalManager().displayCompletedGoals(); break;
            case 4: user.getGoalManager().displayIncompleteGoals(); break;
            case 5: goalSearchMenu(user); break;
            case 6:
            {
                // Complete goal and check achievements
                bool completed = user.getGoalManager().completeGoalFlow();
                if (completed)
                {
                    user.checkAndUpdateTitle();    // Old title system
                    checkAchievements(&user);       // New achievement system
                }
                break;
            }
            case 7: user.getGoalManager().viewAchievements(); break;
            case 8: user.getGoalManager().updateGoalFlow(); break;
            case 9: user.getGoalManager().deleteGoalFlow(); break;
            case 10: cout << "RETURNING TO USER MENU...\n"; break;
            default: cout << "INVALID CHOICE!\n";
            }
        } while (choice != 10);
    }

    void goalSearchMenu(User &user)
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
            Goal *goal = user.getGoalManager().searchByID(id);
            if (goal) goal->displayGoal();
            else cout << "GOAL NOT FOUND!\n";
            break;
        }
        case 2:
        {
            string title;
            cout << "ENTER TITLE: ";
            getline(cin, title);
            vector<Goal *> result = user.getGoalManager().searchByTitle(title);
            user.getGoalManager().displaySearchResults(result);
            break;
        }
        case 3:
        {
            string category;
            cout << "ENTER CATEGORY: ";
            getline(cin, category);
            vector<Goal *> result = user.getGoalManager().searchByCategory(category);
            user.getGoalManager().displaySearchResults(result);
            break;
        }
        case 4:
        {
            Date date;
            cout << "ENTER DEADLINE:\n";
            date.inputDate();
            vector<Goal *> result = user.getGoalManager().searchByDeadline(date);
            user.getGoalManager().displaySearchResults(result);
            break;
        }
        case 5:
        {
            string status;
            cout << "ENTER STATUS (Incomplete/Completed): ";
            getline(cin, status);
            vector<Goal *> result = user.getGoalManager().searchByStatus(status);
            user.getGoalManager().displaySearchResults(result);
            break;
        }
        default: cout << "INVALID SEARCH OPTION!\n";
        }
    }

    // ========================================================
    //              EXPENSE MANAGER MENU (for user)
    // ========================================================

    void expenseMenu(User &user)
    {
        int choice;
        do
        {
            cout << "\n================ EXPENSE MANAGER ================\n";
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
            case 1: user.getExpenseManager().addExpense(); break;
            case 2: user.getExpenseManager().displayAllExpenses(); break;
            case 3: expenseSearchMenu(user); break;
            case 4: user.getExpenseManager().updateExpenseFlow(); break;
            case 5: user.getExpenseManager().deleteExpenseFlow(); break;
            case 6: cout << "TOTAL EXPENSE: " << user.getExpenseManager().calculateTotalExpense() << endl; break;
            case 7: cout << "RETURNING TO USER MENU...\n"; break;
            default: cout << "INVALID CHOICE!\n";
            }
        } while (choice != 7);
    }

    void expenseSearchMenu(User &user)
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
            Expense *expense = user.getExpenseManager().searchByID(id);
            if (expense) expense->displayExpense();
            else cout << "EXPENSE NOT FOUND!\n";
            break;
        }
        case 2:
        {
            string title;
            cout << "ENTER TITLE: ";
            getline(cin, title);
            vector<Expense *> result = user.getExpenseManager().searchByTitle(title);
            user.getExpenseManager().displaySearchResults(result);
            break;
        }
        case 3:
        {
            string category;
            cout << "ENTER CATEGORY: ";
            getline(cin, category);
            vector<Expense *> result = user.getExpenseManager().searchByCategory(category);
            user.getExpenseManager().displaySearchResults(result);
            break;
        }
        case 4:
        {
            Date date;
            cout << "ENTER DATE:\n";
            date.inputDate();
            vector<Expense *> result = user.getExpenseManager().searchByDate(date);
            user.getExpenseManager().displaySearchResults(result);
            break;
        }
        default: cout << "INVALID SEARCH OPTION!\n";
        }
    }

    // ========================================================
    //              MAIN MENU
    // ========================================================
    // No visible Admin option. Admin login is hidden inside
    // the normal login flow.
    // ========================================================

    void mainMenu()
    {
        int choice;
        do
        {
            cout << "\n================ LIFE MANAGER ================\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. Exit\n";
            cout << "ENTER CHOICE: ";
            cin >> choice;
            cin.ignore(1000, '\n');

            switch (choice)
            {
            case 1: loginFlow(); break;
            case 2: registerUser(); break;
            case 3: cout << "EXITING LIFE MANAGER...\n"; break;
            default: cout << "INVALID CHOICE!\n";
            }
        } while (choice != 3);
    }
};

// ============================================================
//                           MAIN
// ============================================================
// System is the central controller. No global vectors needed.
// ============================================================

int main()
{
    System system;
    system.mainMenu();
    return 0;
}