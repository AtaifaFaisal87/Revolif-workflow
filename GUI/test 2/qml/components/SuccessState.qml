import QtQuick
import QtQuick.Layouts
import "../theme"

ColumnLayout {
    id: root

    property string title: "Success!"
    property string message: "Operation was completed successfully."

    spacing: Spacing.md

    Rectangle {
        Layout.alignment: Qt.AlignHCenter
        width: 64
        height: 64
        radius: Spacing.radiusFull
        color: Colors.successBg

        Image {
            anchors.centerIn: parent
            width: Spacing.iconSizeLg
            height: Spacing.iconSizeLg
            source: Icons.check
        }
    }

    Text {
        Layout.alignment: Qt.AlignHCenter
        text: root.title
        color: Colors.success
        font.family: Typography.fontFamily
        font.pointSize: Typography.sizeH3
        font.weight: Typography.weightBold
    }

    Text {
        Layout.alignment: Qt.AlignHCenter
        text: root.message
        color: Colors.textSecondary
        font.family: Typography.fontFamily
        font.pointSize: Typography.sizeSmall
    }
}