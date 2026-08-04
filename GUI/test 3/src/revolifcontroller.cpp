#include "revolifcontroller.h"
#include "../core/revolif_backend.cpp"
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

RevolifController::RevolifController(QObject *parent)
    : QObject(parent), system(nullptr), currentUser(nullptr), adminMode(false)
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    system = new System();
    m_pageTitle = "Dashboard";
}

RevolifController::~RevolifController()
{
    delete system;
}

bool RevolifController::isLoggedIn() const { return currentUser != nullptr; }
bool RevolifController::isAdmin() const { return adminMode; }

QString RevolifController::currentUserName() const {
    return currentUser ? QString::fromStdString(currentUser->getDisplayName()) : "";
}

QString RevolifController::currentUserTitle() const {
    return currentUser ? QString::fromStdString(currentUser->getTitle()) : "";
}

QString RevolifController::currentUserEmail() const {
    return currentUser ? QString::fromStdString(currentUser->getEmail()) : "";
}

int RevolifController::currentStreak() const {
    return currentUser ? currentUser->getCurrentStreak() : 0;
}

int RevolifController::bestStreak() const {
    return currentUser ? currentUser->getBestStreak() : 0;
}

int RevolifController::lifeScore() const {
    return currentUser ? system->calculateLifeScore(*currentUser) : 0;
}

QString RevolifController::lifeScoreLabel() const {
    return currentUser ? QString::fromStdString(system->getLifeScoreLabel(system->calculateLifeScore(*currentUser))) : "";
}

int RevolifController::pendingTasks() const {
    return currentUser ? currentUser->getTaskManager().countPendingTasks() : 0;
}

int RevolifController::completedTasks() const {
    return currentUser ? currentUser->getTaskManager().countCompletedTasks() : 0;
}

int RevolifController::overdueTasks() const {
    return currentUser ? currentUser->getTaskManager().countOverdueTasks() : 0;
}

int RevolifController::pendingGoals() const {
    return currentUser ? currentUser->getGoalManager().countPendingGoals() : 0;
}

int RevolifController::completedGoals() const {
    return currentUser ? currentUser->getGoalManager().countCompletedGoals() : 0;
}

double RevolifController::totalExpenses() const {
    return currentUser ? currentUser->getExpenseManager().calculateTotalExpense() : 0.0;
}

QString RevolifController::pageTitle() const { return m_pageTitle; }

void RevolifController::setPageTitle(const QString &title) {
    if (m_pageTitle != title) {
        m_pageTitle = title;
        emit pageTitleChanged();
    }
}

bool RevolifController::login(const QString &username, const QString &password) {
    User *user = nullptr;
    LoginResult result = system->getAuth().attemptLogin(username.toStdString(), password.toStdString(), user);

    switch (result) {
    case LOGIN_ADMIN:
        adminMode = true;
        currentUser = nullptr;
        emit isLoggedInChanged();
        emit isAdminChanged();
        emit currentUserChanged();
        return true;
    case LOGIN_SUCCESS:
        adminMode = false;
        currentUser = user;
        system->prepareUserSession(user);
        emit isLoggedInChanged();
        emit isAdminChanged();
        emit currentUserChanged();
        emit statsChanged();
        return true;
    case LOGIN_SUSPENDED:
        emit errorOccurred("Account suspended. Please restore or contact admin.");
        return false;
    case LOGIN_NO_ACCOUNTS:
        emit errorOccurred("No accounts registered yet.");
        return false;
    case LOGIN_USER_NOT_FOUND:
        emit errorOccurred("User not found.");
        return false;
    case LOGIN_WRONG_PASSWORD:
        emit errorOccurred("Invalid password.");
        return false;
    }
    return false;
}

bool RevolifController::registerUser(const QString &name, const QString &username,
                                      const QString &dob, const QString &email,
                                      const QString &password) {
    try {
        QStringList parts = dob.split('/');
        if (parts.size() != 3) {
            emit errorOccurred("Invalid date format. Use DD/MM/YYYY");
            return false;
        }
        Date dobDate(parts[0].toInt(), parts[1].toInt(), parts[2].toInt());

        if (system->getAuth().lookupUserByUsername(username.toStdString()) != nullptr) {
            emit errorOccurred("Username already exists.");
            return false;
        }

        if (system->getAuth().emailExists(email.toStdString())) {
            emit errorOccurred("Email already registered.");
            return false;
        }

        User *newUser = new User(name.toStdString(), username.toStdString(), dobDate, password.toStdString(), email.toStdString());
        system->getUsers().push_back(newUser);
        system->saveUserToFile(newUser);
        emit successMessage("Account created successfully!");
        return true;
    } catch (const std::exception &e) {
        emit errorOccurred(QString::fromStdString(e.what()));
        return false;
    }
}

