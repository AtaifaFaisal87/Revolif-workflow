import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    signal continueClicked()

    Rectangle {
        anchors.fill: parent
        color: Colors.bgBase
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Spacing.xl
        width: Math.min(500, parent.width - 80)

        GlassCard {
            Layout.fillWidth: true
            implicitHeight: 320
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.md

                Badge {
                    text: "EMERALD ODYSSEY"
                    badgeColor: Colors.primarySubtle
                    textColor: Colors.primaryHover
                }

                Text {
                    Layout.fillWidth: true
                    text: "Welcome, Operator"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH1
                    font.weight: Typography.weightBold
                }

                Text {
                    Layout.fillWidth: true
                    text: "Your synchronized workspace, life metrics, and biometric productivity streams are fully initialized."
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeBody
                    wrapMode: Text.WordWrap
                }

                Item { Layout.fillHeight: true }

                PrimaryButton {
                    Layout.fillWidth: true
                    text: "Enter Workspace"
                    onClicked: root.continueClicked()
                }
            }
        }
    }
}