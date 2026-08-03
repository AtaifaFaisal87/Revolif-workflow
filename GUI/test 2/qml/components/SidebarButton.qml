import QtQuick
import QtQuick.Layouts
import "../theme"

Item {
    id: root

    property string text: ""
    property string iconSource: ""
    property bool active: false

    signal clicked()

    implicitWidth: 220
    implicitHeight: 44

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: Spacing.radiusMd
        color: {
            if (root.active) return Colors.primarySubtle
            if (mouseArea.containsMouse) return Colors.bgSurfaceElevated
            return Colors.transparent
        }
        border.color: root.active ? Colors.primary : Colors.transparent
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
    }

    Rectangle {
        id: activeIndicator
        visible: root.active
        width: 4
        height: parent.height - (Spacing.sm * 2)
        anchors.left: parent.left
        anchors.leftMargin: Spacing.xs
        anchors.verticalCenter: parent.verticalCenter
        radius: Spacing.radiusSm
        color: Colors.primary
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Spacing.paddingMedium
        anchors.rightMargin: Spacing.paddingMedium
        spacing: Spacing.md

        Image {
            Layout.preferredWidth: Spacing.iconSizeMd
            Layout.preferredHeight: Spacing.iconSizeMd
            source: root.iconSource
            fillMode: Image.PreserveAspectFit
        }

        Text {
            Layout.fillWidth: true
            text: root.text
            color: root.active ? Colors.primaryHover : Colors.textSecondary
            font.family: Typography.fontFamily
            font.pointSize: Typography.sizeBody
            font.weight: root.active ? Typography.weightSemiBold : Typography.weightNormal
            elide: Text.ElideRight
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}