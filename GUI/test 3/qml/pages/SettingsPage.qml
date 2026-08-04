import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../"

Rectangle {
    id: settingsPage
    color: Theme.bg

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: "Settings"
            font.pixelSize: Theme.fontSizeXl
            font.bold: true
            color: Theme.textPrimary
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            radius: Theme.radiusLg
            color: Theme.cardBg
            border.color: Theme.border
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16

                Label {
                    text: "Account Settings"
                    font.bold: true
                    font.pixelSize: Theme.fontSizeLg
                    color: Theme.textPrimary
                }

                Label {
                    text: "Manage your account preferences and data."
                    font.pixelSize: Theme.fontSizeBase
                    color: Theme.textSecondary
                }

                AnimatedButton {
                    text: "Logout"
                    primary: true
                    onClicked: revolif.logout()
                }
            }
        }
    }
}
