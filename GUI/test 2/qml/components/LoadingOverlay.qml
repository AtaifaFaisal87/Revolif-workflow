import QtQuick
import QtQuick.Layouts
import "../theme"

Rectangle {
    id: root

    property string message: "Loading..."

    anchors.fill: parent
    color: Colors.overlayModal
    visible: false

    MouseArea {
        anchors.fill: parent
        preventStealing: true
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Spacing.md

        Image {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: Spacing.iconSizeXl
            Layout.preferredHeight: Spacing.iconSizeXl
            source: Icons.focus

            RotationAnimation on rotation {
                running: root.visible
                from: 0
                to: 360
                loops: Animation.Infinite
                duration: 1000
            }
        }

        Text {
            text: root.message
            color: Colors.textPrimary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeBody
        }
    }
}