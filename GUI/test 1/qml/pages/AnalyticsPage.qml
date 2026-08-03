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
            title: "Productivity Analytics"
            subtitle: "Deep telemetry and performance trends across all operational systems."
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: Spacing.md
            columnSpacing: Spacing.md

            StatCard {
                Layout.fillWidth: true
                title: "Efficiency Ratio"
                value: "94.2%"
                subtext: "+2.8% vs last week"
                iconSource: Icons.dashboard
                accentColor: Colors.primary
            }

            StatCard {
                Layout.fillWidth: true
                title: "Deep Work Hours"
                value: "32.5 hrs"
                subtext: "Weekly aggregate"
                iconSource: Icons.focus
                accentColor: Colors.secondary
            }

            StatCard {
                Layout.fillWidth: true
                title: "Goal Completion"
                value: "88%"
                subtext: "On track"
                iconSource: Icons.goals
                accentColor: Colors.success
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
                    text: "Weekly Output Velocity"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH3
                    font.weight: Typography.weightBold
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true }
            }
        }
    }
}