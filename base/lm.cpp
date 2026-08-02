#include <iostream>
#include <string>
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
        cout << "5. EXIT" << endl;
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
            cout << "EXITING..." << endl;
            break;
        }

    } while (choice != 5);

    return 0;
}