void RevolifController::logout() {
    currentUser = nullptr;
    adminMode = false;
    emit isLoggedInChanged();
    emit isAdminChanged();
    emit currentUserChanged();
    emit statsChanged();
}

QVariantList RevolifController::getTasks() {
    QVariantList list;
    if (!currentUser) return list;

    const std::vector<Task*> &tasks = currentUser->getTaskManager().getTasks();
    for (Task *t : tasks) {
        QVariantMap m;
        m["id"] = t->getTaskID();
        m["title"] = QString::fromStdString(t->getTitle());
        m["description"] = QString::fromStdString(t->getDescription());
        m["category"] = QString::fromStdString(t->getCategory());
        m["status"] = QString::fromStdString(t->getStatus());
        m["priority"] = QString::fromStdString(t->getPriority());
        m["deadline"] = QString::fromStdString(t->getDeadline().toString());
        m["time"] = QString::fromStdString(t->getDeadlineTime().toString());
        m["isRecurring"] = t->getIsRecurring();
        m["recurrenceInterval"] = QString::fromStdString(t->getRecurrenceInterval());
        m["overdue"] = (t->getStatus() == "Pending" && t->getDeadline().isPastDate());
        list.append(m);
    }
    return list;
}

bool RevolifController::addTask(int type, const QString &title, const QString &description,
                                 int day, int month, int year, int hour, int minute,
                                 const QString &meridiem, const QString &priority,
                                 bool recurring, const QString &interval) {
    if (!currentUser) return false;
    try {
        Date deadline(day, month, year);
        Time deadlineTime(hour, minute, meridiem.toStdString());
        Task *task;
        if (type == 1)
            task = new AcademicTask(title.toStdString(), description.toStdString(), deadline, deadlineTime);
        else
            task = new DailyTask(title.toStdString(), description.toStdString(), deadline, deadlineTime);
        task->setPriority(priority.toStdString());
        task->setRecurring(recurring, interval.toStdString());
        currentUser->getTaskManager().loadTask(task);
        system->saveTaskToFile(task, currentUser->getUID());
        emit statsChanged();
        return true;
    } catch (...) {
        emit errorOccurred("Failed to add task.");
        return false;
    }
}

bool RevolifController::updateTask(int id, const QString &field, const QVariant &value) {
    if (!currentUser) return false;
    Task *t = currentUser->getTaskManager().searchByID(id);
    if (!t) return false;

    std::string f = field.toStdString();
    if (f == "title") t->setTitle(value.toString().toStdString());
    else if (f == "description") t->setDescription(value.toString().toStdString());
    else if (f == "priority") t->setPriority(value.toString().toStdString());
    else if (f == "status") {
        if (value.toString() == "Completed") t->markCompleted();
        else t->markPending();
    }

    system->rewriteTasksFile();
    emit statsChanged();
    return true;
}

bool RevolifController::deleteTask(int id) {
    if (!currentUser) return false;
    currentUser->getTaskManager().deleteTaskById(id);
    system->rewriteTasksFile();
    emit statsChanged();
    return true;
}

bool RevolifController::completeTask(int id) {
    if (!currentUser) return false;
    currentUser->getTaskManager().completeTaskById(id);
    system->rewriteTasksFile();
    emit statsChanged();
    return true;
}

QVariantList RevolifController::getGoals() {
    QVariantList list;
    if (!currentUser) return list;

    const std::vector<Goal*> &goals = currentUser->getGoalManager().getGoals();
    for (Goal *g : goals) {
        QVariantMap m;
        m["id"] = g->getGoalID();
        m["title"] = QString::fromStdString(g->getTitle());
        m["description"] = QString::fromStdString(g->getDescription());
        m["category"] = QString::fromStdString(g->getCategory());
        m["status"] = QString::fromStdString(g->getStatus());
        m["displayStatus"] = QString::fromStdString(g->getDisplayStatus());
        m["deadline"] = QString::fromStdString(g->getDeadline().toString());
        m["overdue"] = (g->getStatus() != "Completed" && g->getDeadline().isPastDate());
        list.append(m);
    }
    return list;
}

