#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================
//                          DATE CLASS
// ============================================================
// common class used everywhere a date is needed - DOB, deadlines,
// expense date, goal target date, achievement date etc.
class Date
{
private:
    int day;
    int month;
    int year;

public:
    // DEFAULT CONSTRUCTOR - dummy date, gets overwritten by inputDate()
    Date() : day(1), month(1), year(2000) {}

    // PARAMETERIZED CONSTRUCTOR
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    // ---- GETTERS ----
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    // ---- SETTERS ----
    void setDay(int d) { day = d; }
    void setMonth(int m) { month = m; }
    void setYear(int y) { year = y; }

    // ---- INPUT ----
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

    // ---- DISPLAY ----
    void displayDate() const
    {
        cout << (day < 10 ? "0" : "") << day << "/"
             << (month < 10 ? "0" : "") << month << "/"
             << year;
    }

    // friend ostream so Date can be printed directly, e.g. cout << someDate
    friend ostream &operator<<(ostream &os, const Date &d)
    {
        os << (d.day < 10 ? "0" : "") << d.day << "/"
           << (d.month < 10 ? "0" : "") << d.month << "/"
           << d.year;
        return os;
    }

    // ---- COMPARE ----
    // true if THIS date comes strictly before "other" (useful later for
    // deadlines / sorting tasks by due date)
    bool compareDate(Date other) const
    {
        if (year != other.year)
            return year < other.year;
        if (month != other.month)
            return month < other.month;
        return day < other.day;
    }

    // true if both dates are the same day
    bool isEqual(Date other) const
    {
        return day == other.day && month == other.month && year == other.year;
    }
};

// ============================================================
//                          TASK CLASS (ABSTRACT)
// ============================================================
// base class for AcademicTask and DailyTask - can't create a plain
// Task object directly, only through its children
class Task
{
protected:
    static int next_id; // auto-incrementing id, shared across both subtypes
    int taskID;
    string title;
    Date deadline;
    bool completed;
    string category; // "ACADEMIC" or "DAILY" - auto assigned by the child's constructor

public:
    Task(string t, Date d, string cat)
        : taskID(next_id++), title(t), deadline(d), completed(false), category(cat) {}

    virtual ~Task() {}

    // pure virtual - every task type prints itself differently
    virtual void display() const = 0;

    // ---- GETTERS ----
    int getTaskID() const { return taskID; }
    string getTitle() const { return title; }
    Date getDeadline() const { return deadline; }
    bool isCompleted() const { return completed; }
    string getCategory() const { return category; }

    // ---- UPDATE / COMPLETE ----
    void setTitle(string t) { title = t; }
    void setDeadline(Date d) { deadline = d; }
    void markComplete() { completed = true; }
};

int Task::next_id = 1;

// ============================================================
//                      ACADEMIC TASK CLASS
// ============================================================
class AcademicTask : public Task
{
public:
    AcademicTask(string t, Date d) : Task(t, d, "ACADEMIC") {}

    void display() const override
    {
        cout << "---------------------------------------------" << endl
             << "| ID       : " << taskID << endl
             << "| CATEGORY : " << category << endl
             << "| TITLE    : " << title << endl
             << "| DEADLINE : " << deadline << endl
             << "| STATUS   : " << (completed ? "COMPLETED" : "PENDING") << endl
             << "---------------------------------------------" << endl;
    }
};

// ============================================================
//                        DAILY TASK CLASS
// ============================================================
class DailyTask : public Task
{
public:
    DailyTask(string t, Date d) : Task(t, d, "DAILY") {}

    void display() const override
    {
        cout << "---------------------------------------------" << endl
             << "| ID       : " << taskID << endl
             << "| CATEGORY : " << category << endl
             << "| TITLE    : " << title << endl
             << "| DEADLINE : " << deadline << endl
             << "| STATUS   : " << (completed ? "COMPLETED" : "PENDING") << endl
             << "---------------------------------------------" << endl;
    }
};

// ============================================================
//                      TASK MANAGER CLASS
// ============================================================
// manages all tasks of ONE user - stored as Task* so both
// AcademicTask and DailyTask can live in the same vector (polymorphism)
class TaskManager
{
private:
    vector<Task *> tasks;

