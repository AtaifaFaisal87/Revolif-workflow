import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    signal passwordResetComplete()

    Rectangle {
        anchors.fill: parent
        color: Colors.bgBase
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Spacing.lg
        width: Math.min(440, parent.width - 40)

        GlassCard {
            Layout.fillWidth: true
            implicitHeight: 380
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.md

                Text {
                    text: "Set New Password"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH2
                    font.weight: Typography.weightBold
                }

                Text {
                    text: "Please enter a secure new password for your REVOLIF account."
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs
                    Text { text: "New Password"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; placeholderText: "••••••••••••" }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs
                    Text { text: "Confirm Password"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; placeholderText: "••••••••••••" }
                }

                Item { Layout.fillHeight: true }

                PrimaryButton {
                    Layout.fillWidth: true
                    text: "Update Password"
                    onClicked: root.passwordResetComplete()
                }
            }
        }
    }
}