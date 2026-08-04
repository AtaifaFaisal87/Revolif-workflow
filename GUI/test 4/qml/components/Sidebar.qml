import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"

Rectangle {
    id: sidebar
    color: Theme.sidebarBg

    signal navigate(string page)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            Image {
                source: "qrc:/assets/logo.jpeg"
                fillMode: Image.PreserveAspectFit
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
            }
            Label {
                text: "REVOLIF"
                font.pixelSize: 18
                font.bold: true
                color: Theme.primary
                font.letterSpacing: 2
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.border
            Layout.topMargin: 8
            Layout.bottomMargin: 8
        }

        ColumnLayout {
            id: navColumn
            Layout.fillWidth: true
            spacing: 4

            Repeater {
                model: [
                    { key: "dashboard", label: "Dashboard", icon: "▤" },
                    { key: "tasks", label: "Tasks", icon: "☑" },
                    { key: "goals", label: "Goals", icon: "◉" },
                    { key: "expenses", label: "Finance", icon: "$" },
                    { key: "achievements", label: "Achievements", icon: "★" },
                    { key: "profile", label: "Profile", icon: "◎" },
                    { key: "settings", label: "Settings", icon: "⚙" },
                    { key: "admin", label: "Admin", icon: "⚡", adminOnly: true }
                ]

                delegate: Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    radius: Theme.radiusBase
                    color: sidebar.currentPage === modelData.key ? Theme.selectedBg : "transparent"
                    visible: !modelData.adminOnly || revolif.isAdmin

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 12
                        Label {
                            text: modelData.icon
                            font.pixelSize: 16
                            color: sidebar.currentPage === modelData.key ? Theme.primary : Theme.textSecondary
                        }
                        Label {
                            text: modelData.label
                            font.pixelSize: Theme.fontSizeBase
                            color: sidebar.currentPage === modelData.key ? Theme.primary : Theme.textPrimary
                            font.bold: sidebar.currentPage === modelData.key
                            Layout.fillWidth: true
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onEntered: if (sidebar.currentPage !== modelData.key) parent.color = Theme.hoverBg
                        onExited: if (sidebar.currentPage !== modelData.key) parent.color = "transparent"
                        onClicked: {
                            sidebar.currentPage = modelData.key
                            sidebar.navigate(modelData.key)
                        }
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            radius: Theme.radiusBase
            color: Theme.cardBg
            border.color: Theme.border
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12
                Rectangle {
                    Layout.preferredWidth: 36
                    Layout.preferredHeight: 36
                    radius: 18
                    color: Theme.accent
                    Label {
                        anchors.centerIn: parent
                        text: revolif.currentUserName.charAt(0).toUpperCase()
                        color: "white"
                        font.bold: true
                        font.pixelSize: 14
                    }
                }
                ColumnLayout {
                    spacing: 2
                    Layout.fillWidth: true
                    Label {
                        text: revolif.currentUserName
                        font.pixelSize: Theme.fontSizeSm
                        font.bold: true
                        color: Theme.textPrimary
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                    Label {
                        text: revolif.currentUserTitle || "Member"
                        font.pixelSize: 11
                        color: Theme.textMuted
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    radius: 16
                    color: logoutMouse.containsMouse ? Theme.hoverBg : "transparent"

                    Label {
                        anchors.centerIn: parent
                        text: "⏻"
                        font.pixelSize: 16
                        color: Theme.danger
                    }

                    MouseArea {
                        id: logoutMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: revolif.logout()

                        ToolTip.visible: containsMouse
                        ToolTip.text: "Logout"
                    }
                }
            }
        }
    }

    property string currentPage: "dashboard"
}
