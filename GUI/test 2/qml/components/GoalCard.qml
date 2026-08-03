import QtQuick
import QtQuick.Layouts
import "../theme"

GlassCard {
    id: root

    property string title: ""
    property real progress: 0.0 // 0.0 to 1.0
    property string targetDate: ""

    implicitWidth: 280
    implicitHeight: 140
    hoverable: true

    ColumnLayout {
        anchors.fill: parent
        spacing: Spacing.sm

        RowLayout {
            Layout.fillWidth: true

            Text {
                Layout.fillWidth: true
                text: root.title
                color: Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeH3
                font.weight: Typography.weightBold
                elide: Text.ElideRight
            }

            Badge {
                text: Math.round(root.progress * 100) + "%"
                badgeColor: Colors.primarySubtle
                textColor: Colors.primaryHover
            }
        }

        Text {
            text: "Target: " + root.targetDate
            color: Colors.textMuted
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeSmall
        }

        Item { Layout.fillHeight: true }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 6
            radius: Spacing.radiusFull
            color: Colors.bgSurfaceElevated

            Rectangle {
                width: parent.width * Math.max(0, Math.min(1, root.progress))
                height: parent.height
                radius: Spacing.radiusFull
                color: Colors.primary
            }
        }
    }
}