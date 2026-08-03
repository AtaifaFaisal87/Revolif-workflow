import QtQuick
import "../theme"

Item {
    id: root

    property string iconSource: ""
    property real size: Spacing.buttonHeight
    property real iconSize: Spacing.iconSizeMd
    property color iconColor: Colors.textPrimary
    // Note: "enabled" is intentionally not redeclared here - it is inherited
    // from Item (default true) and reused below.

    signal clicked()

    implicitWidth: size
    implicitHeight: size

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
        border.color: mouseArea.containsMouse ? Colors.borderMedium : Colors.transparent
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
    }

    Image {
        anchors.centerIn: parent
        width: root.iconSize
        height: root.iconSize
        source: root.iconSource
        fillMode: Image.PreserveAspectFit
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