import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../theme"

Item {
    id: root

    // Public API
    property string text: ""
    property string iconSource: ""
    // Note: "enabled" is intentionally not redeclared here - it is inherited
    // from Item (default true) and reused below.
    property bool loading: false

    // Signals
    signal clicked()

    // Dimensions
    implicitWidth: Math.max(120, contentRow.implicitWidth + (Spacing.paddingLarge * 2))
    implicitHeight: Spacing.buttonHeight

    opacity: root.enabled ? 1.0 : 0.5

    // Visual Background
    Rectangle {
        id: bg
        anchors.fill: parent
        radius: Spacing.radiusMd
        color: {
            if (!root.enabled) return Colors.bgSurfaceElevated
            if (mouseArea.pressed) return Colors.primaryActive
            if (mouseArea.containsMouse) return Colors.primaryHover
            return Colors.primary
        }

        Behavior on color {
            ColorAnimation {
                duration: Animations.durationFast
                easing.type: Animations.easeInOut
            }
        }

        // Inner Border Highlight
        border.color: mouseArea.containsMouse ? Colors.secondaryHover : Colors.transparent
        border.width: 1
    }

    // Content Layout
    RowLayout {
        id: contentRow
        anchors.centerIn: parent
        spacing: Spacing.sm

        // Loading Indicator (Rotates when loading)
        Image {
            id: loadingIcon
            visible: root.loading
            Layout.preferredWidth: Spacing.iconSizeSm
            Layout.preferredHeight: Spacing.iconSizeSm
            source: Icons.focus
            fillMode: Image.PreserveAspectFit

            RotationAnimation on rotation {
                running: root.loading
                from: 0
                to: 360
                loops: Animation.Infinite
                duration: 1000
            }
        }

        // Button Icon
        Image {
            id: buttonIcon
            visible: !root.loading && root.iconSource !== ""
            Layout.preferredWidth: Spacing.iconSizeSm
            Layout.preferredHeight: Spacing.iconSizeSm
            source: root.iconSource
            fillMode: Image.PreserveAspectFit
        }

        // Button Text
        Text {
            id: buttonText
            text: root.text
            color: Colors.textOnPrimary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeBody
            font.weight: Typography.weightMedium
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    // Interaction Handling
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.enabled && !root.loading
        hoverEnabled: true
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor

        onClicked: {
            root.clicked()
        }
    }
}