import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../theme"

Item {
    id: root

    property string placeholderText: "Search..."
    property alias text: textInput.text

    signal accepted(string query)

    implicitWidth: 280
    implicitHeight: Spacing.inputHeight

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: Spacing.radiusMd
        color: Colors.bgSurface
        border.color: textInput.activeFocus ? Colors.primary : Colors.borderMedium
        border.width: 1

        Behavior on border.color {
            ColorAnimation { duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Spacing.paddingSmall
        anchors.rightMargin: Spacing.paddingSmall
        spacing: Spacing.sm

        Image {
            Layout.preferredWidth: Spacing.iconSizeSm
            Layout.preferredHeight: Spacing.iconSizeSm
            source: Icons.search
            fillMode: Image.PreserveAspectFit
        }

        TextInput {
            id: textInput
            Layout.fillWidth: true
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeBody
            color: Colors.textPrimary
            clip: true

            Text {
                text: root.placeholderText
                color: Colors.textMuted
                font: parent.font
                visible: !parent.text && !parent.activeFocus
            }

            onAccepted: root.accepted(textInput.text)
        }
    }
}