// ============================================================
//              REVOLIF - LIFE MANAGER (VERSION 2.1)
// ============================================================
// This version includes everything from before, PLUS:
//   - Task priority levels (High/Medium/Low) with smart sorting
//   - Recurring tasks (Daily/Weekly/Monthly auto-reset)
//   - Login streak tracking (current + best streak)
//   - Life Score: a single composite score of overall progress
//   - Category budgets with overspending alerts
//   - ASCII bar chart for spending by category
//   - Hashed passwords (no more plaintext storage)
//   - Unique email enforcement across all users
//   - Exportable monthly report (.txt file)
// ============================================================

// Default admin login: admin / admin123
// (Change the admin password immediately after first login.)

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <utility>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <map>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

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

// ---- SIMPLE PASSWORD HASH (djb2 algorithm) ----
// NOTE: This is a lightweight hash for demonstration purposes so that
// passwords are never stored or compared as plain text in memory/files.
string simpleHash(const string &input)
{
    unsigned long hash = 5381;
    for (int i = 0; i < (int)input.size(); i++)
        hash = ((hash << 5) + hash) + (unsigned char)input[i];

    stringstream ss;
    ss << hex << hash;
    return ss.str();
}

// ---- PRIORITY WEIGHT (for sorting tasks) ----
int priorityWeight(string p)
{
    if (p == "High") return 1;
    if (p == "Medium") return 2;
    return 3; // Low
}

// ============================================================
//              INPUT VALIDATION FUNCTIONS
// ============================================================

// ---- NON EMPTY STRING ----
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

// ---- INTEGER INPUT ----
int getValidInt(string prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        cin >> value;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "INVALID INPUT! PLEASE ENTER A NUMBER." << endl;
        }
        else
        {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

// ---- INTEGER IN RANGE ----
int getValidIntInRange(string prompt, int min, int max)
{
    int value;
    while (true)
    {
        value = getValidInt(prompt);

        if (value < min || value > max)
            cout << "PLEASE ENTER A NUMBER BETWEEN " << min << " AND " << max << "." << endl;
        else
            return value;
    }
}

// ---- NON-NEGATIVE DOUBLE ----
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

// ---- LETTERS ONLY (for names) ----
string getValidAlphaString(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "INPUT CANNOT BE EMPTY!" << endl;
            continue;
        }

        bool valid = true;
        for (int i = 0; i < (int)value.size(); i++)
        {
            if (!isalpha(value[i]) && value[i] != ' ')
            {
                valid = false;
                break;
            }
        }

        if (!valid)
            cout << "ONLY LETTERS AND SPACES ALLOWED!" << endl;
        else
            return value;
    }
}

// ---- USERNAME (alphanumeric, underscores, 3-20 chars) ----
string getValidUsername(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "USERNAME CANNOT BE EMPTY!" << endl;
            continue;
        }

        if (value.size() < 3 || value.size() > 20)
        {
            cout << "USERNAME MUST BE BETWEEN 3 AND 20 CHARACTERS!" << endl;
            continue;
        }

        bool valid = true;
        for (int i = 0; i < (int)value.size(); i++)
        {
            if (!isalnum(value[i]) && value[i] != '_')
            {
                valid = false;
                break;
            }
        }

        if (!valid)
            cout << "USERNAME CAN ONLY CONTAIN LETTERS, NUMBERS, AND UNDERSCORES!" << endl;
        else
            return value;
    }
}

// ---- EMAIL ----
string getValidEmail(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "EMAIL CANNOT BE EMPTY!" << endl;
            continue;
        }

        bool hasAt = false;
        bool hasDot = false;
        int atPos = -1;

        for (int i = 0; i < (int)value.size(); i++)
        {
            if (value[i] == '@')
            {
                hasAt = true;
                atPos = i;
            }
            if (value[i] == '.' && hasAt && i > atPos + 1)
            {
                hasDot = true;
            }
        }

        bool valid = hasAt && hasDot;
        if (valid)
        {
            if (value.find(' ') != string::npos) valid = false;
            if (atPos == 0 || atPos == (int)value.size() - 1) valid = false;
            if (value[value.size()-1] == '.') valid = false;
        }

        if (!valid)
            cout << "INVALID EMAIL! MUST BE A VALID EMAIL FORMAT (e.g., user@example.com)" << endl;
        else
            return value;
    }
}

// ---- MASKED INPUT (echoes '*' instead of the typed characters) ----
string getMaskedInput(string prompt)
{
    cout << prompt;
    string value;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r' && ch != '\n')
    {
        if (ch == '\b' || ch == 127) // backspace
        {
            if (!value.empty())
            {
                value.pop_back();
                cout << "\b \b";
            }
        }
        else
        {
            value.push_back(ch);
            cout << '*';
        }
    }
#else
    termios oldSettings, newSettings;
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

    // NOTE: we read through cin (not a raw read() on the fd) so that this
    // stays in sync with cin's internal buffer, which is also used by all
    // the other getline()/cin>> calls throughout the program.
    char ch;
    while (cin.get(ch) && ch != '\n')
    {
        if (ch == 127 || ch == '\b') // backspace
        {
            if (!value.empty())
            {
                value.pop_back();
                cout << "\b \b" << flush;
            }
        }
        else
        {
            value.push_back(ch);
            cout << '*' << flush;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
#endif

    cout << endl;
    return value;
}

// ---- MASKED, NON-EMPTY INPUT (used for existing-password prompts) ----
string getMaskedNonEmptyLine(string prompt)
{
    string value;
    while (true)
    {
        value = getMaskedInput(prompt);
        if (value.empty())
        {
            cout << "THIS FIELD CANNOT BE EMPTY! PLEASE TRY AGAIN.\n";
            continue;
        }
        return value;
    }
}

// ---- PASSWORD (min 6 characters, masked input) ----
string getValidPassword(string prompt)
{
    string value;
    while (true)
    {
        value = getMaskedInput(prompt);

        if (value.empty())
        {
            cout << "PASSWORD CANNOT BE EMPTY!" << endl;
            continue;
        }

        if (value.size() < 6)
            cout << "PASSWORD TOO SHORT! MINIMUM 6 CHARACTERS." << endl;
        else
            return value;
    }
}

// ---- CONFIRM PASSWORD ----
string getConfirmedPassword(string prompt)
{
    string password, confirmPassword;
    while (true)
    {
        password = getValidPassword(prompt);
        confirmPassword = getValidPassword("CONFIRM PASSWORD: ");
        if (password != confirmPassword)
        {
            cout << "PASSWORDS DO NOT MATCH! PLEASE TRY AGAIN.\n";
            continue;
        }
        return password;
    }
}

// ---- CONFIRM ACTION ----
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

// ---- DATE (DD/MM/YYYY format) ----
string getValidDateString(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true;

        if (value.size() != 10)
            valid = false;
        else if (value[2] != '/' || value[5] != '/')
            valid = false;
        else
        {
            for (int i = 0; i < 10; i++)
            {
                if (i == 2 || i == 5)
                    continue;
                if (!isdigit(value[i]))
                {
                    valid = false;
                    break;
                }
            }

            if (valid)
            {
                int day = stoi(value.substr(0, 2));
                int month = stoi(value.substr(3, 2));
                int year = stoi(value.substr(6, 4));

                if (day < 1 || day > 31)
                    valid = false;
                if (month < 1 || month > 12)
                    valid = false;
                if (year < 1900 || year > 2100)
                    valid = false;

                int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
                if (isLeap) daysInMonth[1] = 29;
                if (month >= 1 && month <= 12 && day > daysInMonth[month - 1])
                    valid = false;
            }
        }

        if (!valid)
            cout << "INVALID DATE! FORMAT MUST BE: DD/MM/YYYY (e.g., 15/07/2026)" << endl;
        else
            return value;
    }
}

// ---- TIME (HH:MM format, 12-hour with AM/PM) ----
string getValidTimeString(string prompt)
{
    string value;
    string period;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true;

        if (value.size() != 5)
            valid = false;
        else if (value[2] != ':')
            valid = false;
        else
        {
            for (int i = 0; i < 5; i++)
            {
                if (i == 2)
                    continue;
                if (!isdigit(value[i]))
                {
                    valid = false;
                    break;
                }
            }

            int hours = stoi(value.substr(0, 2));
            int minutes = stoi(value.substr(3, 2));

            if (hours < 1 || hours > 12)
                valid = false;
            if (minutes < 0 || minutes > 59)
                valid = false;
        }

        if (!valid)
        {
            cout << "INVALID TIME! FORMAT MUST BE: HH:MM (1-12)" << endl;
            continue;
        }

        cout << "ENTER AM OR PM: ";
        getline(cin, period);
        if (period != "AM" && period != "PM" && period != "am" && period != "pm")
        {
            cout << "INVALID! ENTER AM OR PM" << endl;
            continue;
        }

        if (period == "am")
            period = "AM";
        if (period == "pm")
            period = "PM";

        int hours = stoi(value.substr(0, 2));
        int minutes = stoi(value.substr(3, 2));

        if (period == "AM" && hours == 12)
            hours = 0;
        if (period == "PM" && hours != 12)
            hours += 12;

        char buf[6];
        sprintf(buf, "%02d:%02d", hours, minutes);
        return string(buf);
    }
}

