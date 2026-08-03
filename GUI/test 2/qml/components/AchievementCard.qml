import QtQuick
import QtQuick.Layouts
import "../theme"

GlassCard {
    id: root

    property string title: ""
    property string description: ""
    property bool unlocked: false

    implicitWidth: 220
    implicitHeight: 140

    ColumnLayout {
        anchors.fill: parent
        spacing: Spacing.xs

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            radius: Spacing.radiusFull
            color: root.unlocked ? Colors.primarySubtle : Colors.bgSurfaceElevated

            Image {
                anchors.centerIn: parent
                width: Spacing.iconSizeLg
                height: Spacing.iconSizeLg
                source: Icons.goals
                opacity: root.unlocked ? 1.0 : 0.3
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: root.title
            color: root.unlocked ? Colors.textPrimary : Colors.textMuted
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeBody
            font.weight: Typography.weightBold
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: root.description
            color: Colors.textSecondary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeTiny
            wrapMode: Text.WordWrap
        }
    }
}