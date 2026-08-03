import QtQuick
import QtQuick.Layouts
import "../theme"

Item {
    id: root

    property string title: ""
    property string timestamp: ""
    property string description: ""
    property bool isLast: false

    implicitWidth: 300
    implicitHeight: contentColumn.implicitHeight + Spacing.md

    RowLayout {
        anchors.fill: parent
        spacing: Spacing.md

        Column {
            Layout.fillHeight: true
            Layout.preferredWidth: 16

            Rectangle {
                width: 12
                height: 12
                radius: Spacing.radiusFull
                color: Colors.primary
            }

            Rectangle {
                visible: !root.isLast
                width: 2
                height: parent.height - 12
                color: Colors.borderMedium
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        ColumnLayout {
            id: contentColumn
            Layout.fillWidth: true
            spacing: Spacing.xxs

            Text {
                text: root.title
                color: Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeBody
                font.weight: Typography.weightBold
            }

            Text {
                text: root.timestamp
                color: Colors.textMuted
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeTiny
            }

            Text {
                text: root.description
                color: Colors.textSecondary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeSmall
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
    }
}