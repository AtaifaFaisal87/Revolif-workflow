import QtQuick
import QtQuick.Layouts
import "../theme"

GlassCard {
    id: root

    property string title: ""
    property string category: "General"
    property bool completed: false

    signal toggled()
    signal deleted()

    implicitWidth: 320
    implicitHeight: 70
    hoverable: true

    RowLayout {
        anchors.fill: parent
        spacing: Spacing.md

        Rectangle {
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            radius: Spacing.radiusSm
            color: root.completed ? Colors.primary : Colors.transparent
            border.color: root.completed ? Colors.primary : Colors.borderBright
            border.width: 2

            Image {
                visible: root.completed
                anchors.centerIn: parent
                width: 14
                height: 14
                source: Icons.check
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.toggled()
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: root.title
                color: root.completed ? Colors.textMuted : Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeBody
                font.strikeout: root.completed
                elide: Text.ElideRight
            }

            Text {
                text: root.category
                color: Colors.textSecondary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeTiny
            }
        }

        IconButton {
            iconSource: Icons.trash
            size: 32
            iconSize: Spacing.iconSizeSm
            onClicked: root.deleted()
        }
    }
}