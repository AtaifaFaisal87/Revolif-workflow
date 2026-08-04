import QtQuick
import QtQuick.Controls
import "../"

Rectangle {
    id: btn

    property string text: "Button"
    property bool primary: false
    property int horizontalPadding: 20
    signal clicked()

    implicitWidth: Math.max(label.implicitWidth + horizontalPadding * 2, 96)
    implicitHeight: 48

    radius: Theme.radiusBase
    color: primary ? Theme.primary : "transparent"
    border.color: primary ? Theme.primary : Theme.border
    border.width: primary ? 0 : 1

    Label {
        id: label
        anchors.centerIn: parent
        text: btn.text
        color: btn.primary ? Theme.textOnPrimary : Theme.textPrimary
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
