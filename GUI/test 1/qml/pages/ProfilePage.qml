import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.paddingLarge
        spacing: Spacing.lg

        SectionHeader {
            title: "Operator Profile"
            subtitle: "Manage account credentials, security preferences, and tier status."
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.lg

                RowLayout {
                    spacing: Spacing.md

                    Rectangle {
                        width: 64
                        height: 64
                        radius: Spacing.radiusFull
                        color: Colors.primarySubtle
                        border.color: Colors.primary
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: "RO"
                            color: Colors.primaryHover
                            font.family: Typography.fontFamily
                            font.pointSize: Typography.sizeH2
                            font.weight: Typography.weightBold
                        }
                    }

                    ColumnLayout {
                        spacing: 2
                        Text { text: "Operator"; color: Colors.textPrimary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeH3; font.weight: Typography.weightBold }
                        Text { text: "operator@revolif.io — Emerald Tier"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeSmall }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: Colors.borderSubtle }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.sm

                    Text { text: "Display Name"; color: Colors.textSecondary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    SearchBar { Layout.fillWidth: true; text: "Operator" }
                }

                Item { Layout.fillHeight: true }

                PrimaryButton {
                    text: "Save Changes"
                    onClicked: console.log("Profile updated")
                }
            }
        }
    }
}