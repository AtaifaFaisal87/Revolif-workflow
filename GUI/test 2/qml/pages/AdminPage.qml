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
            title: "Admin & Diagnostics"
            subtitle: "System health monitoring, database integrity checks, and logs."
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: Spacing.md
            columnSpacing: Spacing.md

            StatCard {
                Layout.fillWidth: true
                title: "Database State"
                value: "Healthy"
                subtext: "SQLite 3.42"
                iconSource: Icons.tasks
                accentColor: Colors.success
            }

            StatCard {
                Layout.fillWidth: true
                title: "Memory Usage"
                value: "42.8 MB"
                subtext: "Optimal"
                iconSource: Icons.focus
                accentColor: Colors.primary
            }

            StatCard {
                Layout.fillWidth: true
                title: "Active Threads"
                value: "8 Cores"
                subtext: "Hardware concurrent"
                iconSource: Icons.dashboard
                accentColor: Colors.secondary
            }
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.md

                Text {
                    text: "System Diagnostic Actions"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH3
                    font.weight: Typography.weightBold
                }

                RowLayout {
                    spacing: Spacing.md
                    PrimaryButton { text: "Verify Database Integrity"; onClicked: console.log("DB check") }
                    SecondaryButton { text: "Purge Cache"; onClicked: console.log("Cache purged") }
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}