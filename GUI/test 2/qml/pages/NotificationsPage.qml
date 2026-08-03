import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.paddingLarge
        spacing: Spacing.lg

        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.md

            SectionHeader {
                Layout.fillWidth: true
                title: "System Notifications"
                subtitle: "Alerts, reminders, and workspace transmissions."
            }

            SecondaryButton {
                text: "Mark All Read"
                onClicked: console.log("All notifications marked read")
            }
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.sm

                NotificationCard {
                    Layout.fillWidth: true
                    message: "C++ Backend Integration compiled successfully without warnings."
                    timestamp: "10 minutes ago"
                    unread: true
                }

                NotificationCard {
                    Layout.fillWidth: true
                    message: "Milestone 'Launch v1.0 Desktop App' is due in 14 days."
                    timestamp: "2 hours ago"
                    unread: true
                }

                NotificationCard {
                    Layout.fillWidth: true
                    message: "Daily backup to local SQLite database verified."
                    timestamp: "Yesterday"
                    unread: false
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}