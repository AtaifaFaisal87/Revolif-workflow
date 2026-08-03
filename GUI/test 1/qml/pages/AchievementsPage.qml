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
            title: "Achievements & Badges"
            subtitle: "Unlocked milestones and operator proficiency accolades."
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 3
            rowSpacing: Spacing.md
            columnSpacing: Spacing.md

            AchievementCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                title: "Emerald Titan"
                description: "Maintained a 30-day continuous workflow streak."
                unlocked: true
            }

            AchievementCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                title: "Deep Focus Master"
                description: "Completed 100 hours of uninterrupted Pomodoro sessions."
                unlocked: true
            }

            AchievementCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                title: "Architect of Systems"
                description: "Deployed all 12 core REVOLIF subsystems."
                unlocked: false
            }
        }
    }
}