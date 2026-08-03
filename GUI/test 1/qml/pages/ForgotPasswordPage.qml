import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    signal resetRequested()
    signal backToLogin()

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
            implicitHeight: 360
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.md

                Text {
                    text: "Reset Password"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH2
                    font.weight: Typography.weightBold
                }

                Text {
                    text: "Enter your account email and we'll send you recovery instructions."
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Item { Layout.preferredHeight: Spacing.sm }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs
                    Text { text: "Email Address"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; placeholderText: "operator@revolif.io" }
                }

                Item { Layout.fillHeight: true }

                PrimaryButton {
                    Layout.fillWidth: true
                    text: "Send Reset Instructions"
                    onClicked: root.resetRequested()
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Back to Sign In"
                    color: Colors.primaryHover
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.backToLogin()
                    }
                }
            }
        }
    }
}