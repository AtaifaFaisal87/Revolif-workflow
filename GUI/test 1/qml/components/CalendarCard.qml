import QtQuick
import QtQuick.Layouts
import "../theme"

GlassCard {
    id: root

    property string eventTitle: ""
    property string timeText: ""
    property color tagColor: Colors.secondary

    implicitWidth: 260
    implicitHeight: 70
    hoverable: true

    RowLayout {
        anchors.fill: parent
        spacing: Spacing.md

        Rectangle {
            Layout.preferredWidth: 4
            Layout.fillHeight: true
            radius: Spacing.radiusSm
            color: root.tagColor
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: root.eventTitle
                color: Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeBody
                font.weight: Typography.weightMedium
                elide: Text.ElideRight
            }

            Text {
                text: root.timeText
                color: Colors.textSecondary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeSmall
            }
        }
    }
}