    // preset title menus - last option is always "OTHER" (handled separately)
    vector<string> academicTitles = {
        "Exam", "Assignment", "Project", "Midterm",
        "Test", "Submission", "Viva", "Presentation"};

    vector<string> dailyTitles = {
        "Laundry", "Cleaning House", "Washing Dishes", "Cooking",
        "Grocery Shopping", "Workout", "Watering Plants", "Ironing Clothes"};

    // finds a task by ID, returns nullptr if not found (used internally)
    Task *findByID(int id)
    {
        for (int i = 0; i < (int)tasks.size(); i++)
            if (tasks[i]->getTaskID() == id)
                return tasks[i];
        return nullptr;
    }

public:
    // cleans up all dynamically allocated tasks
    ~TaskManager()
    {
        for (int i = 0; i < (int)tasks.size(); i++)
            delete tasks[i];
        tasks.clear();
    }

    // ---- ADD ----
    void addTask()
    {
        cout << "\n---- ADD TASK ----" << endl;
        cout << "1. ACADEMIC TASK" << endl;
        cout << "2. DAILY TASK" << endl;
        cout << "ENTER CHOICE: ";
        int type;
        cin >> type;
        cin.ignore(1000, '\n');

        vector<string> &options = (type == 1) ? academicTitles : dailyTitles;

        cout << "\n---- CHOOSE TASK ----" << endl;
        for (int i = 0; i < (int)options.size(); i++)
            cout << i + 1 << ". " << options[i] << endl;
        cout << options.size() + 1 << ". OTHER" << endl;
        cout << "ENTER CHOICE: ";
        int choice;
        cin >> choice;
        cin.ignore(1000, '\n');

        string title;
        if (choice >= 1 && choice <= (int)options.size())
            title = options[choice - 1];
        else
        {
            cout << "ENTER TASK TITLE: ";
            getline(cin, title);
        }

        cout << "ENTER DEADLINE:" << endl;
        Date d;
        d.inputDate();

        Task *t;
        if (type == 1)
            t = new AcademicTask(title, d);
        else
            t = new DailyTask(title, d);

        tasks.push_back(t);
        cout << "TASK ADDED! TASK ID: " << t->getTaskID() << endl;
    }

    // ---- DELETE ----
    void deleteTask(int id)
    {
        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->getTaskID() == id)
            {
                delete tasks[i];
                tasks.erase(tasks.begin() + i);
                cout << "TASK DELETED!" << endl;
                return;
            }
        }
        cout << "TASK NOT FOUND!" << endl;
    }

    // ---- UPDATE ----
    void updateTask(int id)
    {
        Task *t = findByID(id);
        if (!t)
        {
            cout << "TASK NOT FOUND!" << endl;
            return;
        }

        cout << "1. UPDATE TITLE" << endl;
        cout << "2. UPDATE DEADLINE" << endl;
        cout << "ENTER CHOICE: ";
        int choice;
        cin >> choice;
        cin.ignore(1000, '\n');

        if (choice == 1)
        {
            cout << "ENTER NEW TITLE: ";
            string newTitle;
            getline(cin, newTitle);
            t->setTitle(newTitle);
            cout << "TITLE UPDATED!" << endl;
        }
        else if (choice == 2)
        {
            cout << "ENTER NEW DEADLINE:" << endl;
            Date d;
            d.inputDate();
            t->setDeadline(d);
            cout << "DEADLINE UPDATED!" << endl;
        }
    }

    // ---- MARK COMPLETE ----
    // task stays in the same list, just flips its status - it then shows
    // up under "completed" instead of "incomplete"
    void markTaskComplete(int id)
    {
        Task *t = findByID(id);
        if (!t)
        {
            cout << "TASK NOT FOUND!" << endl;
            return;
        }
        t->markComplete();
        cout << "TASK MARKED AS COMPLETE!" << endl;
    }

    // ---- SEARCH ----
    void searchTask(string title)
    {
        bool found = false;
        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->getTitle() == title)
            {
                tasks[i]->display();
                found = true;
            }
        }
        if (!found)
            cout << "NO TASK FOUND WITH THAT TITLE!" << endl;
    }

    // ---- DISPLAY ----
    void displayAllTasks()
    {
        if (tasks.empty())
        {
            cout << "NO TASKS ADDED YET!" << endl;
            return;
        }
        cout << "\n==== ALL TASKS ====" << endl;
        for (int i = 0; i < (int)tasks.size(); i++)
            tasks[i]->display(); // polymorphism - calls the right display()
    }

    void displayCompletedTasks()
    {
        bool found = false;
        cout << "\n==== COMPLETED TASKS ====" << endl;
        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->isCompleted())
            {
                tasks[i]->display();
                found = true;
            }
        }
        if (!found)
            cout << "NO COMPLETED TASKS YET!" << endl;
    }

    void displayIncompleteTasks()
    {
        bool found = false;
        cout << "\n==== INCOMPLETE TASKS ====" << endl;
        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (!tasks[i]->isCompleted())
            {
                tasks[i]->display();
                found = true;
            }
        }
        if (!found)
            cout << "NO INCOMPLETE TASKS!" << endl;
    }
};

