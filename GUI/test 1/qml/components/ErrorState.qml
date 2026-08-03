import QtQuick
import QtQuick.Layouts
import "../theme"

ColumnLayout {
    id: root

    property string title: "Something went wrong"
    property string message: "An unhandled error occurred. Please try again."

    signal retry()

    spacing: Spacing.md

    Rectangle {
        Layout.alignment: Qt.AlignHCenter
        width: 64
        height: 64
        radius: Spacing.radiusFull
        color: Colors.errorBg

        Image {
            anchors.centerIn: parent
            width: Spacing.iconSizeLg
            height: Spacing.iconSizeLg
            source: Icons.trash
        }
    }

    Text {
        Layout.alignment: Qt.AlignHCenter
        text: root.title
        color: Colors.error
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

    PrimaryButton {
        Layout.alignment: Qt.AlignHCenter
        text: "Retry"
        onClicked: root.retry()
    }
}