bool RevolifController::addGoal(const QString &title, const QString &description,
                                 const QString &category, int day, int month, int year) {
    if (!currentUser) return false;
    try {
        Date deadline(day, month, year);
        Goal *goal = new Goal(title.toStdString(), description.toStdString(), category.toStdString(), deadline);
        currentUser->getGoalManager().loadGoal(goal);
        system->saveGoalToFile(goal, currentUser->getUID());
        emit statsChanged();
        return true;
    } catch (...) {
        emit errorOccurred("Failed to add goal.");
        return false;
    }
}

bool RevolifController::updateGoal(int id, const QString &field, const QVariant &value) {
    if (!currentUser) return false;
    Goal *g = currentUser->getGoalManager().searchByID(id);
    if (!g) return false;

    std::string f = field.toStdString();
    if (f == "title") g->setTitle(value.toString().toStdString());
    else if (f == "description") g->setDescription(value.toString().toStdString());
    else if (f == "category") g->setCategory(value.toString().toStdString());

    system->rewriteGoalsFile();
    emit statsChanged();
    return true;
}

bool RevolifController::deleteGoal(int id) {
    if (!currentUser) return false;
    currentUser->getGoalManager().deleteGoalById(id);
    system->rewriteGoalsFile();
    emit statsChanged();
    return true;
}

bool RevolifController::completeGoal(int id) {
    if (!currentUser) return false;
    currentUser->getGoalManager().completeGoalById(id);
    system->checkAchievements(currentUser);
    currentUser->checkAndUpdateTitle();
    system->rewriteGoalsFile();
    system->rewriteUsersFile();
    emit statsChanged();
    emit currentUserChanged();
    return true;
}

QVariantList RevolifController::getExpenses() {
    QVariantList list;
    if (!currentUser) return list;

    const std::vector<Expense*> &expenses = currentUser->getExpenseManager().getExpenses();
    for (Expense *e : expenses) {
        QVariantMap m;
        m["id"] = e->getExpenseID();
        m["title"] = QString::fromStdString(e->getTitle());
        m["amount"] = e->getAmount();
        m["category"] = QString::fromStdString(e->getCategory());
        m["date"] = QString::fromStdString(e->getDate().toString());
        m["description"] = QString::fromStdString(e->getDescription());
        list.append(m);
    }
    return list;
}

bool RevolifController::addExpense(const QString &title, double amount, const QString &category,
                                    int day, int month, int year, const QString &description) {
    if (!currentUser) return false;
    try {
        Date date(day, month, year);
        Expense *expense = new Expense(title.toStdString(), amount, category.toStdString(), date, description.toStdString());
        currentUser->getExpenseManager().loadExpense(expense);
        system->saveExpenseToFile(expense, currentUser->getUID());
        emit statsChanged();
        return true;
    } catch (...) {
        emit errorOccurred("Failed to add expense.");
        return false;
    }
}

bool RevolifController::updateExpense(int id, const QString &field, const QVariant &value) {
    if (!currentUser) return false;
    Expense *e = currentUser->getExpenseManager().searchByID(id);
    if (!e) return false;

    std::string f = field.toStdString();
    if (f == "title") e->setTitle(value.toString().toStdString());
    else if (f == "amount") e->setAmount(value.toDouble());
    else if (f == "category") e->setCategory(value.toString().toStdString());
    else if (f == "description") e->setDescription(value.toString().toStdString());

    system->rewriteExpensesFile();
    emit statsChanged();
    return true;
}

bool RevolifController::deleteExpense(int id) {
    if (!currentUser) return false;
    currentUser->getExpenseManager().deleteExpenseById(id);
    system->rewriteExpensesFile();
    emit statsChanged();
    return true;
}

