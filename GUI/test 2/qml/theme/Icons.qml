pragma Singleton
import QtQuick

QtObject {
    id: root

    // Centralized Asset Icon Paths
    readonly property string dashboard: "qrc:/assets/icons/dashboard.svg"
    readonly property string tasks: "qrc:/assets/icons/tasks.svg"
    readonly property string goals: "qrc:/assets/icons/goals.svg"
    readonly property string focus: "qrc:/assets/icons/focus.svg"
    readonly property string finance: "qrc:/assets/icons/finance.svg"
    readonly property string calendar: "qrc:/assets/icons/calendar.svg"
    readonly property string settings: "qrc:/assets/icons/settings.svg"
    readonly property string bell: "qrc:/assets/icons/bell.svg"
    readonly property string search: "qrc:/assets/icons/search.svg"
    readonly property string plus: "qrc:/assets/icons/plus.svg"
    readonly property string check: "qrc:/assets/icons/check.svg"
    readonly property string trash: "qrc:/assets/icons/trash.svg"
    readonly property string play: "qrc:/assets/icons/play.svg"
    readonly property string pause: "qrc:/assets/icons/pause.svg"
}