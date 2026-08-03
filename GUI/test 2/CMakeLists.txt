cmake_minimum_required(VERSION 3.16)
project(REVOLIF VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2)

qt_standard_project_setup()

qt_add_executable(REVOLIF
    main.cpp
)

qt_add_qml_module(REVOLIF
    URI "REVOLIF"
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        qml/MainWindow.qml
        qml/AppShell.qml
        qml/Sidebar.qml
        qml/TopNavigation.qml
        qml/NavigationRouter.qml
        qml/PageContainer.qml
        qml/theme/Theme.qml
        qml/theme/Colors.qml
        qml/theme/Typography.qml
        qml/theme/Spacing.qml
        qml/theme/Animations.qml
        qml/theme/Icons.qml
        qml/components/GlassCard.qml
        qml/components/PrimaryButton.qml
        qml/components/SecondaryButton.qml
        qml/components/IconButton.qml
        qml/components/SidebarButton.qml
        qml/components/SearchBar.qml
        qml/components/SectionHeader.qml
        qml/components/Badge.qml
        qml/components/Chip.qml
        qml/components/ProgressRing.qml
        qml/components/CircularProgress.qml
        qml/components/StatCard.qml
        qml/components/TaskCard.qml
        qml/components/GoalCard.qml
        qml/components/FinanceCard.qml
        qml/components/CalendarCard.qml
        qml/components/TimelineCard.qml
        qml/components/AchievementCard.qml
        qml/components/NotificationCard.qml
        qml/components/Toast.qml
        qml/components/Tooltip.qml
        qml/components/LoadingOverlay.qml
        qml/components/SkeletonLoader.qml
        qml/components/EmptyState.qml
        qml/components/ErrorState.qml
        qml/components/SuccessState.qml
        qml/pages/DashboardPage.qml
        qml/pages/TasksPage.qml
        qml/pages/GoalsPage.qml
        qml/pages/CalendarPage.qml
        qml/pages/FocusPage.qml
        qml/pages/StudyPage.qml
        qml/pages/FinancePage.qml
        qml/pages/AnalyticsPage.qml
        qml/pages/ActivityPage.qml
        qml/pages/AchievementsPage.qml
        qml/pages/ReportsPage.qml
        qml/pages/NotificationsPage.qml
        qml/pages/ProfilePage.qml
        qml/pages/SettingsPage.qml
        qml/pages/AdminPage.qml
        qml/pages/LandingPage.qml
        qml/pages/LoginPage.qml
        qml/pages/RegisterPage.qml
        qml/pages/ForgotPasswordPage.qml
        qml/pages/ResetPasswordPage.qml
        qml/pages/EmailVerificationPage.qml
        qml/pages/WelcomePage.qml
        modules/dashboard/DashboardView.qml
        modules/tasks/TasksView.qml
        modules/goals/GoalsView.qml
        modules/focus/FocusView.qml
        modules/finance/FinanceView.qml
        modules/calendar/CalendarView.qml
        modules/settings/SettingsView.qml
    RESOURCES
        resources.qrc
)

target_link_libraries(REVOLIF PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
)