// ---- PUBLISHER (letters, spaces, dots, & allowed) ----
string getValidPublisher(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "INPUT CANNOT BE EMPTY!" << endl;
            continue;
        }

        bool valid = true;
        for (int i = 0; i < (int)value.size(); i++)
        {
            if (!isalpha(value[i]) && value[i] != ' ' &&
                value[i] != '.' && value[i] != '&' && value[i] != '-')
            {
                valid = false;
                break;
            }
        }

        if (!valid)
            cout << "INVALID INPUT! ONLY LETTERS, SPACES, '.', '&', '-' ALLOWED." << endl;
        else
            return value;
    }
}

// ---- ISBN (10 or 13 digits, hyphens allowed) ----
string getValidISBN(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "INPUT CANNOT BE EMPTY!" << endl;
            continue;
        }

        string digitsOnly = "";
        bool valid = true;

        for (int i = 0; i < (int)value.size(); i++)
        {
            if (isdigit(value[i]))
                digitsOnly += value[i];
            else if (value[i] != '-')
            {
                valid = false;
                break;
            }
        }

        if (!valid || (digitsOnly.size() != 10 && digitsOnly.size() != 13))
            cout << "INVALID ISBN! MUST BE 10 OR 13 DIGITS (HYPHENS OPTIONAL)." << endl;
        else
            return value;
    }
}

// ---- CNIC (xxxxx-xxxxxxx-x format) ----
string getValidCNIC(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true;

        if (value.size() != 15)
            valid = false;
        else
        {
            for (int i = 0; i < 15; i++)
            {
                if (i == 5 || i == 13)
                {
                    if (value[i] != '-')
                    {
                        valid = false;
                        break;
                    }
                }
                else
                {
                    if (!isdigit(value[i]))
                    {
                        valid = false;
                        break;
                    }
                }
            }
        }

        if (!valid)
            cout << "INVALID CNIC! FORMAT MUST BE: xxxxx-xxxxxxx-x" << endl;
        else
            return value;
    }
}

// ---- PHONE (03xxxxxxxxx, 11 digits) ----
string getValidPhone(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true;

        if (value.size() != 11)
            valid = false;
        else if (value[0] != '0' || value[1] != '3')
            valid = false;
        else
        {
            for (int i = 0; i < 11; i++)
            {
                if (!isdigit(value[i]))
                {
                    valid = false;
                    break;
                }
            }
        }

        if (!valid)
            cout << "INVALID PHONE! FORMAT MUST BE: 03xxxxxxxxx (11 digits)" << endl;
        else
            return value;
    }
}

