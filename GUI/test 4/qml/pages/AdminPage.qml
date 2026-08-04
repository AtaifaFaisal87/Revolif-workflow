import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../dialogs"
import "../"

Rectangle {
    id: adminPage
    color: Theme.bg
    visible: revolif.isAdmin

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: "Admin Panel"
            font.pixelSize: Theme.fontSizeXl
            font.bold: true
            color: Theme.textPrimary
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Repeater {
                id: statsRepeater
                model: []

                function refresh() {
                    var s = revolif.getSystemStatistics();
                    statsRepeater.model = [
                        { label: "Total Users", value: s.totalUsers || 0 },
                        { label: "Active", value: s.activeUsers || 0 },
                        { label: "Inactive", value: s.inactiveUsers || 0 },
                        { label: "Suspended", value: s.suspendedUsers || 0 }
                    ];
                }

                Component.onCompleted: refresh()

                Connections {
                    target: revolif
                    function onStatsChanged() { statsRepeater.refresh() }
                }

                delegate: Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    radius: Theme.radiusLg
                    color: Theme.cardBg
                    border.color: Theme.border
                    border.width: 1

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 4

                        Label {
                            text: modelData.value
                            font.bold: true
                            font.pixelSize: 28
                            color: Theme.primary
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Label {
                            text: modelData.label
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
        }

        Label {
            text: "Users"
            font.bold: true
            font.pixelSize: Theme.fontSizeLg
            color: Theme.textPrimary
        }

        ListView {
            id: userListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            model: []

            function refresh() { userListView.model = revolif.getAllUsers() }

            Component.onCompleted: refresh()

            Connections {
                target: revolif
                function onStatsChanged() { userListView.refresh() }
            }

            delegate: Rectangle {
                width: ListView.view.width
                height: 60
                radius: Theme.radiusBase
                color: Theme.cardBg
                border.color: Theme.border
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 16

                    Label {
                        text: modelData.username
                        font.bold: true
                        color: Theme.textPrimary
                        Layout.preferredWidth: 120
                    }

                    Label {
                        text: modelData.name
                        color: Theme.textSecondary
                        Layout.preferredWidth: 150
                    }

                    Label {
                        text: modelData.active ? "Active" : "Inactive"
                        color: modelData.active ? Theme.success : Theme.danger
                        font.bold: true
                        Layout.preferredWidth: 80
                    }

                    Item { Layout.fillWidth: true }

                    AnimatedButton {
                        text: "Suspend"
                        primary: false
                        visible: modelData.active
                        onClicked: revolif.suspendUser(modelData.username)
                    }

                    AnimatedButton {
                        text: "Delete"
                        primary: false
                        onClicked: {
                            deleteConfirm.username = modelData.username
                            deleteConfirm.message = "Permanently delete user \"" + modelData.username + "\"? This cannot be undone."
                            deleteConfirm.open()
                        }
                    }
                }
            }
        }

        AnimatedButton {
            text: "Generate System Report"
            primary: true
            onClicked: revolif.generateSystemReport()
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 8
            Label {
                text: "Achievements"
                font.bold: true
                font.pixelSize: Theme.fontSizeLg
                color: Theme.textPrimary
            }
            Item { Layout.fillWidth: true }
            AnimatedButton {
                text: "+ Add Achievement"
                primary: true
                onClicked: addAchievementDialog.open()
            }
        }

        ListView {
            id: achievementAdminList
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            spacing: 8
            clip: true
            model: []

            function refresh() { achievementAdminList.model = revolif.getAchievements() }

            Component.onCompleted: refresh()

            Connections {
                target: revolif
                function onStatsChanged() { achievementAdminList.refresh() }
            }

            delegate: Rectangle {
                width: ListView.view.width
                height: 56
                radius: Theme.radiusBase
                color: Theme.cardBg
                border.color: Theme.border
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 16

                    Label {
                        text: modelData.name
                        font.bold: true
                        color: Theme.textPrimary
                        Layout.preferredWidth: 160
                        elide: Text.ElideRight
                    }

                    Label {
                        text: modelData.description
                        color: Theme.textSecondary
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Label {
                        text: modelData.requiredGoals + " goals"
                        color: Theme.textMuted
                        Layout.preferredWidth: 80
                    }

                    AnimatedButton {
                        text: "Delete"
                        primary: false
                        visible: !modelData.isDefault
                        onClicked: {
                            deleteAchievementConfirm.achievementId = modelData.id
                            deleteAchievementConfirm.message = "Delete achievement \"" + modelData.name + "\"? This cannot be undone."
                            deleteAchievementConfirm.open()
                        }
                    }
                }
            }
        }
    }

    AddAchievementDialog { id: addAchievementDialog }

    ConfirmDialog {
        id: deleteAchievementConfirm
        property int achievementId: -1
        confirmText: "Delete"
        onConfirmed: revolif.removeAchievement(achievementId)
    }

    ConfirmDialog {
        id: deleteConfirm
        property string username: ""
        confirmText: "Delete"
        onConfirmed: revolif.permanentlyDeleteUser(username, "Admin action")
    }
}