QVariantMap RevolifController::getDashboardData() {
    QVariantMap data;
    if (!currentUser) return data;

    TaskManager& tm = currentUser->getTaskManager();
    GoalManager& gm = currentUser->getGoalManager();
    ExpenseManager& em = currentUser->getExpenseManager();

    data["userName"] = QString::fromStdString(currentUser->getDisplayName());
    data["title"] = QString::fromStdString(currentUser->getTitle());
    int score = system->calculateLifeScore(*currentUser);
    data["lifeScore"] = score;
    data["lifeScoreLabel"] = QString::fromStdString(system->getLifeScoreLabel(score));
    data["currentStreak"] = currentUser->getCurrentStreak();
    data["bestStreak"] = currentUser->getBestStreak();
    data["pendingTasks"] = tm.countPendingTasks();
    data["completedTasks"] = tm.countCompletedTasks();
    data["overdueTasks"] = tm.countOverdueTasks();
    data["dueSoonTasks"] = tm.countDueSoonTasks(3);
    data["pendingGoals"] = gm.countPendingGoals();
    data["completedGoals"] = gm.countCompletedGoals();
    data["overdueGoals"] = gm.countOverdueGoals();
    data["totalExpenses"] = em.calculateTotalExpense();

    auto topCat = em.getTopCategoryInfo();
    data["topCategory"] = QString::fromStdString(topCat.first);
    data["topCategoryAmount"] = topCat.second;
    data["nextTitle"] = QString::fromStdString(gm.getNextTitleName());
    data["goalsUntilNextTitle"] = gm.goalsUntilNextTitle();

    QVariantList focusTasks;
    std::vector<Task*> pending;
    for (Task* t : tm.getTasks()) {
        if (t->getStatus() == "Pending") pending.push_back(t);
    }
    std::sort(pending.begin(), pending.end(), [](Task* a, Task* b) {
        int wa = (a->getPriority() == "High") ? 1 : (a->getPriority() == "Medium") ? 2 : 3;
        int wb = (b->getPriority() == "High") ? 1 : (b->getPriority() == "Medium") ? 2 : 3;
        if (wa != wb) return wa < wb;
        return a->getDeadline().toComparable() < b->getDeadline().toComparable();
    });
    for (size_t i = 0; i < pending.size() && i < 3; i++) {
        QVariantMap t;
        t["id"] = pending[i]->getTaskID();
        t["title"] = QString::fromStdString(pending[i]->getTitle());
        t["category"] = QString::fromStdString(pending[i]->getCategory());
        t["priority"] = QString::fromStdString(pending[i]->getPriority());
        t["deadline"] = QString::fromStdString(pending[i]->getDeadline().toString());
        focusTasks.append(t);
    }
    data["focusTasks"] = focusTasks;

    QVariantList upcoming;
    for (size_t i = 0; i < pending.size() && i < 5; i++) {
        QVariantMap t;
        t["id"] = pending[i]->getTaskID();
        t["title"] = QString::fromStdString(pending[i]->getTitle());
        t["deadline"] = QString::fromStdString(pending[i]->getDeadline().toString());
        t["time"] = QString::fromStdString(pending[i]->getDeadlineTime().toString());
        upcoming.append(t);
    }
    data["upcomingTasks"] = upcoming;

    return data;
}

QVariantMap RevolifController::getProfileData() {
    QVariantMap data;
    if (!currentUser) return data;
    data["uid"] = currentUser->getUID();
    data["username"] = QString::fromStdString(currentUser->getUsername());
    data["name"] = QString::fromStdString(currentUser->getName());
    data["email"] = QString::fromStdString(currentUser->getEmail());
    data["dob"] = QString::fromStdString(currentUser->getDOB().toString());
    data["registrationDate"] = QString::fromStdString(currentUser->getRegistrationDate().toString());
    data["lastLogin"] = currentUser->getLastLogin().toComparable() == 20000101 
                         ? "Never" : QString::fromStdString(currentUser->getLastLogin().toString());
    data["status"] = currentUser->getIsActive() ? "Active" : "Inactive";
    data["achievements"] = currentUser->getUnlockedCount();
    data["streak"] = currentUser->getCurrentStreak();
    data["bestStreak"] = currentUser->getBestStreak();
    return data;
}

bool RevolifController::updateProfile(const QString &field, const QVariant &value) {
    if (!currentUser) return false;
    std::string f = field.toStdString();
    if (f == "name") currentUser->setName(value.toString().toStdString());
    else if (f == "email") currentUser->setEmail(value.toString().toStdString());
    system->rewriteUsersFile();
    emit currentUserChanged();
    return true;
}

