import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    signal getStartedClicked()
    signal loginClicked()

    Rectangle {
        anchors.fill: parent
        color: Colors.bgBase

        // Animated ambient background circles
        Rectangle {
            width: 500; height: 500
            radius: 250
            color: Colors.primarySubtle
            x: 100; y: -100
            opacity: 0.3

            NumberAnimation on y {
                from: -100; to: -50; duration: 6000; loops: Animation.Infinite; easing.type: Easing.InOutSine
            }
        }

        Rectangle {
            width: 400; height: 400
            radius: 200
            color: Colors.secondary
            x: parent.width - 300; y: parent.height - 300
            opacity: 0.15

            NumberAnimation on x {
                from: parent.width - 300; to: parent.width - 350; duration: 5000; loops: Animation.Infinite; easing.type: Easing.InOutSine
            }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Spacing.xl
        width: Math.min(600, parent.width - 80)

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Spacing.md

            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 64; height: 64
                radius: Spacing.radiusXl
                color: Colors.primarySubtle
                border.color: Colors.primary
                border.width: 1

                Image {
                    anchors.centerIn: parent
                    width: Spacing.iconSizeLg
                    height: Spacing.iconSizeLg
                    source: Icons.focus
                }
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "REVOLIF"
                color: Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeDisplay
                font.weight: Typography.weightBold
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: "The Emerald Odyssey Operating System for Ultimate Life Mastery."
                color: Colors.textSecondary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeH3
                wrapMode: Text.WordWrap
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Spacing.md

            PrimaryButton {
                text: "Get Started"
                onClicked: root.getStartedClicked()
            }

            SecondaryButton {
                text: "Sign In"
                onClicked: root.loginClicked()
            }
        }
    }
}