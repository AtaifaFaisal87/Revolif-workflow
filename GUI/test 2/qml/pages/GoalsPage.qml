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

        // Header and Actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.md

            SectionHeader {
                Layout.fillWidth: true
                title: "Strategic Goals"
                subtitle: "Track long-term milestones and overarching life targets."
            }

            PrimaryButton {
                text: "New Goal"
                iconSource: Icons.plus
                onClicked: console.log("New goal dialog opened")
            }
        }

        // Category Filter Chips
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.xs

            Chip { text: "All Goals"; selected: true; onClicked: console.log("Filter: All") }
            Chip { text: "Career"; selected: false; onClicked: console.log("Filter: Career") }
            Chip { text: "Health"; selected: false; onClicked: console.log("Filter: Health") }
            Chip { text: "Finance"; selected: false; onClicked: console.log("Filter: Finance") }
            Chip { text: "Personal"; selected: false; onClicked: console.log("Filter: Personal") }
        }

        // Goals Grid Container
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 3
            rowSpacing: Spacing.md
            columnSpacing: Spacing.md

            GoalCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                title: "Launch v1.0 Desktop App"
                progress: 0.85
                targetDate: "Aug 15, 2026"
            }

            GoalCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                title: "Master Advanced Qt 6 & QML Architecture"
                progress: 0.60
                targetDate: "Sep 30, 2026"
            }

            GoalCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                title: "Achieve Emerald Tier Fitness Benchmark"
                progress: 0.40
                targetDate: "Nov 15, 2026"
            }

            GoalCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                title: "Automate Personal Finance Portfolios"
                progress: 0.90
                targetDate: "Aug 31, 2026"
            }
        }
    }
}