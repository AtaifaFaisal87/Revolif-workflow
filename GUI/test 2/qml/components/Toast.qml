import QtQuick
import QtQuick.Layouts
import "../theme"

Rectangle {
    id: root

    property string message: ""
    property color toastColor: Colors.primary

    implicitWidth: toastRow.implicitWidth + (Spacing.paddingMedium * 2)
    implicitHeight: 44
    radius: Spacing.radiusMd
    color: Colors.bgSurfaceElevated
    border.color: root.toastColor
    border.width: 1

    RowLayout {
        id: toastRow
        anchors.centerIn: parent
        spacing: Spacing.sm

        Rectangle {
            width: 8
            height: 8
            radius: Spacing.radiusFull
            color: root.toastColor
        }

        Text {
            text: root.message
            color: Colors.textPrimary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeBody
        }
    }
}