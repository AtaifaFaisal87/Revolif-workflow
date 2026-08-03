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
            title: "Focus & Pomodoro Subsystem"
            subtitle: "Optimize deep work sessions and eliminate cognitive friction."
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.centerIn: parent
                spacing: Spacing.lg

                Badge {
                    Layout.alignment: Qt.AlignHCenter
                    text: "DEEP WORK MODE"
                    badgeColor: Colors.primarySubtle
                    textColor: Colors.primaryHover
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "25:00"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: 64
                    font.weight: Typography.weightBold
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Session 3 of 4 • Target: 2 Hours Daily"
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeBody
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: Spacing.md

                    PrimaryButton {
                        text: "Start Session"
                        iconSource: Icons.play
                        onClicked: console.log("Focus started")
                    }

                    SecondaryButton {
                        text: "Reset Timer"
                        onClicked: console.log("Focus reset")
                    }
                }
            }
        }
    }
}