import QtQuick
import QtQuick.Layouts
import "../theme"

GlassCard {
    id: root

    property string title: ""
    property string value: "0"
    property string subtext: ""
    property string iconSource: ""
    property color accentColor: Colors.primary

    implicitWidth: 220
    implicitHeight: 120

    ColumnLayout {
        anchors.fill: parent
        spacing: Spacing.xs

        RowLayout {
            Layout.fillWidth: true

            Text {
                Layout.fillWidth: true
                text: root.title
                color: Colors.textSecondary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeSmall
                elide: Text.ElideRight
            }

            Rectangle {
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
                radius: Spacing.radiusSm
                color: Qt.rgba(root.accentColor.r, root.accentColor.g, root.accentColor.b, 0.15)

                Image {
                    anchors.centerIn: parent
                    width: Spacing.iconSizeSm
                    height: Spacing.iconSizeSm
                    source: root.iconSource
                    fillMode: Image.PreserveAspectFit
                }
            }
        }

        Text {
            text: root.value
            color: Colors.textPrimary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeDisplay
            font.weight: Typography.weightBold
        }

        Text {
            visible: root.subtext !== ""
            text: root.subtext
            color: root.accentColor
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeTiny
        }
    }
}