// forward declaration since Authentication needs to touch User's password
class Authentication;

// ============================================================
//                          USER CLASS
// ============================================================
class User
{
private:
    string name;
    int UID;
    static int next_id; // auto-incrementing id
    Date DOB;
    string password;

    TaskManager taskManager; // composition - every user owns their own tasks

    // lets Authentication reset password without exposing a public setter
    // to everyone else
    friend class Authentication;

public:
    // DEFAULT CONSTRUCTOR
    User() : name("NONE"), UID(next_id++), password("") {}

    // PARAMETERIZED CONSTRUCTOR
    User(string n, Date dob, string pass)
        : name(n), UID(next_id++), DOB(dob), password(pass) {}

    // ---- GETTERS ----
    string getName() const { return name; }
    int getUID() const { return UID; }
    Date getDOB() const { return DOB; }

    // ---- STATIC ID MANAGEMENT ----
    static void setNextID(int id) { next_id = id; }
    static int getNextID() { return next_id; }

    // ---- DISPLAY ----
    void displayProfile() const
    {
        cout << "================= USER PROFILE =================" << endl
             << "| UID       : " << UID << endl
             << "| NAME      : " << name << endl
             << "| DOB       : " << DOB << endl
             << "==================================================" << endl;
    }

    // ---- UPDATE ----
    // lets the user edit name / DOB, one field at a time
    void updateProfile()
    {
        cout << "\nWHAT DO YOU WANT TO UPDATE?\n1. NAME\n2. DATE OF BIRTH\n3. CANCEL\nCHOICE: ";
        int choice;
        cin >> choice;
        cin.ignore(1000, '\n');

        if (choice == 1)
        {
            cout << "ENTER NEW NAME: ";
            getline(cin, name);
            cout << "NAME UPDATED!" << endl;
        }
        else if (choice == 2)
        {
            cout << "ENTER NEW DATE OF BIRTH:" << endl;
            DOB.inputDate();
            cout << "DATE OF BIRTH UPDATED!" << endl;
        }
        else
        {
            cout << "UPDATE CANCELLED." << endl;
        }
    }

    // ---- LOGIN CHECK ----
    // just compares the given password against the stored one
    bool login(string pass) const
    {
        return password == pass;
    }

