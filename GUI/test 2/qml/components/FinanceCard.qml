import QtQuick
import QtQuick.Layouts
import "../theme"

GlassCard {
    id: root

    property string title: ""
    property string amount: "$0.00"
    property bool isIncome: true

    implicitWidth: 240
    implicitHeight: 90

    RowLayout {
        anchors.fill: parent
        spacing: Spacing.md

        Rectangle {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            radius: Spacing.radiusMd
            color: root.isIncome ? Colors.successBg : Colors.errorBg

            Image {
                anchors.centerIn: parent
                width: Spacing.iconSizeMd
                height: Spacing.iconSizeMd
                source: Icons.finance
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: root.title
                color: Colors.textSecondary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeSmall
            }

            Text {
                text: (root.isIncome ? "+ " : "- ") + root.amount
                color: root.isIncome ? Colors.success : Colors.error
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeH3
                font.weight: Typography.weightBold
            }
        }
    }
}