import QtQuick
import QtQuick.Controls
import "../"

Rectangle {
    id: btn
    height: 48
    radius: Theme.radiusBase
    color: primary ? Theme.primary : "transparent"
    border.color: primary ? Theme.primary : Theme.border
    border.width: primary ? 0 : 1

    property string text: "Button"
    property bool primary: false
    signal clicked()

    Label {
        anchors.centerIn: parent
        text: btn.text
        color: primary ? Theme.textOnPrimary : Theme.textPrimary
        font.bold: true
        font.pixelSize: Theme.fontSizeBase
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onPressed: btn.scale = 0.97
        onReleased: btn.scale = 1.0
        onClicked: btn.clicked()
    }

    Behavior on scale {
        NumberAnimation { duration: 100 }
    }
}
