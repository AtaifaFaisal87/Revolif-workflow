import QtQuick
import QtQuick.Layouts
import "../theme"

Rectangle {
    id: root

    // Public API
    default property alias content: contentContainer.data
    property real cardCornerRadius: Spacing.radiusLg
    property color borderBaseColor: Colors.borderSubtle
    property color borderHoverColor: Colors.borderBright
    property color backgroundBaseColor: Colors.bgSurface
    property color backgroundHoverColor: Colors.bgSurfaceElevated
    property bool hoverable: false
    property bool interactive: false

    // Signals
    signal clicked()

    // Dimensions & Styling Defaults
    implicitWidth: 300
    implicitHeight: 180
    radius: cardCornerRadius
    color: (hoverable && mouseArea.containsMouse) ? backgroundHoverColor : backgroundBaseColor
    border.color: (hoverable && mouseArea.containsMouse) ? borderHoverColor : borderBaseColor
    border.width: 1

    // Smooth Color Transitions
    Behavior on color {
        ColorAnimation {
            duration: Animations.durationFast
            easing.type: Animations.easeInOut
        }
    }

    Behavior on border.color {
        ColorAnimation {
            duration: Animations.durationFast
            easing.type: Animations.easeInOut
        }
    }

    // Inner Content Container with Padding Support
    Item {
        id: contentContainer
        anchors.fill: parent
        anchors.margins: Spacing.paddingMedium
    }

    // Interaction Handling
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.hoverable || root.interactive
        hoverEnabled: root.hoverable
        cursorShape: root.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor

        onClicked: {
            if (root.interactive) {
                root.clicked()
            }
        }
    }
}