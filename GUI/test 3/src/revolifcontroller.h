#ifndef REVOLIFCONTROLLER_H
#define REVOLIFCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QString>

class System;
class User;

class RevolifController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY isLoggedInChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY isAdminChanged)
    Q_PROPERTY(QString currentUserName READ currentUserName NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserTitle READ currentUserTitle NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserEmail READ currentUserEmail NOTIFY currentUserChanged)
    Q_PROPERTY(int currentStreak READ currentStreak NOTIFY currentUserChanged)
    Q_PROPERTY(int bestStreak READ bestStreak NOTIFY currentUserChanged)
    Q_PROPERTY(int lifeScore READ lifeScore NOTIFY statsChanged)
    Q_PROPERTY(QString lifeScoreLabel READ lifeScoreLabel NOTIFY statsChanged)
    Q_PROPERTY(int pendingTasks READ pendingTasks NOTIFY statsChanged)
    Q_PROPERTY(int completedTasks READ completedTasks NOTIFY statsChanged)
    Q_PROPERTY(int overdueTasks READ overdueTasks NOTIFY statsChanged)
    Q_PROPERTY(int pendingGoals READ pendingGoals NOTIFY statsChanged)
    Q_PROPERTY(int completedGoals READ completedGoals NOTIFY statsChanged)
    Q_PROPERTY(double totalExpenses READ totalExpenses NOTIFY statsChanged)
    Q_PROPERTY(QString pageTitle READ pageTitle WRITE setPageTitle NOTIFY pageTitleChanged)

public:
    explicit RevolifController(QObject *parent = nullptr);
    ~RevolifController();

    bool isLoggedIn() const;
    bool isAdmin() const;
    QString currentUserName() const;
    QString currentUserTitle() const;
    QString currentUserEmail() const;
    int currentStreak() const;
    int bestStreak() const;
    int lifeScore() const;
    QString lifeScoreLabel() const;
    int pendingTasks() const;
    int completedTasks() const;
    int overdueTasks() const;
    int pendingGoals() const;
    int completedGoals() const;
    double totalExpenses() const;
    QString pageTitle() const;
    void setPageTitle(const QString &title);

    Q_INVOKABLE bool login(const QString &username, const QString &password);
    Q_INVOKABLE bool registerUser(const QString &name, const QString &username,
                                   const QString &dob, const QString &email,
                                   const QString &password);
    Q_INVOKABLE void logout();

    Q_INVOKABLE QVariantList getTasks();
    Q_INVOKABLE bool addTask(int type, const QString &title, const QString &description,
                              int day, int month, int year, int hour, int minute,
                              const QString &meridiem, const QString &priority,
                              bool recurring, const QString &interval);
    Q_INVOKABLE bool updateTask(int id, const QString &field, const QVariant &value);
    Q_INVOKABLE bool deleteTask(int id);
    Q_INVOKABLE bool completeTask(int id);

    Q_INVOKABLE QVariantList getGoals();
    Q_INVOKABLE bool addGoal(const QString &title, const QString &description,
                              const QString &category, int day, int month, int year);
    Q_INVOKABLE bool updateGoal(int id, const QString &field, const QVariant &value);
    Q_INVOKABLE bool deleteGoal(int id);
    Q_INVOKABLE bool completeGoal(int id);

    Q_INVOKABLE QVariantList getExpenses();
    Q_INVOKABLE bool addExpense(const QString &title, double amount, const QString &category,
                                 int day, int month, int year, const QString &description);
    Q_INVOKABLE bool updateExpense(int id, const QString &field, const QVariant &value);
    Q_INVOKABLE bool deleteExpense(int id);

    Q_INVOKABLE QVariantMap getDashboardData();
    Q_INVOKABLE QVariantMap getProfileData();
    Q_INVOKABLE bool updateProfile(const QString &field, const QVariant &value);
    Q_INVOKABLE bool changePassword(const QString &oldPass, const QString &newPass);
    Q_INVOKABLE bool deleteMyAccount(const QString &password);
    Q_INVOKABLE bool generateMonthlyReport();

    Q_INVOKABLE QVariantList getAchievements();
    Q_INVOKABLE bool setDisplayedAchievement(int id);
    Q_INVOKABLE int getDisplayedAchievementId();

    Q_INVOKABLE QVariantList getBudgets();
    Q_INVOKABLE bool setBudget(const QString &category, double limit);
    Q_INVOKABLE QVariantMap getSpendingByCategory();

    Q_INVOKABLE bool isCurrentUserAdmin() const;
    Q_INVOKABLE QVariantList getAllUsers();
    Q_INVOKABLE bool suspendUser(const QString &username);
    Q_INVOKABLE bool permanentlyDeleteUser(const QString &username, const QString &reason);
    Q_INVOKABLE QVariantList getPermanentlyDeletedUsers();
    Q_INVOKABLE QVariantMap getSystemStatistics();
    Q_INVOKABLE bool generateSystemReport();
    Q_INVOKABLE bool adminChangePassword(const QString &oldPass, const QString &newPass);
    Q_INVOKABLE bool addAchievement(const QString &name, const QString &description, int requiredGoals);
    Q_INVOKABLE bool removeAchievement(int id);
    Q_INVOKABLE bool updateAchievement(int id, const QString &field, const QVariant &value);

    Q_INVOKABLE QString getErrorMessage() const;

signals:
    void isLoggedInChanged();
    void isAdminChanged();
    void currentUserChanged();
    void statsChanged();
    void pageTitleChanged();
    void errorOccurred(const QString &message);
    void successMessage(const QString &message);

private:
    void updateStats();
    System *system;
    User *currentUser;
    bool adminMode;
    QString m_pageTitle;
    QString m_errorMessage;
};

#endif // REVOLIFCONTROLLER_H
