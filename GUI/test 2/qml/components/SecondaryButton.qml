import QtQuick
import QtQuick.Layouts
import "../theme"

Item {
    id: root

    property string text: ""
    property string iconSource: ""
    // Note: "enabled" is intentionally not redeclared here - it is inherited
    // from Item (default true) and reused below.

    signal clicked()

    implicitWidth: Math.max(120, contentRow.implicitWidth + (Spacing.paddingLarge * 2))
    implicitHeight: Spacing.buttonHeight

    opacity: root.enabled ? 1.0 : 0.5

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: Spacing.radiusMd
        color: {
            if (!root.enabled) return Colors.transparent
            if (mouseArea.pressed) return Colors.bgSurfaceHover
            if (mouseArea.containsMouse) return Colors.bgSurfaceElevated
            return Colors.transparent
        }
        border.color: mouseArea.containsMouse ? Colors.borderBright : Colors.borderMedium
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
        Behavior on border.color {
            ColorAnimation { duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
    }

    RowLayout {
        id: contentRow
        anchors.centerIn: parent
        spacing: Spacing.sm

        Image {
            id: buttonIcon
            visible: root.iconSource !== ""
            Layout.preferredWidth: Spacing.iconSizeSm
            Layout.preferredHeight: Spacing.iconSizeSm
            source: root.iconSource
            fillMode: Image.PreserveAspectFit
        }

        Text {
            id: buttonText
            text: root.text
            color: Colors.textPrimary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeBody
            font.weight: Typography.weightMedium
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.enabled
        hoverEnabled: true
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
        onClicked: root.clicked()
    }
}