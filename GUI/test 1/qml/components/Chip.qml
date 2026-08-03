import QtQuick
import QtQuick.Layouts
import "../theme"

Rectangle {
    id: root

    property string text: ""
    property bool selected: false

    signal clicked()

    implicitWidth: chipRow.implicitWidth + (Spacing.paddingMedium * 2)
    implicitHeight: 32

    radius: Spacing.radiusFull
    color: selected ? Colors.primarySubtle : Colors.bgSurfaceElevated
    border.color: selected ? Colors.primary : Colors.borderMedium
    border.width: 1

    RowLayout {
        id: chipRow
        anchors.centerIn: parent
        spacing: Spacing.xs

        Text {
            text: root.text
            color: root.selected ? Colors.primaryHover : Colors.textSecondary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeSmall
            font.weight: root.selected ? Typography.weightMedium : Typography.weightNormal
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}