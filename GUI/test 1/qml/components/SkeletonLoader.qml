import QtQuick
import "../theme"

Rectangle {
    id: root

    implicitWidth: 200
    implicitHeight: 20
    radius: Spacing.radiusSm
    color: Colors.bgSurfaceElevated

    NumberAnimation on opacity {
        running: true
        from: 0.3
        to: 0.8
        duration: 800
        loops: Animation.Infinite
        easing.type: Easing.InOutSine
    }
}