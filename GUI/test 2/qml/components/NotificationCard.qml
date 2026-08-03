import QtQuick
import QtQuick.Layouts
import "../theme"

GlassCard {
    id: root

    property string message: ""
    property string timestamp: ""
    property bool unread: true

    implicitWidth: 300
    implicitHeight: 70
    hoverable: true

    RowLayout {
        anchors.fill: parent
        spacing: Spacing.md

        Rectangle {
            Layout.preferredWidth: 8
            Layout.preferredHeight: 8
            radius: Spacing.radiusFull
            color: root.unread ? Colors.primary : Colors.transparent
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: root.message
                color: Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeSmall
                elide: Text.ElideRight
            }

            Text {
                text: root.timestamp
                color: Colors.textMuted
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeTiny
            }
        }
    }
}