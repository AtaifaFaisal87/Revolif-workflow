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

        SectionHeader {
            title: "System Activity Log"
            subtitle: "Chronological event streams and operational history."
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.sm

                TimelineCard {
                    Layout.fillWidth: true
                    title: "Component Build Success"
                    timestamp: "Today, 11:30 AM"
                    description: "Compiled all QML pages without warning or error."
                }

                TimelineCard {
                    Layout.fillWidth: true
                    title: "Milestone Reached"
                    timestamp: "Today, 09:15 AM"
                    description: "Completed Tasks & Goals module integration."
                }

                TimelineCard {
                    Layout.fillWidth: true
                    title: "Database Synchronization"
                    timestamp: "Yesterday, 11:59 PM"
                    description: "Local SQLite backup successfully verified."
                    isLast: true
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}