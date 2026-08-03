import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    signal registerSuccessful()
    signal loginClicked()

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
            implicitHeight: 520
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.md

                Text {
                    text: "Create Account"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH2
                    font.weight: Typography.weightBold
                }

                Text {
                    text: "Begin your Emerald Odyssey journey today."
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                }

                Item { Layout.preferredHeight: Spacing.xxs }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs
                    Text { text: "Full Name"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; placeholderText: "John Operator" }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs
                    Text { text: "Email Address"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; placeholderText: "operator@revolif.io" }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs
                    Text { text: "Password"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; placeholderText: "••••••••••••" }
                }

                Item { Layout.fillHeight: true }

                PrimaryButton {
                    Layout.fillWidth: true
                    text: "Create Account"
                    onClicked: root.registerSuccessful()
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: Spacing.xs

                    Text {
                        text: "Already have an account?"
                        color: Colors.textMuted
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeSmall
                    }

                    Text {
                        text: "Sign In"
                        color: Colors.primaryHover
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeSmall
                        font.weight: Typography.weightBold

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.loginClicked()
                        }
                    }
                }
            }
        }
    }
}