    // ---- TASK MENU ----
    // only reachable through the User object - this is what keeps a
    // user's tasks tied to them and not accessible from outside
    void taskMenu()
    {
        int choice = 0;
        do
        {
            cout << "\n================ TASK MENU ================" << endl;
            cout << "1. ADD TASK" << endl;
            cout << "2. VIEW ALL TASKS" << endl;
            cout << "3. VIEW COMPLETED TASKS" << endl;
            cout << "4. VIEW INCOMPLETE TASKS" << endl;
            cout << "5. MARK TASK AS COMPLETE" << endl;
            cout << "6. UPDATE TASK" << endl;
            cout << "7. DELETE TASK" << endl;
            cout << "8. SEARCH TASK BY TITLE" << endl;
            cout << "9. BACK" << endl;
            cout << "ENTER CHOICE: ";
            cin >> choice;
            cin.ignore(1000, '\n');

            switch (choice)
            {
            case 1:
                taskManager.addTask();
                break;
            case 2:
                taskManager.displayAllTasks();
                break;
            case 3:
                taskManager.displayCompletedTasks();
                break;
            case 4:
                taskManager.displayIncompleteTasks();
                break;
            case 5:
            {
                cout << "ENTER TASK ID: ";
                int id;
                cin >> id;
                cin.ignore(1000, '\n');
                taskManager.markTaskComplete(id);
                break;
            }
            case 6:
            {
                cout << "ENTER TASK ID: ";
                int id;
                cin >> id;
                cin.ignore(1000, '\n');
                taskManager.updateTask(id);
                break;
            }
            case 7:
            {
                cout << "ENTER TASK ID: ";
                int id;
                cin >> id;
                cin.ignore(1000, '\n');
                taskManager.deleteTask(id);
                break;
            }
            case 8:
            {
                cout << "ENTER TITLE TO SEARCH: ";
                string title;
                getline(cin, title);
                taskManager.searchTask(title);
                break;
            }
            case 9:
                cout << "RETURNING..." << endl;
                break;
            }

        } while (choice != 9);
    }
};

// STATIC MEMBER DEFINITION
int User::next_id = 1;

// ============================================================
//                    AUTHENTICATION CLASS
// ============================================================
// Authentication is associated with User but doesn't own it -
// it just operates on a User reference it's given
class Authentication
{
public:
    // ---- LOGIN ----
    bool authenticate(User &user, string password)
    {
        if (user.login(password))
        {
            cout << "LOGIN SUCCESSFUL! WELCOME " << user.getName() << "." << endl;
            return true;
        }
        cout << "INVALID PASSWORD!" << endl;
        return false;
    }

    // ---- CHANGE PASSWORD ----
    // confirms old password first before allowing a new one
    void changePassword(User &user)
    {
        cout << "ENTER CURRENT PASSWORD: ";
        string oldPass;
        getline(cin, oldPass);

        if (!user.login(oldPass))
        {
            cout << "CURRENT PASSWORD IS INCORRECT! PASSWORD NOT CHANGED." << endl;
            return;
        }

        cout << "ENTER NEW PASSWORD: ";
        string newPass;
        getline(cin, newPass);

        user.password = newPass;
        cout << "PASSWORD CHANGED SUCCESSFULLY!" << endl;
    }
};

// ============================================================
//              TEMPORARY TEST MAIN (will be replaced
//              by the System class once it's built)
// ============================================================
int main()
{
    cout << "===== CREATE YOUR ACCOUNT =====" << endl;
    cout << "ENTER NAME: ";
    string name;
    getline(cin, name);

    cout << "ENTER DATE OF BIRTH:" << endl;
    Date dob;
    dob.inputDate();

    cout << "SET PASSWORD: ";
    string password;
    getline(cin, password);

    User u(name, dob, password);
    Authentication auth;

    int choice = 0;
    do
    {
        cout << "\n================ TEST MENU ================" << endl;
        cout << "1. LOGIN" << endl;
        cout << "2. VIEW PROFILE" << endl;
        cout << "3. UPDATE PROFILE" << endl;
        cout << "4. CHANGE PASSWORD" << endl;
        cout << "5. TASK MENU" << endl;
        cout << "6. EXIT" << endl;
        cout << "ENTER CHOICE: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice)
        {
        case 1:
        {
            cout << "ENTER PASSWORD: ";
            string pass;
            getline(cin, pass);
            auth.authenticate(u, pass);
            break;
        }
        case 2:
            u.displayProfile();
            break;
        case 3:
            u.updateProfile();
            break;
        case 4:
            auth.changePassword(u);
            break;
        case 5:
            u.taskMenu();
            break;
        case 6:
            cout << "EXITING..." << endl;
            break;
        }

    } while (choice != 6);

    return 0;
}