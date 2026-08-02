#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Date;
class Authentication;
class User;
class Admin;

class Date
{
private:
    int day;
    int year;
    string month;

public:
    Date(int d = 1, string m = " ", int y = 2000)
    {
        day = d;
        year = y;
        month = m;
    }

    void setday(int d) { day = d; }
    void setyear(int y) { year = y; }
    void setmonth(string m) { month = m; }

    int getday() { return day; }
    string getmonth() { return month; }
    int getyear() { return year; }

    void displayDate()
    {
        cout << "| DATE:" << day << "-" << month << "-" << year << "|" << endl;
    }
};

class User
{
private:
    int userID;
    static int nextUserID; // used to auto-generate userID

    string name;
    Date DateOfBirth;

    string username;
    string password;
    string email;
    bool status;

public:
    User(string n = "UNKNOWN", Date dob = Date(), string u = "unknown", string p = "123456", string e = "unknown@gmail.com")
    {
        userID = nextUserID++;
        name = n;
        DateOfBirth = dob;
        username = u;
        password = p;
        email = e;
        status=true;
    }

    // setters
    void setName(string n) { name = n; }
    void setDateOfBirth(Date dob) { DateOfBirth = dob; }
    void setUsername(string u) { username = u; }
    void setPassword(string p) { password = p; }
    void setEmail(string e) { email = e; }

    // Getters
    int getUserID() const { return userID; }
    string getName() const { return name; }
    Date getDateOfBirth() const { return DateOfBirth; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getEmail() const { return email; }

    void displayUser()
    {
        cout << "================ USER DETAILS ================" << endl;
        cout << "| USER ID: " << userID << endl;
        cout << "| NAME: " << name << endl;
        cout << "| DATE OF BIRTH: ";
        DateOfBirth.displayDate();
        cout << "| USERNAME: " << username << endl;
        cout << "| EMAIL: " << email << endl;
        cout << "==============================================" << endl;
    }

    void updateUser();
};

// static member definition - starts IDs at 1
int User::nextUserID = 1;

class Admin
{
private:
    string username;
    string password;
    vector<User *> Users;

public:
    Admin(string u = "admin", string p = "admin123")
    {
        username = u;
        password = p;
    }

    string getUsername() const { return username; }
    string getPassword() const { return password; }

    void displayAdmin()
    {
        cout << "================ ADMIN DETAILS ================" << endl;
        cout << "| USERNAME: " << username << endl;
        cout << "===============================================" << endl;
    }
};

class Authentication
{
public:
    bool userLogin(const User &user, string username, string password)
    {
        if (user.getUsername() == username && user.getPassword() == password)
        {
            cout << "User Login Successful!" << endl;
            return true;
        }

        cout << "Invalid User Credentials!" << endl;
        return false;
    }

    bool adminLogin(const Admin &admin, string username, string password)
    {
        if (admin.getUsername() == username &&
            admin.getPassword() == password)
        {
            cout << "Admin Login Successful!" << endl;
            return true;
        }

        cout << "Invalid Admin Credentials!" << endl;
        return false;
    }

    void logout()
    {
        cout << "Logged out successfully!" << endl;
    }
};

class System
{
private:
    vector<User> users;
    Admin admin;

public:
    vector<User> &getUsers()
    {
        return users;
    }

    Admin &getAdmin()
    {
        return admin;
    }

    //================REMOVE==================
    // ================= REMOVE BY ID =================

    bool removeUserByID(int id)
    {
        for (auto it = users.begin(); it != users.end(); it++)
        {
            if (it->getUserID() == id)
            {
                users.erase(it);

                cout << "USER REMOVED SUCCESSFULLY!" << endl;
                return true;
            }
        }

        cout << "USER NOT FOUND!" << endl;
        return false;
    }

    // ================= REGISTER =================

    void registerUser()
    {
        string name;
        string username;
        string password;
        string email;

        int day;
        int year;
        string month;

        cout << "ENTER NAME: ";
        cin >> name;

        cout << "ENTER DATE OF BIRTH\n";

        cout << "DAY: ";
        cin >> day;

        cout << "MONTH: ";
        cin >> month;

        cout << "YEAR: ";
        cin >> year;

        Date dob(day, month, year);

        cout << "ENTER USERNAME: ";
        cin >> username;

        cout << "ENTER PASSWORD: ";
        cin >> password;

        cout << "ENTER EMAIL: ";
        cin >> email;

        User newUser(name, dob, username, password, email);

        users.push_back(newUser);

        cout << "REGISTRATION SUCCESSFUL!" << endl;
    }

    // ================= REMOVE BY USERNAME =================

    bool removeUserByUsername(string username)
    {
        for (auto it = users.begin(); it != users.end(); it++)
        {
            if (it->getUsername() == username)
            {
                users.erase(it);

                cout << "USER REMOVED SUCCESSFULLY!" << endl;
                return true;
            }
        }

        cout << "USER NOT FOUND!" << endl;
        return false;
    }

    // ================= SEARCH =================

    User *SearchbyID(int id)
    {
        for (int i = 0; i < users.size(); i++)
        {
            User *user = &users[i];

            if (user->getUserID() == id)
                return user;
        }

        return nullptr;
    }

    User *SearchbyUsername(string u)
    {
        for (int i = 0; i < users.size(); i++)
        {
            User *user = &users[i];

            if (user->getUsername() == u)
                return user;
        }

        return nullptr;
    }

    User *SearchbyEmail(string e)
    {
        for (int i = 0; i < users.size(); i++)
        {
            User *user = &users[i];

            if (user->getEmail() == e)
                return user;
        }

        return nullptr;
    }

    User *SearchbyName(string n)
    {
        for (int i = 0; i < users.size(); i++)
        {
            User *user = &users[i];

            if (user->getName() == n)
                return user;
        }

        return nullptr;
    }

    void ViewUserList() const
    {
        if (users.empty())
        {
            cout << "NO USERS REGISTERED!" << endl;
            return;
        }

        cout << "========== USER LIST ==========" << endl;

        for (User u : users)
            u.displayUser();
    }
};

void User::updateUser()
{
    int choice;

    do
    {
        cout << "\n========== UPDATE PROFILE ==========" << endl;
        cout << "1. Update Name" << endl;
        cout << "2. Update Date Of Birth" << endl;
        cout << "3. Update Username" << endl;
        cout << "4. Update Password" << endl;
        cout << "5. Update Email" << endl;
        cout << "6. Exit" << endl;

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string n;

            cout << "Enter new name: ";
            cin >> n;

            name = n;

            cout << "Name updated successfully!" << endl;
            break;
        }

        case 2:
        {
            int d, y;
            string m;

            cout << "Enter new day: ";
            cin >> d;

            cout << "Enter new month: ";
            cin >> m;

            cout << "Enter new year: ";
            cin >> y;

            DateOfBirth = Date(d, m, y);

            cout << "Date of Birth updated successfully!" << endl;
            break;
        }

        case 3:
        {
            string u;

            cout << "Enter new username: ";
            cin >> u;

            username = u;

            cout << "Username updated successfully!" << endl;
            break;
        }

        case 4:
        {
            string p;

            cout << "Enter new password: ";
            cin >> p;

            password = p;

            cout << "Password updated successfully!" << endl;
            break;
        }

        case 5:
        {
            string e;

            cout << "Enter new email: ";
            cin >> e;

            email = e;

            cout << "Email updated successfully!" << endl;
            break;
        }

        case 6:
        {
            cout << "Exiting update menu..." << endl;
            break;
        }

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 6);
}