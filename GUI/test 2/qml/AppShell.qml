import QtQuick
import QtQuick.Layouts
import "theme"
import "components"

Item {
    id: root

    property string currentRoute: "dashboard"

    Rectangle {
        anchors.fill: parent
        color: Colors.bgBase
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Sidebar {
            id: sidebar
            Layout.fillHeight: true
            currentRoute: root.currentRoute
            onRouteSelected: function(route) {
                root.currentRoute = route
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            TopNavigation {
                id: topNav
                Layout.fillWidth: true
                Layout.preferredHeight: Spacing.headerHeight
                onSearchAccepted: function(query) {
                    console.log("Search executed:", query)
                }
                onNotificationClicked: {
                    console.log("Notification clicked")
                }
                onSettingsClicked: {
                    root.currentRoute = "settings"
                }
            }

            PageContainer {
                id: pageContainer
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentRoute: root.currentRoute
            }
        }
    }
}