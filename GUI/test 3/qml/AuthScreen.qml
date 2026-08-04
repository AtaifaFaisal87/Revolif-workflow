import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"

Rectangle {
    id: authScreen
    color: Theme.bg

    property bool isLogin: true

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(420, parent.width - 48)
        spacing: 24

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 8

            Image {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/logo.jpeg"
                fillMode: Image.PreserveAspectFit
                Layout.preferredWidth: 120
                Layout.preferredHeight: 120
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "REVOLIF"
                font.pixelSize: Theme.fontSize2xl
                font.bold: true
                color: Theme.primary
                font.letterSpacing: 4
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "Life, Beautifully Aligned."
                font.pixelSize: Theme.fontSizeSm
                color: Theme.textSecondary
                font.letterSpacing: 2
            }
        }

        Rectangle {
            Layout.fillWidth: true
            color: Theme.cardBg
            radius: Theme.radiusXl
            Layout.preferredHeight: contentCol.implicitHeight + 64

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: Theme.border
                border.width: 1
                radius: Theme.radiusXl
            }

            ColumnLayout {
                id: contentCol
                anchors.fill: parent
                anchors.margins: 32
                spacing: 16

                Label {
                    text: isLogin ? "Welcome Back" : "Create Account"
                    font.pixelSize: Theme.fontSizeXl
                    font.bold: true
                    color: Theme.textPrimary
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    visible: !isLogin
                    Label {
                        text: "Full Name"
                        font.pixelSize: Theme.fontSizeSm
                        color: Theme.textSecondary
                    }
                    TextField {
                        id: nameField
                        Layout.fillWidth: true
                        placeholderText: "Enter your full name"
                        background: Rectangle {
                            radius: Theme.radiusBase
                            border.color: parent.activeFocus ? Theme.accent : Theme.border
                            border.width: parent.activeFocus ? 2 : 1
                        }
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSizeBase
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    Label {
                        text: "Username"
                        font.pixelSize: Theme.fontSizeSm
                        color: Theme.textSecondary
                    }
                    TextField {
                        id: usernameField
                        Layout.fillWidth: true
                        placeholderText: "Enter username"
                        background: Rectangle {
                            radius: Theme.radiusBase
                            border.color: parent.activeFocus ? Theme.accent : Theme.border
                            border.width: parent.activeFocus ? 2 : 1
                        }
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSizeBase
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    visible: !isLogin
                    Label {
                        text: "Date of Birth (DD/MM/YYYY)"
                        font.pixelSize: Theme.fontSizeSm
                        color: Theme.textSecondary
                    }
                    TextField {
                        id: dobField
                        Layout.fillWidth: true
                        placeholderText: "15/07/1998"
                        background: Rectangle {
                            radius: Theme.radiusBase
                            border.color: parent.activeFocus ? Theme.accent : Theme.border
                            border.width: parent.activeFocus ? 2 : 1
                        }
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSizeBase
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    visible: !isLogin
                    Label {
                        text: "Email"
                        font.pixelSize: Theme.fontSizeSm
                        color: Theme.textSecondary
                    }
                    TextField {
                        id: emailField
                        Layout.fillWidth: true
                        placeholderText: "user@example.com"
                        background: Rectangle {
                            radius: Theme.radiusBase
                            border.color: parent.activeFocus ? Theme.accent : Theme.border
                            border.width: parent.activeFocus ? 2 : 1
                        }
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSizeBase
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    Label {
                        text: "Password"
                        font.pixelSize: Theme.fontSizeSm
                        color: Theme.textSecondary
                    }
                    TextField {
                        id: passwordField
                        Layout.fillWidth: true
                        placeholderText: "Enter password"
                        echoMode: TextInput.Password
                        background: Rectangle {
                            radius: Theme.radiusBase
                            border.color: parent.activeFocus ? Theme.accent : Theme.border
                            border.width: parent.activeFocus ? 2 : 1
                        }
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSizeBase
                    }
                }

                AnimatedButton {
                    Layout.fillWidth: true
                    text: isLogin ? "Login" : "Register"
                    primary: true
                    onClicked: {
                        if (isLogin) {
                            revolif.login(usernameField.text, passwordField.text)
                        } else {
                            revolif.registerUser(nameField.text, usernameField.text, dobField.text, emailField.text, passwordField.text)
                        }
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 4
                    Label {
                        text: isLogin ? "Don't have an account?" : "Already have an account?"
                        font.pixelSize: Theme.fontSizeSm
                        color: Theme.textSecondary
                    }
                    Label {
                        text: isLogin ? "Register" : "Login"
                        font.pixelSize: Theme.fontSizeSm
                        color: Theme.accent
                        font.bold: true
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: authScreen.isLogin = !authScreen.isLogin
                        }
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Admin login: admin / admin123"
                    font.pixelSize: 10
                    color: Theme.textMuted
                    visible: isLogin
                }
            }
        }
    }
}
