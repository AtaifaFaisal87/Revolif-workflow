import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"

Rectangle {
    id: topBar
    color: Theme.bg

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Label {
            text: revolif.pageTitle
            font.pixelSize: Theme.fontSizeXl
            font.bold: true
            color: Theme.textPrimary
        }

        Item { Layout.fillWidth: true }

        Rectangle {
            Layout.preferredWidth: 320
            Layout.preferredHeight: 44
            radius: Theme.radiusFull
            color: Theme.cardBg
            border.color: Theme.border
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 8
                Label {
                    text: "⌕"
                    font.pixelSize: 18
                    color: Theme.textMuted
                }
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: "Search anything..."
                    background: null
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeBase
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44
            radius: 22
            color: Theme.cardBg
            border.color: Theme.border
            border.width: 1
            Label {
                anchors.centerIn: parent
                text: "🔔"
                font.pixelSize: 16
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
            }
            Rectangle {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 8
                width: 8
                height: 8
                radius: 4
                color: Theme.warning
                visible: revolif.overdueTasks > 0
            }
        }

        Rectangle {
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44
            radius: 22
            color: Theme.primary
            Label {
                anchors.centerIn: parent
                text: revolif.currentUserName.charAt(0).toUpperCase()
                color: "white"
                font.bold: true
                font.pixelSize: 16
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    sidebar.currentPage = "profile"
                    sidebar.navigate("profile")
                }
            }
        }
    }
}
