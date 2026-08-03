import QtQuick
import "../theme"

Rectangle {
    id: root

    property string text: ""

    implicitWidth: tooltipText.implicitWidth + (Spacing.sm * 2)
    implicitHeight: 26
    radius: Spacing.radiusSm
    color: Colors.bgSurfaceElevated
    border.color: Colors.borderMedium
    border.width: 1

    Text {
        id: tooltipText
        anchors.centerIn: parent
        text: root.text
        color: Colors.textPrimary
        font.family: Typography.fontFamily
        font.pointSize: Typography.sizeTiny
    }
}