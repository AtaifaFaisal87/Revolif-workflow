import QtQuick
import "theme"
import "components"

Item {
    id: root

    property string route: "dashboard"

    // Maps each sidebar / top-nav route to its real page implementation.
    function pageForRoute(r) {
        switch (r) {
            case "dashboard": return "pages/DashboardPage.qml";
            case "tasks": return "pages/TasksPage.qml";
            case "goals": return "pages/GoalsPage.qml";
            case "focus": return "pages/FocusPage.qml";
            case "finance": return "pages/FinancePage.qml";
            case "calendar": return "pages/CalendarPage.qml";
            case "settings": return "pages/SettingsPage.qml";
            default: return "";
        }
    }

    // Dynamic routing container - loads the real page for the active route.
    Loader {
        id: viewLoader
        anchors.fill: parent
        source: root.pageForRoute(root.route)
        asynchronous: true
    }

    // Fallback shown for unknown/unmapped routes or while nothing is loaded.
    Item {
        anchors.fill: parent
        visible: viewLoader.source == "" || viewLoader.status === Loader.Error

        EmptyState {
            anchors.centerIn: parent
            title: "Module: " + root.route.toUpperCase()
            description: "Subsystem will be loaded in the upcoming implementation phase."
            iconSource: Icons.dashboard
        }
    }
}