bool RevolifController::changePassword(const QString &oldPass, const QString &newPass) {
    if (!currentUser) return false;
    try {
        if (!system->getAuth().verifyPassword(currentUser, oldPass.toStdString())) {
            emit errorOccurred("Wrong current password.");
            return false;
        }
        if (newPass.length() < 6) {
            emit errorOccurred("Password must be at least 6 characters.");
            return false;
        }
        currentUser->setPasswordHash(simpleHash(newPass.toStdString()));
        system->rewriteUsersFile();
        emit successMessage("Password changed successfully.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to change password.");
        return false;
    }
}

bool RevolifController::deleteMyAccount(const QString &password) {
    if (!currentUser) return false;
    if (!system->getAuth().verifyPassword(currentUser, password.toStdString())) {
        emit errorOccurred("Wrong password.");
        return false;
    }
    currentUser->setActive(false);
    currentUser->setDeactivatedBySelf(true);
    system->rewriteUsersFile();
    logout();
    emit successMessage("Account deleted successfully.");
    return true;
}

bool RevolifController::generateMonthlyReport() {
    if (!currentUser) return false;
    try {
        system->generateMonthlyReport(*currentUser);
        emit successMessage("Report generated successfully.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to generate report.");
        return false;
    }
}

QVariantList RevolifController::getAchievements() {
    QVariantList list;
    const std::vector<Achievement> &achs = system->getAchievements();
    std::vector<int> unlocked = currentUser ? currentUser->getUnlockedAchievementIDs() : std::vector<int>();

    for (const Achievement &a : achs) {
        QVariantMap m;
        m["id"] = a.getAchievementID();
        m["name"] = QString::fromStdString(a.getName());
        m["description"] = QString::fromStdString(a.getDescription());
        m["requiredGoals"] = a.getRequiredGoals();
        m["isDefault"] = a.getIsDefault();
        m["unlocked"] = std::find(unlocked.begin(), unlocked.end(), a.getAchievementID()) != unlocked.end();
        list.append(m);
    }
    return list;
}

bool RevolifController::setDisplayedAchievement(int id) {
    if (!currentUser) return false;
    currentUser->setDisplayedAchievementID(id);
    system->rewriteUsersFile();
    emit currentUserChanged();
    return true;
}

int RevolifController::getDisplayedAchievementId() {
    return currentUser ? currentUser->getDisplayedAchievementID() : -1;
}

QVariantList RevolifController::getBudgets() {
    QVariantList list;
    return list;
}

bool RevolifController::setBudget(const QString &category, double limit) {
    return false;
}

QVariantMap RevolifController::getSpendingByCategory() {
    QVariantMap data;
    if (!currentUser) return data;

    const std::vector<Expense*> &expenses = currentUser->getExpenseManager().getExpenses();
    std::map<std::string, double> totals;
    for (Expense *e : expenses) {
        totals[e->getCategory()] += e->getAmount();
    }
    for (auto &p : totals) {
        data[QString::fromStdString(p.first)] = p.second;
    }
    return data;
}

bool RevolifController::isCurrentUserAdmin() const {
    return adminMode;
}

QVariantList RevolifController::getAllUsers() {
    QVariantList list;
    const std::vector<User*> &users = system->getUsers();
    for (User *u : users) {
        QVariantMap m;
        m["uid"] = u->getUID();
        m["username"] = QString::fromStdString(u->getUsername());
        m["name"] = QString::fromStdString(u->getName());
        m["email"] = QString::fromStdString(u->getEmail());
        m["active"] = u->getIsActive();
        m["deactivatedBySelf"] = u->getDeactivatedBySelf();
        m["streak"] = u->getCurrentStreak();
        m["bestStreak"] = u->getBestStreak();
        m["goalsCompleted"] = u->getGoalManager().countCompletedGoals();
        list.append(m);
    }
    return list;
}

bool RevolifController::suspendUser(const QString &username) {
    try {
        User *u = system->getAuth().lookupUserByUsername(username.toStdString());
        if (!u) {
            emit errorOccurred("User not found.");
            return false;
        }
        u->setActive(false);
        u->setDeactivatedBySelf(false);
        system->rewriteUsersFile();
        emit statsChanged();
        emit successMessage("User suspended.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to suspend user.");
        return false;
    }
}

bool RevolifController::permanentlyDeleteUser(const QString &username, const QString &reason) {
    try {
        User *u = system->getAuth().lookupUserByUsername(username.toStdString());
        if (!u) {
            emit errorOccurred("User not found.");
            return false;
        }
        system->permanentlyDeleteUser(u, reason.toStdString());
        emit statsChanged();
        emit successMessage("User permanently deleted.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to delete user.");
        return false;
    }
}

