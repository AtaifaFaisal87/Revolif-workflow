import QtQuick
import QtQuick.Layouts
import "theme"
import "components"

Rectangle {
    id: root

    property string currentRoute: "dashboard"
    property bool collapsed: false

    signal routeSelected(string route)

    implicitWidth: collapsed ? 80 : Spacing.sidebarWidth
    color: Colors.bgSurface
    border.color: Colors.borderSubtle
    border.width: 1

    Behavior on implicitWidth {
        NumberAnimation { duration: Animations.durationNormal; easing.type: Animations.easeInOut }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.paddingMedium
        spacing: Spacing.md

        // App Branding / Logo Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.sm

            Rectangle {
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
                radius: Spacing.radiusMd
                color: Colors.primarySubtle
                border.color: Colors.primary
                border.width: 1

                Image {
                    anchors.centerIn: parent
                    width: Spacing.iconSizeMd
                    height: Spacing.iconSizeMd
                    source: Icons.focus
                }
            }

            Text {
                visible: !root.collapsed
                Layout.fillWidth: true
                text: "REVOLIF"
                color: Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeH3
                font.weight: Typography.weightBold
                elide: Text.ElideRight
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Colors.borderSubtle
        }

        // Navigation Items
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Spacing.xs

            SidebarButton {
                Layout.fillWidth: true
                text: "Dashboard"
                iconSource: Icons.dashboard
                active: root.currentRoute === "dashboard"
                onClicked: root.routeSelected("dashboard")
            }

            SidebarButton {
                Layout.fillWidth: true
                text: "Tasks"
                iconSource: Icons.tasks
                active: root.currentRoute === "tasks"
                onClicked: root.routeSelected("tasks")
            }

            SidebarButton {
                Layout.fillWidth: true
                text: "Goals"
                iconSource: Icons.goals
                active: root.currentRoute === "goals"
                onClicked: root.routeSelected("goals")
            }

            SidebarButton {
                Layout.fillWidth: true
                text: "Focus"
                iconSource: Icons.focus
                active: root.currentRoute === "focus"
                onClicked: root.routeSelected("focus")
            }

            SidebarButton {
                Layout.fillWidth: true
                text: "Finance"
                iconSource: Icons.finance
                active: root.currentRoute === "finance"
                onClicked: root.routeSelected("finance")
            }

            SidebarButton {
                Layout.fillWidth: true
                text: "Calendar"
                iconSource: Icons.calendar
                active: root.currentRoute === "calendar"
                onClicked: root.routeSelected("calendar")
            }
        }

        Item { Layout.fillHeight: true }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Colors.borderSubtle
        }

        // User Profile Section
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.sm

            Rectangle {
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
                radius: Spacing.radiusFull
                color: Colors.bgSurfaceElevated
                border.color: Colors.borderMedium
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "RO"
                    color: Colors.primaryHover
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                    font.weight: Typography.weightBold
                }
            }

            ColumnLayout {
                visible: !root.collapsed
                Layout.fillWidth: true
                spacing: 2

                Text {
                    Layout.fillWidth: true
                    text: "Operator"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                    font.weight: Typography.weightMedium
                    elide: Text.ElideRight
                }

                Text {
                    Layout.fillWidth: true
                    text: "Emerald Tier"
                    color: Colors.textMuted
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeTiny
                    elide: Text.ElideRight
                }
            }
        }
    }
}