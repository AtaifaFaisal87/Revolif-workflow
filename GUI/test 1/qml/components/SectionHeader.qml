import QtQuick
import QtQuick.Layouts
import "../theme"

Item {
    id: root

    property string title: ""
    property string subtitle: ""
    default property alias actions: actionContainer.data

    implicitWidth: 400
    implicitHeight: contentColumn.implicitHeight

    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        spacing: Spacing.xxs

        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.md

            Text {
                Layout.fillWidth: true
                text: root.title
                color: Colors.textPrimary
                font.family: Typography.fontFamily
                font.pointSize: Typography.sizeH2
                font.weight: Typography.weightBold
                elide: Text.ElideRight
            }

            RowLayout {
                id: actionContainer
                spacing: Spacing.sm
            }
        }

        Text {
            visible: root.subtitle !== ""
            Layout.fillWidth: true
            text: root.subtitle
            color: Colors.textSecondary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeSmall
            elide: Text.ElideRight
        }
    }
}