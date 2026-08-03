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
                title: "Calendar & Schedule"
                subtitle: "Manage your daily events, meetings, and milestone deadlines."
            }

            PrimaryButton {
                text: "New Event"
                iconSource: Icons.plus
                onClicked: console.log("New event dialog opened")
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Spacing.lg

            GlassCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                hoverable: false

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Spacing.paddingMedium
                    spacing: Spacing.md

                    Text {
                        text: "August 2026"
                        color: Colors.textPrimary
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeH3
                        font.weight: Typography.weightBold
                    }

                    CalendarCard {
                        Layout.fillWidth: true
                        eventTitle: "Sprint Planning & Architecture Review"
                        timeText: "09:00 AM - 10:30 AM"
                        tagColor: Colors.primary
                    }

                    CalendarCard {
                        Layout.fillWidth: true
                        eventTitle: "QML Component Integration Sync"
                        timeText: "02:00 PM - 03:00 PM"
                        tagColor: Colors.secondary
                    }

                    CalendarCard {
                        Layout.fillWidth: true
                        eventTitle: "Weekly Financial & Budget Audit"
                        timeText: "04:30 PM - 05:15 PM"
                        tagColor: Colors.warning
                    }

                    Item { Layout.fillHeight: true }
                }
            }
        }
    }
}