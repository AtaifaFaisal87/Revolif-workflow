import QtQuick
import QtQuick.Layouts
import "../theme"

ColumnLayout {
    id: root

    property string title: "No Data Found"
    property string description: "There is nothing to display right now."
    property string iconSource: Icons.search

    spacing: Spacing.md

    Image {
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 64
        Layout.preferredHeight: 64
        source: root.iconSource
        opacity: 0.4
    }

    Text {
        Layout.alignment: Qt.AlignHCenter
        text: root.title
        color: Colors.textPrimary
        font.family: Typography.fontFamily
        font.pointSize: Typography.sizeH3
        font.weight: Typography.weightBold
    }

    Text {
        Layout.alignment: Qt.AlignHCenter
        text: root.description
        color: Colors.textMuted
        font.family: Typography.fontFamily
        font.pointSize: Typography.sizeSmall
    }
}