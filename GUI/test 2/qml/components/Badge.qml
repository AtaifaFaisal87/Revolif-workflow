import QtQuick
import "../theme"

Rectangle {
    id: root

    property string text: ""
    property color badgeColor: Colors.primary
    property color textColor: Colors.textOnPrimary

    implicitWidth: badgeText.implicitWidth + (Spacing.sm * 2)
    implicitHeight: 22

    radius: Spacing.radiusFull
    color: badgeColor

    Text {
        id: badgeText
        anchors.centerIn: parent
        text: root.text
        color: root.textColor
        font.family: Typography.fontFamily
        font.pointSize: Typography.sizeTiny
        font.weight: Typography.weightBold
    }
}