// ---- POSITIVE INTEGER ----
int getPositiveInt(string prompt)
{
    int value;
    while (true)
    {
        value = getValidInt(prompt);
        if (value <= 0)
            cout << "VALUE MUST BE GREATER THAN 0!" << endl;
        else
            return value;
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

    // ---- Inverse of toDayNumber(): builds a Date back from a day count ----
    static Date fromDayNumber(long z)
    {
        z += 719468;
        long era = (z >= 0 ? z : z - 146096) / 146097;
        unsigned long doe = (unsigned long)(z - era * 146097);
        unsigned long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        long y = (long)yoe + era * 400;
        unsigned long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        unsigned long mp = (5 * doy + 2) / 153;
        unsigned long d = doy - (153 * mp + 2) / 5 + 1;
        unsigned long m = mp + (mp < 10 ? 3 : -9);
        y += (m <= 2);
        return Date((int)d, (int)m, (int)y);
    }

    // ---- Returns a new Date shifted forward by 'days' days ----
    Date addDays(int days) const
    {
        return fromDayNumber(toDayNumber() + days);
    }

    long daysUntil(const Date &other) const
    {
        return other.toDayNumber() - toDayNumber();
    }

    void inputDate(bool restrictPast = false)
    {
        while (true)
        {
            int d = getValidIntInRange("ENTER DAY(1-31): ", 1, 31);
            int m = getValidIntInRange("ENTER MONTH(1-12): ", 1, 12);
            int y = getValidIntInRange("ENTER YEAR(1900-2100): ", 1900, 2100);

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
            int h = getValidIntInRange("ENTER HOUR(1-12): ", 1, 12);
            int m = getValidIntInRange("ENTER MINUTE(0-59): ", 0, 59);

            cout << "SELECT AM/PM:\n";
            cout << "1. AM\n";
            cout << "2. PM\n";
            int merChoice = getValidIntInRange("ENTER CHOICE: ", 1, 2);
            string mer = (merChoice == 1) ? "AM" : "PM";

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
    string priority;            // "High" / "Medium" / "Low"
    bool isRecurring;            // whether this task repeats
    string recurrenceInterval;   // "None" / "Daily" / "Weekly" / "Monthly"

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
        priority = "Medium";
        isRecurring = false;
        recurrenceInterval = "None";
    }

    virtual ~Task() {}

    int getTaskID() const { return taskID; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    Date getDeadline() const { return deadline; }
    Time getDeadlineTime() const { return deadlineTime; }
    string getCategory() const { return category; }
    string getStatus() const { return status; }
    string getPriority() const { return priority; }
    bool getIsRecurring() const { return isRecurring; }
    string getRecurrenceInterval() const { return recurrenceInterval; }

    void setTitle(string t) { title = t; }
    void setDescription(string d) { description = d; }
    void setDeadline(Date d) { deadline = d; }
    void setDeadlineTime(Time t) { deadlineTime = t; }
    void setPriority(string p) { priority = p; }
    void setRecurring(bool r, string interval = "None")
    {
        isRecurring = r;
        recurrenceInterval = r ? interval : "None";
    }

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
        int choice = getValidIntInRange("ENTER CHOICE: ", 1, 9);

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
            return getNonEmptyLine("ENTER TASK TITLE: ");
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
        cout << "PRIORITY    : " << priority << endl;
        cout << "STATUS      : " << status << endl;
        if (isRecurring)
            cout << "RECURRING   : " << recurrenceInterval << endl;
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
        cout << "\n========== DAILY TASK ==========\n";
        cout << "1. Laundry\n";
        cout << "2. Cleaning House\n";
        cout << "3. Washing Dishes\n";
        cout << "4. Exercise\n";
        cout << "5. Grocery Shopping\n";
        cout << "6. Cooking\n";
        cout << "7. Reading\n";
        cout << "8. Other\n";
        int choice = getValidIntInRange("ENTER CHOICE: ", 1, 8);

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
            return getNonEmptyLine("ENTER TASK TITLE: ");
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
        cout << "PRIORITY    : " << priority << endl;
        cout << "STATUS      : " << status << endl;
        if (isRecurring)
            cout << "RECURRING   : " << recurrenceInterval << endl;
        cout << "====================================\n";
    }
};

// ============================================================
//                       TASK MANAGER CLASS
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
        cout << "\n========== ADD TASK ==========\n";
        cout << "1. Academic Task\n";
        cout << "2. Daily Task\n";
        int choice = getValidIntInRange("ENTER CHOICE: ", 1, 2);

        string title = (choice == 1) ? AcademicTask::getAcademicTitle()
                                      : DailyTask::getDailyTitle();
        string description = getNonEmptyLine("ENTER DESCRIPTION: ");
        Date deadline;
        cout << "\nENTER DEADLINE DATE:\n";
        deadline.inputDate(true);
        Time deadlineTime;
        cout << "\nENTER DEADLINE TIME:\n";
        deadlineTime.inputTime();

        cout << "\nSELECT PRIORITY:\n";
        cout << "1. High\n";
        cout << "2. Medium\n";
        cout << "3. Low\n";
        int pChoice = getValidIntInRange("ENTER CHOICE: ", 1, 3);
        string priority = (pChoice == 1) ? "High" : (pChoice == 2) ? "Medium" : "Low";

        bool recurring = confirmAction("IS THIS A RECURRING TASK?");
        string interval = "None";
        if (recurring)
        {
            cout << "\nSELECT RECURRENCE INTERVAL:\n";
            cout << "1. Daily\n";
            cout << "2. Weekly\n";
            cout << "3. Monthly\n";
            int rChoice = getValidIntInRange("ENTER CHOICE: ", 1, 3);
            interval = (rChoice == 1) ? "Daily" : (rChoice == 2) ? "Weekly" : "Monthly";
        }

        Task *task;
        if (choice == 1)
        {
            task = new AcademicTask(title, description, deadline, deadlineTime);
            cout << "ACADEMIC TASK ADDED SUCCESSFULLY!\n";
        }
        else
        {
            task = new DailyTask(title, description, deadline, deadlineTime);
            cout << "DAILY TASK ADDED SUCCESSFULLY!\n";
        }

        task->setPriority(priority);
        task->setRecurring(recurring, interval);
        tasks.push_back(task);
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

    // Now sorted by priority (High -> Medium -> Low), then by nearest deadline
    void displayPendingTasks() const
    {
        vector<Task *> pending;
        for (int i = 0; i < (int)tasks.size(); i++)
            if (tasks[i]->getStatus() == "Pending")
                pending.push_back(tasks[i]);

        cout << "\n========== PENDING TASKS (SORTED BY PRIORITY) ==========\n";
        if (pending.empty())
        {
            cout << "NO PENDING TASKS!\n";
            return;
        }

        sort(pending.begin(), pending.end(), [](Task *a, Task *b) {
            int wa = priorityWeight(a->getPriority());
            int wb = priorityWeight(b->getPriority());
            if (wa != wb) return wa < wb;
            return a->getDeadline().toComparable() < b->getDeadline().toComparable();
        });

        for (int i = 0; i < (int)pending.size(); i++)
        {
            cout << "\n" << (i + 1) << ".\n";
            pending[i]->displayTask();
        }
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

    // ---- Auto-resets completed recurring tasks once their cycle has passed ----
    void processRecurringTasks()
    {
        Date today = Date::getToday();
        int regenerated = 0;

        for (int i = 0; i < (int)tasks.size(); i++)
        {
            if (tasks[i]->getIsRecurring() &&
                tasks[i]->getStatus() == "Completed" &&
                tasks[i]->getDeadline().isPastDate())
            {
                int stepDays = 1;
                if (tasks[i]->getRecurrenceInterval() == "Weekly") stepDays = 7;
                else if (tasks[i]->getRecurrenceInterval() == "Monthly") stepDays = 30;

                Date newDeadline = tasks[i]->getDeadline();
                while (newDeadline.toComparable() <= today.toComparable())
                    newDeadline = newDeadline.addDays(stepDays);

                tasks[i]->setDeadline(newDeadline);
                tasks[i]->markPending();
                regenerated++;
            }
        }

        if (regenerated > 0)
            cout << "\n[RECURRING TASKS] " << regenerated
                 << " recurring task(s) have been reset for their next cycle.\n";
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
        int id = getValidInt("ENTER TASK ID TO MARK AS COMPLETED: ");
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
        int id = getValidInt("ENTER TASK ID TO DELETE: ");
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
        int id = getValidInt("ENTER TASK ID TO UPDATE: ");
        Task *task = searchByID(id);
        if (task == nullptr)
        {
            cout << "TASK NOT FOUND!\n";
            return;
        }
        cout << "\n========== UPDATE TASK ==========\n";
        cout << "1. Title\n";
        cout << "2. Description\n";
        cout << "3. Deadline\n";
        cout << "4. Time\n";
        cout << "5. Status\n";
        cout << "6. Priority\n";
        cout << "7. Recurring Settings\n";
        cout << "8. Cancel\n";
        int fieldChoice = getValidIntInRange("ENTER CHOICE: ", 1, 8);

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
            cout << "1. Pending\n";
            cout << "2. Completed\n";
            int statusChoice = getValidIntInRange("ENTER CHOICE: ", 1, 2);
            if (statusChoice == 1) task->markPending();
            else task->markCompleted();
            break;
        }
        case 6:
        {
            cout << "1. High\n2. Medium\n3. Low\n";
            int pChoice = getValidIntInRange("ENTER CHOICE: ", 1, 3);
            string priority = (pChoice == 1) ? "High" : (pChoice == 2) ? "Medium" : "Low";
            task->setPriority(priority);
            break;
        }
        case 7:
        {
            bool recurring = confirmAction("SHOULD THIS TASK BE RECURRING?");
            string interval = "None";
            if (recurring)
            {
                cout << "1. Daily\n2. Weekly\n3. Monthly\n";
                int rChoice = getValidIntInRange("ENTER CHOICE: ", 1, 3);
                interval = (rChoice == 1) ? "Daily" : (rChoice == 2) ? "Weekly" : "Monthly";
            }
            task->setRecurring(recurring, interval);
            break;
        }
        case 8:
            cout << "UPDATE CANCELLED!\n";
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
    map<string, double> categoryBudgets;

public:
    ~ExpenseManager()
    {
        for (int i = 0; i < expenses.size(); i++)
            delete expenses[i];
    }

    string selectCategory()
    {
        cout << "\n========== EXPENSE CATEGORY ==========\n";
        cout << "1. Food\n";
        cout << "2. Transport\n";
        cout << "3. Education\n";
        cout << "4. Shopping\n";
        cout << "5. Bills\n";
        cout << "6. Entertainment\n";
        cout << "7. Health\n";
        cout << "8. Other\n";
        int choice = getValidIntInRange("ENTER CHOICE: ", 1, 8);

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
            return getNonEmptyLine("ENTER CATEGORY NAME: ");
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

        checkBudgetAlert(category);
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
        int id = getValidInt("ENTER EXPENSE ID TO DELETE: ");
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
        int id = getValidInt("ENTER EXPENSE ID TO UPDATE: ");
        Expense *expense = searchByID(id);
        if (expense == nullptr)
        {
            cout << "EXPENSE NOT FOUND!\n";
            return;
        }
        cout << "\n========== UPDATE EXPENSE ==========\n";
        cout << "1. Title\n";
        cout << "2. Amount\n";
        cout << "3. Category\n";
        cout << "4. Date\n";
        cout << "5. Description\n";
        cout << "6. Cancel\n";
        int fieldChoice = getValidIntInRange("ENTER CHOICE: ", 1, 6);

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

    // ---- BUDGETS ----
    void setBudget()
    {
        cout << "\n========== SET CATEGORY BUDGET ==========\n";
        string category = selectCategory();
        double limit = getNonNegativeAmount("ENTER MONTHLY BUDGET LIMIT FOR THIS CATEGORY: ");
        categoryBudgets[category] = limit;
        cout << "BUDGET SET FOR " << category << ": " << limit << endl;
    }

    double getCategoryTotal(string category) const
    {
        double total = 0;
        for (int i = 0; i < (int)expenses.size(); i++)
            if (expenses[i]->getCategory() == category)
                total += expenses[i]->getAmount();
        return total;
    }

    void checkBudgetAlert(string category)
    {
        if (categoryBudgets.find(category) == categoryBudgets.end()) return;

        double limit = categoryBudgets[category];
        double spent = getCategoryTotal(category);

        if (spent > limit)
            cout << "\n[BUDGET ALERT] You have EXCEEDED your " << category
                 << " budget! (Spent: " << spent << " / Limit: " << limit << ")\n";
        else if (limit > 0 && spent > limit * 0.8)
            cout << "\n[BUDGET WARNING] You are close to your " << category
                 << " budget! (Spent: " << spent << " / Limit: " << limit << ")\n";
    }

    void viewBudgets() const
    {
        if (categoryBudgets.empty())
        {
            cout << "NO BUDGETS SET YET!\n";
            return;
        }
        cout << "\n========== BUDGET OVERVIEW ==========\n";
        for (map<string, double>::const_iterator it = categoryBudgets.begin();
             it != categoryBudgets.end(); ++it)
        {
            double spent = getCategoryTotal(it->first);
            cout << left << setw(15) << it->first
                 << " Spent: " << setw(10) << spent
                 << " Limit: " << it->second;
            if (spent > it->second) cout << "  [OVER BUDGET]";
            cout << endl;
        }
        cout << "======================================\n";
    }

    // ---- SPENDING CHART (bar chart by category, sorted, with % share) ----
    void displaySpendingChart() const
    {
        if (expenses.empty())
        {
            cout << "NO EXPENSES TO DISPLAY!\n";
            return;
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

        // Sort categories by spend, highest first
        vector<int> order((int)categories.size());
        for (int i = 0; i < (int)order.size(); i++) order[i] = i;
        sort(order.begin(), order.end(), [&](int a, int b) {
            return totals[a] > totals[b];
        });

        double maxVal = 0, grandTotal = 0;
        for (int i = 0; i < (int)totals.size(); i++)
        {
            if (totals[i] > maxVal) maxVal = totals[i];
            grandTotal += totals[i];
        }

        const int maxBarLength = 25;

        cout << "\n============================== SPENDING BY CATEGORY ==============================\n";
        cout << left << setw(14) << "CATEGORY" << " " << left << setw(maxBarLength + 2) << "SHARE"
             << right << setw(12) << "AMOUNT" << setw(9) << "PERCENT" << "\n";
        cout << "------------------------------------------------------------------------------------\n";

        for (int k = 0; k < (int)order.size(); k++)
        {
            int i = order[k];
            int barLength = (maxVal > 0) ? (int)((totals[i] / maxVal) * maxBarLength + 0.5) : 0;
            double percent = (grandTotal > 0) ? (totals[i] / grandTotal) * 100.0 : 0.0;

            string bar = "[" + string(barLength, '#') + string(maxBarLength - barLength, '-') + "]";

            cout << left << setw(14) << categories[i]
                 << " " << bar
                 << right << setw(12) << fixed << setprecision(2) << totals[i]
                 << setw(8) << fixed << setprecision(1) << percent << "%"
                 << "\n";
        }

        cout << "------------------------------------------------------------------------------------\n";
        cout << left << setw(14) << "TOTAL" << " " << setw(maxBarLength + 2) << ""
             << right << setw(12) << fixed << setprecision(2) << grandTotal << setw(9) << "100.0%\n";
        cout << "======================================================================================\n";
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
        cout << "\n========== GOAL CATEGORY ==========\n";
        cout << "1. Academic\n";
        cout << "2. Career\n";
        cout << "3. Health\n";
        cout << "4. Personal Development\n";
        cout << "5. Financial\n";
        cout << "6. Other\n";
        int choice = getValidIntInRange("ENTER CHOICE: ", 1, 6);

        switch (choice)
        {
        case 1: return "Academic";
        case 2: return "Career";
        case 3: return "Health";
        case 4: return "Personal Development";
        case 5: return "Financial";
        default:
            return getNonEmptyLine("ENTER CATEGORY NAME: ");
        }
    }

    void addGoal()
    {
        cout << "\n========== ADD GOAL ==========\n";
        string title = getNonEmptyLine("ENTER GOAL TITLE: ");
        string category = selectCategory();
        Date deadline;
        cout << "ENTER DEADLINE:\n";
        deadline.inputDate(true);
        string description = getNonEmptyLine("ENTER DESCRIPTION: ");
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

    bool completeGoalFlow()
    {
        displayIncompleteGoals();
        if (goals.empty()) return false;
        int id = getValidInt("ENTER GOAL ID TO MARK AS COMPLETED: ");
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
        int id = getValidInt("ENTER GOAL ID TO UPDATE: ");
        Goal *goal = searchByID(id);
        if (goal == nullptr)
        {
            cout << "GOAL NOT FOUND!\n";
            return;
        }
        cout << "\n========== UPDATE GOAL ==========\n";
        cout << "1. Title\n";
        cout << "2. Description\n";
        cout << "3. Category\n";
        cout << "4. Deadline\n";
        cout << "5. Cancel\n";
        int fieldChoice = getValidIntInRange("ENTER CHOICE: ", 1, 5);

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
        }
        cout << "GOAL UPDATED SUCCESSFULLY!\n";
    }

    void deleteGoalFlow()
    {
        displayAllGoals();
        if (goals.empty()) return;
        int id = getValidInt("ENTER GOAL ID TO DELETE: ");
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
//                     ACHIEVEMENT CLASS
// ============================================================

class Achievement
{
private:
    int achievementID;
    static int nextAchievementID;
    string name;
    string description;
    int requiredGoals;
    bool isDefault;

public:
    Achievement(string n, string desc, int reqGoals, bool def = false)
    {
        achievementID = nextAchievementID++;
        name = n;
        description = desc;
        requiredGoals = reqGoals;
        isDefault = def;
    }

    int getAchievementID() const { return achievementID; }
    string getName() const { return name; }
    string getDescription() const { return description; }
    int getRequiredGoals() const { return requiredGoals; }
    bool getIsDefault() const { return isDefault; }

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

class Admin
{
private:
    string username;
    string password; // stored hashed

public:
    Admin(string u = "admin", string p = "admin123")
        : username(u), password(simpleHash(p)) {}

    bool authenticate(string u, string p) const
    {
        return (username == u && password == simpleHash(p));
    }

    string getUsername() const { return username; }

    void changePassword()
    {
        string oldPass = getMaskedNonEmptyLine("ENTER CURRENT PASSWORD: ");
        if (simpleHash(oldPass) != password)
        {
            cout << "WRONG PASSWORD!\n";
            return;
        }
        string newPass = getValidPassword("ENTER NEW PASSWORD: ");
        password = simpleHash(newPass);
        cout << "ADMIN PASSWORD CHANGED SUCCESSFULLY!\n";
    }
};

// Forward declaration of System (needed for User class)
class System;

// ============================================================
//                     USER CLASS (UPDATED)
// ============================================================

class User
{
private:
    string name;
    string username;
    int UID;
    static int next_id;
    Date DOB;
    string password; // stored hashed
    string email;
    Date registrationDate;

    string title;

    bool isActive;
    Date lastLogin;

    int currentStreak;
    int bestStreak;

    vector<int> unlockedAchievementIDs;
    int displayedAchievementID;

    TaskManager taskManager;
    ExpenseManager expenseManager;
    GoalManager goalManager;

    friend class System;

public:
    User()
        : UID(next_id++),
          name("NONE"),
          username(""),
          password(""),
          email(""),
          title(""),
          isActive(false),
          currentStreak(0),
          bestStreak(0),
          displayedAchievementID(-1)
    {
        registrationDate = Date::getToday();
        lastLogin = Date();
    }

    User(string n, string uname, Date dob, string pass, string em = "")
        : name(n),
          username(uname),
          UID(next_id++),
          DOB(dob),
          password(simpleHash(pass)),
          email(em),
          title(""),
          isActive(false),
          currentStreak(0),
          bestStreak(0),
          displayedAchievementID(-1)
    {
        registrationDate = Date::getToday();
        lastLogin = Date();
    }

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
    int getCurrentStreak() const { return currentStreak; }
    int getBestStreak() const { return bestStreak; }

    void setName(string n) { name = n; }
    void setEmail(string e) { email = e; }
    void setDOB(Date d) { DOB = d; }
    void setActive(bool active) { isActive = active; }
    void setLastLogin(Date d) { lastLogin = d; }
    void setDisplayedAchievementID(int id) { displayedAchievementID = id; }

    // ---- Records a login and updates the daily streak ----
    void recordLogin()
    {
        Date today = Date::getToday();

        bool neverLoggedInBefore = (lastLogin.toComparable() == 20000101);

        if (neverLoggedInBefore)
        {
            currentStreak = 1;
        }
        else
        {
            long diff = lastLogin.daysUntil(today);
            if (diff == 1)
                currentStreak++;
            else if (diff > 1)
                currentStreak = 1;
            // diff == 0 (same-day re-login) leaves streak unchanged
        }

        if (currentStreak > bestStreak)
            bestStreak = currentStreak;

        isActive = true;
        lastLogin = today;
    }

    void addUnlockedAchievement(int achID)
    {
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

    string getDisplayName() const
    {
        if (title.empty()) return name;
        return name + " (" + title + ")";
    }

    TaskManager &getTaskManager() { return taskManager; }
    ExpenseManager &getExpenseManager() { return expenseManager; }
    GoalManager &getGoalManager() { return goalManager; }

    bool login(string pass) const { return password == simpleHash(pass); }

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
        cout << "LOGIN STREAK     : " << currentStreak << " day(s) (Best: " << bestStreak << ")" << endl;
        cout << "==============================================\n";
    }

    void showWelcome(const vector<Achievement> &achievements) const
    {
        cout << "\n========== WELCOME " << name << " ==========\n";

        if (displayedAchievementID != -1)
        {
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
        cout << "Login Streak: " << currentStreak << " day(s) (Best: " << bestStreak << ")\n";
        cout << "==============================================\n";
    }

    void changePassword()
    {
        string oldPass = getMaskedNonEmptyLine("ENTER CURRENT PASSWORD: ");
        if (!login(oldPass))
        {
            cout << "WRONG PASSWORD!\n";
            return;
        }
        string newPass = getValidPassword("ENTER NEW PASSWORD: ");
        password = simpleHash(newPass);
        cout << "PASSWORD CHANGED SUCCESSFULLY!\n";
    }

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

class System
{
private:
    vector<User*> users;
    Admin admin;
    vector<Achievement> achievements;

public:
    System()
    {
        loadDefaultAchievements();
    }

    ~System()
    {
        for (int i = 0; i < (int)users.size(); i++)
            delete users[i];
    }

    void loadDefaultAchievements()
    {
        achievements.push_back(Achievement("First Goal", "Complete your very first goal!", 1, true));
        achievements.push_back(Achievement("Rising Star", "Complete 5 goals to become a rising star!", 5, true));
        achievements.push_back(Achievement("Goal Hunter", "Complete 10 goals - you're on a roll!", 10, true));
        achievements.push_back(Achievement("Silver Achiever", "Complete 25 goals - impressive dedication!", 25, true));
        achievements.push_back(Achievement("Gold Achiever", "Complete 50 goals - truly golden!", 50, true));
        achievements.push_back(Achievement("Legend", "Complete 100 goals - you are a legend!", 100, true));
    }

    Achievement *getAchievementByID(int id)
    {
        for (int i = 0; i < (int)achievements.size(); i++)
            if (achievements[i].getAchievementID() == id)
                return &achievements[i];
        return nullptr;
    }

    const vector<Achievement> &getAchievements() const
    {
        return achievements;
    }

    // ---- Checks whether an email is already used by another user ----
    bool emailExists(string email, User *excludeUser = nullptr)
    {
        for (int i = 0; i < (int)users.size(); i++)
        {
            if (users[i] == excludeUser) continue;
            if (toLowerStr(users[i]->getEmail()) == toLowerStr(email))
                return true;
        }
        return false;
    }

    void registerUser()
    {
        cout << "\n========== CREATE ACCOUNT ==========\n";

        string name = getValidAlphaString("ENTER FULL NAME: ");

        string username;
        while (true)
        {
            username = getValidUsername("ENTER USERNAME: ");
            if (findUserByUsername(username) != nullptr)
            {
                cout << "THIS USERNAME IS ALREADY TAKEN! PLEASE CHOOSE ANOTHER.\n";
                continue;
            }
            break;
        }

        string email;
        while (true)
        {
            email = getValidEmail("ENTER EMAIL: ");
            if (emailExists(email))
            {
                cout << "THIS EMAIL IS ALREADY REGISTERED TO ANOTHER ACCOUNT! PLEASE USE ANOTHER.\n";
                continue;
            }
            break;
        }

        Date dob;
        cout << "ENTER DATE OF BIRTH:\n";
        dob.inputDate();

        string password = getConfirmedPassword("SET PASSWORD: ");

        User *newUser = new User(name, username, dob, password, email);
        users.push_back(newUser);

        cout << "ACCOUNT CREATED SUCCESSFULLY! YOU CAN NOW LOGIN.\n";
    }

    User *findUserByUsername(string username)
    {
        for (int i = 0; i < (int)users.size(); i++)
            if (users[i]->getUsername() == username)
                return users[i];
        return nullptr;
    }

    void loginFlow()
    {
        string username = getNonEmptyLine("ENTER USERNAME: ");
        string pass = getMaskedNonEmptyLine("ENTER PASSWORD: ");

        if (admin.authenticate(username, pass))
        {
            cout << "\nADMIN LOGIN SUCCESSFUL!\n";
            adminMenu();
            return;
        }

        if (users.empty())
        {
            cout << "NO ACCOUNTS REGISTERED YET! PLEASE REGISTER FIRST.\n";
            return;
        }

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

        cout << "\nLOGIN SUCCESSFUL!\n";
        user->recordLogin();
        user->getTaskManager().processRecurringTasks();
        user->checkAndUpdateTitle();
        user->showWelcome(achievements);
        showDashboard(*user);
        userMenu(*user);
    }

    void checkAchievements(User *user)
    {
        if (user == nullptr) return;

        int completedGoals = user->getGoalManager().countCompletedGoals();

        for (int i = 0; i < (int)achievements.size(); i++)
        {
            int achID = achievements[i].getAchievementID();
            int reqGoals = achievements[i].getRequiredGoals();

            if (!user->hasAchievement(achID))
            {
                if (completedGoals >= reqGoals)
                {
                    user->addUnlockedAchievement(achID);
                    cout << "\n" << char(4) << " ACHIEVEMENT UNLOCKED: "
                         << achievements[i].getName() << "!\n";
                }
            }
        }
    }

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

        int choice = getValidInt("");
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

    // ---- Update Profile flow (moved here so email uniqueness can be enforced) ----
    void updateProfileFlow(User &user)
    {
        cout << "\n========== UPDATE PROFILE ==========\n";
        cout << "1. Update Name\n";
        cout << "2. Update Email\n";
        cout << "3. Update DOB\n";
        cout << "4. Cancel\n";
        int choice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

        switch (choice)
        {
        case 1:
            user.setName(getValidAlphaString("ENTER NEW NAME: "));
            cout << "NAME UPDATED!\n";
            break;
        case 2:
        {
            string newEmail;
            while (true)
            {
                newEmail = getValidEmail("ENTER NEW EMAIL: ");
                if (emailExists(newEmail, &user))
                {
                    cout << "THIS EMAIL IS ALREADY REGISTERED TO ANOTHER USER! PLEASE USE ANOTHER.\n";
                    continue;
                }
                break;
            }
            user.setEmail(newEmail);
            cout << "EMAIL UPDATED!\n";
            break;
        }
        case 3:
        {
            Date newDOB;
            cout << "ENTER NEW DOB:\n";
            newDOB.inputDate();
            user.setDOB(newDOB);
            cout << "DOB UPDATED!\n";
            break;
        }
        case 4:
            cout << "UPDATE CANCELLED!\n";
            break;
        }
    }

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
        cout << "LOGIN STREAK      : " << user->getCurrentStreak() << " day(s) (Best: " << user->getBestStreak() << ")" << endl;
        cout << "------------------------------------\n";
    }

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

    void adminAddAchievement()
    {
        cout << "\n========== ADD CUSTOM ACHIEVEMENT ==========\n";
        string name = getNonEmptyLine("ENTER ACHIEVEMENT NAME: ");
        string desc = getNonEmptyLine("ENTER DESCRIPTION: ");
        int reqGoals = getPositiveInt("ENTER REQUIRED GOALS TO UNLOCK: ");

        achievements.push_back(Achievement(name, desc, reqGoals, false));
        cout << "CUSTOM ACHIEVEMENT ADDED SUCCESSFULLY!\n";
    }

    void adminRemoveAchievement()
    {
        cout << "\n========== REMOVE ACHIEVEMENT ==========\n";
        cout << "Available achievements:\n";
        for (int i = 0; i < (int)achievements.size(); i++)
        {
            achievements[i].displayBrief();
        }

        int id = getValidInt("ENTER ACHIEVEMENT ID TO REMOVE: ");
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

    void adminUpdateAchievement()
    {
        cout << "\n========== UPDATE ACHIEVEMENT ==========\n";
        cout << "Available achievements:\n";
        for (int i = 0; i < (int)achievements.size(); i++)
        {
            achievements[i].displayBrief();
        }

        int id = getValidInt("ENTER ACHIEVEMENT ID TO UPDATE: ");
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

        cout << "\n1. Update Name\n";
        cout << "2. Update Description\n";
        cout << "3. Update Required Goals\n";
        cout << "4. Cancel\n";
        int fieldChoice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

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
            int req = getPositiveInt("ENTER NEW REQUIRED GOALS: ");
            ach->setRequiredGoals(req);
            cout << "REQUIREMENT UPDATED!\n";
            break;
        }
        case 4:
            cout << "UPDATE CANCELLED!\n";
            return;
        }
    }

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
            choice = getValidIntInRange("ENTER CHOICE: ", 1, 11);

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
            }
        } while (choice != 11);
    }

    // ---- LIFE SCORE: a single composite score of overall progress ----
    int calculateLifeScore(User &user)
    {
        TaskManager &tm = user.getTaskManager();
        GoalManager &gm = user.getGoalManager();

        int totalTasks = tm.countPendingTasks() + tm.countCompletedTasks();
        double taskScore = (totalTasks > 0)
            ? ((double)tm.countCompletedTasks() / totalTasks) * 100.0
            : 100.0;

        int totalGoals = gm.countCompletedGoals() + gm.countPendingGoals() + gm.countOverdueGoals();
        double goalScore = (totalGoals > 0)
            ? ((double)gm.countCompletedGoals() / totalGoals) * 100.0
            : 100.0;

        double overduePenalty = (tm.countOverdueTasks() * 5.0) + (gm.countOverdueGoals() * 5.0);

        double rawScore = (taskScore * 0.4) + (goalScore * 0.4) + 20.0 - overduePenalty;

        if (rawScore > 100) rawScore = 100;
        if (rawScore < 0) rawScore = 0;

        return (int)rawScore;
    }

    string getLifeScoreLabel(int score)
    {
        if (score >= 90) return "Outstanding";
        if (score >= 75) return "Excellent";
        if (score >= 60) return "Good";
        if (score >= 40) return "Fair";
        return "Needs Improvement";
    }

    // ---- Exports a snapshot report of the user's progress to a .txt file ----
    void generateMonthlyReport(User &user)
    {
        string filename = user.getUsername() + "_report.txt";
        ofstream report(filename.c_str());
        if (!report.is_open())
        {
            cout << "ERROR! COULD NOT CREATE REPORT FILE.\n";
            return;
        }

        TaskManager &tm = user.getTaskManager();
        GoalManager &gm = user.getGoalManager();
        ExpenseManager &em = user.getExpenseManager();

        report << "====================================================\n";
        report << "                 REVOLIF - MONTHLY REPORT\n";
        report << "====================================================\n";
        report << "USER        : " << user.getDisplayName() << "\n";
        report << "GENERATED ON: " << Date::getToday() << "\n";
        report << "----------------------------------------------------\n\n";

        report << "TASKS\n";
        report << "  Pending   : " << tm.countPendingTasks() << "\n";
        report << "  Completed : " << tm.countCompletedTasks() << "\n";
        report << "  Overdue   : " << tm.countOverdueTasks() << "\n\n";

        report << "GOALS\n";
        report << "  Completed : " << gm.countCompletedGoals() << "\n";
        report << "  Pending   : " << gm.countPendingGoals() << "\n";
        report << "  Overdue   : " << gm.countOverdueGoals() << "\n\n";

        report << "EXPENSES\n";
        report << "  Total Spent      : " << fixed << setprecision(2) << em.calculateTotalExpense() << "\n";
        pair<string, double> topCat = em.getTopCategoryInfo();
        if (!topCat.first.empty())
            report << "  Highest Category : " << topCat.first << " (" << topCat.second << ")\n";

        report << "\nSTREAK\n";
        report << "  Current Streak : " << user.getCurrentStreak() << " day(s)\n";
        report << "  Best Streak    : " << user.getBestStreak() << " day(s)\n";

        int lifeScore = calculateLifeScore(user);
        report << "\nLIFE SCORE: " << lifeScore << "/100 (" << getLifeScoreLabel(lifeScore) << ")\n";
        report << "====================================================\n";

        report.close();
        cout << "REPORT GENERATED SUCCESSFULLY! SAVED AS: " << filename << "\n";
    }

    void showDashboard(User &user)
    {
        TaskManager &tm = user.getTaskManager();
        GoalManager &gm = user.getGoalManager();
        ExpenseManager &em = user.getExpenseManager();

        cout << "\n================ YOUR DASHBOARD ================\n";
        cout << "WELCOME BACK, " << user.getDisplayName() << "\n";

        int overdueTasks = tm.countOverdueTasks();
        int dueSoonTasks = tm.countDueSoonTasks(3);
        int pendingTasks = tm.countPendingTasks();
        int completedTasks = tm.countCompletedTasks();

        cout << "\nTASKS\n";
        cout << "  - " << overdueTasks << " OVERDUE, "
             << dueSoonTasks << " DUE WITHIN THE NEXT 3 DAYS\n";
        cout << "  - " << pendingTasks << " PENDING TOTAL, "
             << completedTasks << " COMPLETED TOTAL\n";

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

        double totalSpent = em.calculateTotalExpense();
        cout << "\nEXPENSES\n";
        cout << "  - TOTAL SPENT: " << totalSpent << "\n";

        pair<string, double> topCategory = em.getTopCategoryInfo();
        if (!topCategory.first.empty())
        {
            cout << "  - HIGHEST CATEGORY: " << topCategory.first
                 << " (" << topCategory.second << ")\n";
        }

        if (!user.getTitle().empty())
        {
            cout << "\nRECENTLY EARNED TITLE: " << user.getTitle() << "\n";
        }

        int lifeScore = calculateLifeScore(user);
        cout << "\nLIFE SCORE: " << lifeScore << "/100 (" << getLifeScoreLabel(lifeScore) << ")\n";
        cout << "LOGIN STREAK: " << user.getCurrentStreak() << " day(s) (Best: "
             << user.getBestStreak() << ")\n";

        cout << "==================================================\n";

        if (pendingTasks > 0)
        {
            cout << "\nFOLLOWING ARE YOUR PENDING TASKS:\n";
            tm.displayPendingTasks();
        }
    }

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
            cout << "9. Generate Monthly Report\n";
            cout << "10. Logout\n";
            choice = getValidIntInRange("ENTER CHOICE: ", 1, 10);

            switch (choice)
            {
            case 1: user.displayProfile(); break;
            case 2: updateProfileFlow(user); break;
            case 3: taskMenu(user); break;
            case 4: goalMenu(user); break;
            case 5: expenseMenu(user); break;
            case 6: viewUserAchievements(&user); break;
            case 7: chooseDisplayAchievement(&user); break;
            case 8: user.changePassword(); break;
            case 9: generateMonthlyReport(user); break;
            case 10: cout << "LOGGING OUT...\n"; break;
            }
        } while (choice != 10);
    }

    void taskMenu(User &user)
    {
        int choice;
        do
        {
            cout << "\n================ TASK MANAGER ================\n";
            cout << "1. Add Task\n";
            cout << "2. Display All Tasks\n";
            cout << "3. Display Completed Tasks\n";
            cout << "4. Display Pending Tasks (Sorted By Priority)\n";
            cout << "5. Search Task\n";
            cout << "6. Mark Task As Completed\n";
            cout << "7. Update Task\n";
            cout << "8. Delete Task\n";
            cout << "9. Back\n";
            choice = getValidIntInRange("ENTER CHOICE: ", 1, 9);

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
            }
        } while (choice != 9);
    }

    void taskSearchMenu(User &user)
    {
        cout << "\n========== SEARCH TASK ==========\n";
        cout << "1. Search By ID\n";
        cout << "2. Search By Title\n";
        cout << "3. Search By Category\n";
        cout << "4. Search By Deadline\n";
        cout << "5. Search By Status\n";
        int searchChoice = getValidIntInRange("ENTER CHOICE: ", 1, 5);

        switch (searchChoice)
        {
        case 1:
        {
            int id = getValidInt("ENTER TASK ID: ");
            Task *task = user.getTaskManager().searchByID(id);
            if (task) task->displayTask();
            else cout << "TASK NOT FOUND!\n";
            break;
        }
        case 2:
        {
            string title = getNonEmptyLine("ENTER TITLE: ");
            vector<Task *> result = user.getTaskManager().searchByTitle(title);
            user.getTaskManager().displaySearchResults(result);
            break;
        }
        case 3:
        {
            string category = getNonEmptyLine("ENTER CATEGORY (Academic/Daily): ");
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
            string status = getNonEmptyLine("ENTER STATUS (Pending/Completed): ");
            vector<Task *> result = user.getTaskManager().searchByStatus(status);
            user.getTaskManager().displaySearchResults(result);
            break;
        }
        }
    }

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
            choice = getValidIntInRange("ENTER CHOICE: ", 1, 10);

            switch (choice)
            {
            case 1: user.getGoalManager().addGoal(); break;
            case 2: user.getGoalManager().displayAllGoals(); break;
            case 3: user.getGoalManager().displayCompletedGoals(); break;
            case 4: user.getGoalManager().displayIncompleteGoals(); break;
            case 5: goalSearchMenu(user); break;
            case 6:
            {
                bool completed = user.getGoalManager().completeGoalFlow();
                if (completed)
                {
                    user.checkAndUpdateTitle();
                    checkAchievements(&user);
                }
                break;
            }
            case 7: user.getGoalManager().viewAchievements(); break;
            case 8: user.getGoalManager().updateGoalFlow(); break;
            case 9: user.getGoalManager().deleteGoalFlow(); break;
            case 10: cout << "RETURNING TO USER MENU...\n"; break;
            }
        } while (choice != 10);
    }

    void goalSearchMenu(User &user)
    {
        cout << "\n========== SEARCH GOAL ==========\n";
        cout << "1. Search By ID\n";
        cout << "2. Search By Title\n";
        cout << "3. Search By Category\n";
        cout << "4. Search By Deadline\n";
        cout << "5. Search By Status\n";
        int searchChoice = getValidIntInRange("ENTER CHOICE: ", 1, 5);

        switch (searchChoice)
        {
        case 1:
        {
            int id = getValidInt("ENTER GOAL ID: ");
            Goal *goal = user.getGoalManager().searchByID(id);
            if (goal) goal->displayGoal();
            else cout << "GOAL NOT FOUND!\n";
            break;
        }
        case 2:
        {
            string title = getNonEmptyLine("ENTER TITLE: ");
            vector<Goal *> result = user.getGoalManager().searchByTitle(title);
            user.getGoalManager().displaySearchResults(result);
            break;
        }
        case 3:
        {
            string category = getNonEmptyLine("ENTER CATEGORY: ");
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
            string status = getNonEmptyLine("ENTER STATUS (Incomplete/Completed): ");
            vector<Goal *> result = user.getGoalManager().searchByStatus(status);
            user.getGoalManager().displaySearchResults(result);
            break;
        }
        }
    }

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
            cout << "7. Set Category Budget\n";
            cout << "8. View Budgets\n";
            cout << "9. View Spending Chart\n";
            cout << "10. Back\n";
            choice = getValidIntInRange("ENTER CHOICE: ", 1, 10);

            switch (choice)
            {
            case 1: user.getExpenseManager().addExpense(); break;
            case 2: user.getExpenseManager().displayAllExpenses(); break;
            case 3: expenseSearchMenu(user); break;
            case 4: user.getExpenseManager().updateExpenseFlow(); break;
            case 5: user.getExpenseManager().deleteExpenseFlow(); break;
            case 6: cout << "TOTAL EXPENSE: " << user.getExpenseManager().calculateTotalExpense() << endl; break;
            case 7: user.getExpenseManager().setBudget(); break;
            case 8: user.getExpenseManager().viewBudgets(); break;
            case 9: user.getExpenseManager().displaySpendingChart(); break;
            case 10: cout << "RETURNING TO USER MENU...\n"; break;
            }
        } while (choice != 10);
    }

    void expenseSearchMenu(User &user)
    {
        cout << "\n========== SEARCH EXPENSE ==========\n";
        cout << "1. Search By ID\n";
        cout << "2. Search By Title\n";
        cout << "3. Search By Category\n";
        cout << "4. Search By Date\n";
        int searchChoice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

        switch (searchChoice)
        {
        case 1:
        {
            int id = getValidInt("ENTER EXPENSE ID: ");
            Expense *expense = user.getExpenseManager().searchByID(id);
            if (expense) expense->displayExpense();
            else cout << "EXPENSE NOT FOUND!\n";
            break;
        }
        case 2:
        {
            string title = getNonEmptyLine("ENTER TITLE: ");
            vector<Expense *> result = user.getExpenseManager().searchByTitle(title);
            user.getExpenseManager().displaySearchResults(result);
            break;
        }
        case 3:
        {
            string category = getNonEmptyLine("ENTER CATEGORY: ");
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
        }
    }

    void mainMenu()
    {
        int choice;
        do
        {
            cout << "\n================================================\n";
            cout << "                    R E V O L I F\n";
            cout << "              Life , Beautifully  Aligned\n";
            cout << "================================================\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. Exit\n";
            choice = getValidIntInRange("ENTER CHOICE: ", 1, 3);

            switch (choice)
            {
            case 1: loginFlow(); break;
            case 2: registerUser(); break;
            case 3: cout << "EXITING REVOLIF...\n"; break;
            }
        } while (choice != 3);
    }
};

// ============================================================
//                           MAIN
// ============================================================

int main()
{
    System system;
    system.mainMenu();
    return 0;
}

