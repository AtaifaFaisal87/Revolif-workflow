import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../"

Rectangle {
    id: profilePage
    color: Theme.bg

    ScrollView {
        anchors.fill: parent
        contentWidth: parent.width
        clip: true

        ColumnLayout {
            width: profilePage.width
            anchors.margins: 24
            spacing: 24

            Label {
                text: "Profile"
                font.pixelSize: Theme.fontSizeXl
                font.bold: true
                color: Theme.textPrimary
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: content.implicitHeight + 48
                radius: Theme.radiusLg
                color: Theme.cardBg
                border.color: Theme.border
                border.width: 1

                ColumnLayout {
                    id: content
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 16

                    RowLayout {
                        spacing: 24

                        Rectangle {
                            Layout.preferredWidth: 80
                            Layout.preferredHeight: 80
                            radius: 40
                            color: Theme.primary

                            Label {
                                anchors.centerIn: parent
                                text: revolif.currentUserName.charAt(0).toUpperCase()
                                font.pixelSize: 32
                                font.bold: true
                                color: "white"
                            }
                        }

                        ColumnLayout {
                            spacing: 4

                            Label {
                                text: revolif.currentUserName
                                font.pixelSize: Theme.fontSizeXl
                                font.bold: true
                                color: Theme.textPrimary
                            }

                            Label {
                                text: revolif.currentUserEmail
                                font.pixelSize: Theme.fontSizeBase
                                color: Theme.textSecondary
                            }
                        }
                    }

                    GridLayout {
                        columns: 2
                        columnSpacing: 24
                        rowSpacing: 16
                        Layout.fillWidth: true

                        Repeater {
                            id: profileRepeater
                            model: []

                            function refresh() {
                                var d = revolif.getProfileData();
                                profileRepeater.model = [
                                    { label: "Username", value: d.username || "", key: "" },
                                    { label: "Name", value: d.name || "", key: "name", editable: true },
                                    { label: "Email", value: d.email || "", key: "email", editable: true },
                                    { label: "Date of Birth", value: d.dob || "", key: "" },
                                    { label: "Member Since", value: d.registrationDate || "", key: "" },
                                    { label: "Login Streak", value: (d.streak || 0) + " days (Best: " + (d.bestStreak || 0) + ")", key: "" }
                                ];
                            }

                            Component.onCompleted: refresh()

                            Connections {
                                target: revolif
                                function onCurrentUserChanged() { profileRepeater.refresh() }
                            }

                            delegate: ColumnLayout {
                                spacing: 4

                                Label {
                                    text: modelData.label
                                    font.pixelSize: Theme.fontSizeSm
                                    color: Theme.textSecondary
                                }

                                RowLayout {
                                    spacing: 8

                                    TextField {
                                        id: fieldInput
                                        text: modelData.value
                                        enabled: modelData.editable === true
                                        background: Rectangle {
                                            radius: Theme.radiusBase
                                            border.color: fieldInput.enabled ? Theme.border : "transparent"
                                            border.width: 1
                                        }
                                        color: Theme.textPrimary
                                        font.pixelSize: Theme.fontSizeBase
                                    }

                                    AnimatedButton {
                                        text: "Save"
                                        visible: modelData.editable === true
                                        primary: true
                                        onClicked: revolif.updateProfile(modelData.key, fieldInput.text)
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: Theme.border
                    }

                    Label {
                        text: "Change Password"
                        font.bold: true
                        font.pixelSize: Theme.fontSizeLg
                        color: Theme.textPrimary
                    }

                    RowLayout {
                        spacing: 12

                        TextField {
                            id: oldPass
                            placeholderText: "Current password"
                            echoMode: TextInput.Password
                            background: Rectangle { radius: Theme.radiusBase; border.color: Theme.border; border.width: 1 }
                        }

                        TextField {
                            id: newPass
                            placeholderText: "New password"
                            echoMode: TextInput.Password
                            background: Rectangle { radius: Theme.radiusBase; border.color: Theme.border; border.width: 1 }
                        }

                        AnimatedButton {
                            text: "Change"
                            primary: true
                            onClicked: revolif.changePassword(oldPass.text, newPass.text)
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: Theme.border
                    }

                    AnimatedButton {
                        text: "Generate Monthly Report"
                        primary: true
                        onClicked: revolif.generateMonthlyReport()
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: Theme.border
                    }

                    AnimatedButton {
                        Layout.preferredWidth: 140
                        text: "Logout"
                        primary: false
                        onClicked: revolif.logout()
                    }
                }
            }
        }
    }
}
