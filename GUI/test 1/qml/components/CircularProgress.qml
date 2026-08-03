import QtQuick
import QtQuick.Layouts
import "../theme"

Item {
    id: root

    property real value: 0.75 // 0.0 to 1.0
    property string title: ""
    property string label: Math.round(value * 100) + "%"

    implicitWidth: 140
    implicitHeight: 140

    ProgressRing {
        anchors.fill: parent
        value: root.value
        strokeWidth: 10
        progressColor: Colors.primary
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 0

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: root.label
            color: Colors.textPrimary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeH2
            font.weight: Typography.weightBold
        }

        Text {
            visible: root.title !== ""
            Layout.alignment: Qt.AlignHCenter
            text: root.title
            color: Colors.textMuted
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeTiny
        }
    }
}