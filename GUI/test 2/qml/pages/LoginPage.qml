import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    signal loginSuccessful()
    signal forgotPasswordClicked()
    signal registerClicked()

    Rectangle {
        anchors.fill: parent
        color: Colors.bgBase
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Spacing.lg
        width: Math.min(440, parent.width - 40)

        GlassCard {
            Layout.fillWidth: true
            implicitHeight: 460
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.md

                Text {
                    text: "Sign In"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH2
                    font.weight: Typography.weightBold
                }

                Text {
                    text: "Enter your credentials to access REVOLIF."
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                }

                Item { Layout.preferredHeight: Spacing.xs }

                // Email Field Container
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs

                    Text {
                        text: "Email Address"
                        color: Colors.textSecondary
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeTiny
                    }

                    SearchBar {
                        Layout.fillWidth: true
                        placeholderText: "operator@revolif.io"
                    }
                }

                // Password Field Container
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs

                    Text {
                        text: "Password"
                        color: Colors.textSecondary
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeTiny
                    }

                    SearchBar {
                        Layout.fillWidth: true
                        placeholderText: "••••••••••••"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "Forgot Password?"
                        color: Colors.primaryHover
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeSmall

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.forgotPasswordClicked()
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                PrimaryButton {
                    Layout.fillWidth: true
                    text: "Sign In"
                    onClicked: root.loginSuccessful()
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: Spacing.xs

                    Text {
                        text: "Don't have an account?"
                        color: Colors.textMuted
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeSmall
                    }

                    Text {
                        text: "Register"
                        color: Colors.primaryHover
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeSmall
                        font.weight: Typography.weightBold

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.registerClicked()
                        }
                    }
                }
            }
        }
    }
}