import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    signal verified()

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
            implicitHeight: 340
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.md

                Text {
                    text: "Verify Your Email"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH2
                    font.weight: Typography.weightBold
                }

                Text {
                    text: "We have sent a 6-digit verification code to your email address. Enter it below to activate your account."
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs
                    Text { text: "Verification Code"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; placeholderText: "000 000" }
                }

                Item { Layout.fillHeight: true }

                PrimaryButton {
                    Layout.fillWidth: true
                    text: "Verify & Continue"
                    onClicked: root.verified()
                }
            }
        }
    }
}