QVariantList RevolifController::getPermanentlyDeletedUsers() {
    QVariantList list;
    std::ifstream file("permanently_deleted_users.dat", std::ios::binary);
    if (!file) return list;

    PermanentDeletedUserRecord pdr;
    while (file.read((char*)&pdr, sizeof(pdr))) {
        QVariantMap m;
        m["uid"] = pdr.uid;
        m["username"] = QString::fromStdString(std::string(pdr.username));
        m["date"] = QString("%1/%2/%3").arg(pdr.deletion_day).arg(pdr.deletion_month).arg(pdr.deletion_year);
        m["reason"] = QString::fromStdString(std::string(pdr.reason));
        list.append(m);
    }
    file.close();
    return list;
}

QVariantMap RevolifController::getSystemStatistics() {
    QVariantMap data;
    const std::vector<User*> &users = system->getUsers();
    int active = 0, inactive = 0, suspended = 0, totalGoals = 0;
    for (User *u : users) {
        if (u->getIsActive()) active++;
        else if (u->getDeactivatedBySelf()) inactive++;
        else suspended++;
        totalGoals += u->getGoalManager().countCompletedGoals();
    }
    data["totalUsers"] = (int)users.size();
    data["activeUsers"] = active;
    data["inactiveUsers"] = inactive;
    data["suspendedUsers"] = suspended;
    data["totalGoals"] = totalGoals;
    return data;
}

bool RevolifController::generateSystemReport() {
    try {
        system->generateSystemReport();
        emit successMessage("System report generated.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to generate report.");
        return false;
    }
}

bool RevolifController::adminChangePassword(const QString &oldPass, const QString &newPass) {
    try {
        if (!system->getAuth().verifyAdminPassword(oldPass.toStdString())) {
            emit errorOccurred("Wrong current password.");
            return false;
        }
        if (newPass.length() < 6) {
            emit errorOccurred("Password must be at least 6 characters.");
            return false;
        }
        system->getAuth().changeAdminPassword();
        system->rewriteAdminFile();
        emit successMessage("Admin password changed.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to change password.");
        return false;
    }
}

bool RevolifController::addAchievement(const QString &name, const QString &description, int requiredGoals) {
    try {
        system->getAchievements().push_back(Achievement(name.toStdString(), description.toStdString(), requiredGoals, false));
        system->rewriteAchievementsFile();
        emit statsChanged();
        emit successMessage("Achievement added.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to add achievement.");
        return false;
    }
}

bool RevolifController::removeAchievement(int id) {
    try {
        Achievement *ach = system->getAchievementByID(id);
        if (!ach) {
            emit errorOccurred("Achievement not found.");
            return false;
        }
        if (ach->getIsDefault()) {
            emit errorOccurred("Cannot remove default achievements.");
            return false;
        }
        auto &achs = system->getAchievements();
        for (size_t i = 0; i < achs.size(); i++) {
            if (achs[i].getAchievementID() == id) {
                achs.erase(achs.begin() + i);
                system->rewriteAchievementsFile();
                emit statsChanged();
                emit successMessage("Achievement removed.");
                return true;
            }
        }
        return false;
    } catch (...) {
        emit errorOccurred("Failed to remove achievement.");
        return false;
    }
}

bool RevolifController::updateAchievement(int id, const QString &field, const QVariant &value) {
    try {
        Achievement *ach = system->getAchievementByID(id);
        if (!ach) {
            emit errorOccurred("Achievement not found.");
            return false;
        }
        if (ach->getIsDefault()) {
            emit errorOccurred("Cannot update default achievements.");
            return false;
        }
        std::string f = field.toStdString();
        if (f == "name") ach->setName(value.toString().toStdString());
        else if (f == "description") ach->setDescription(value.toString().toStdString());
        else if (f == "requiredGoals") ach->setRequiredGoals(value.toInt());
        system->rewriteAchievementsFile();
        emit statsChanged();
        emit successMessage("Achievement updated.");
        return true;
    } catch (...) {
        emit errorOccurred("Failed to update achievement.");
        return false;
    }
}

QString RevolifController::getErrorMessage() const {
    return m_errorMessage;
}
