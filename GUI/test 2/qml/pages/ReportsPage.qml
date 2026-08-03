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

        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.md

            SectionHeader {
                Layout.fillWidth: true
                title: "Executive Reports"
                subtitle: "Generate comprehensive weekly and monthly life summaries."
            }

            PrimaryButton {
                text: "Export PDF Report"
                iconSource: Icons.plus
                onClicked: console.log("Export triggered")
            }
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.md

                Text {
                    text: "Available Report Templates"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH3
                    font.weight: Typography.weightBold
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Monthly Emerald Odyssey Performance Summary (July 2026)"
                    category: "Executive"
                    completed: true
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Quarterly Financial Ledger & Capital Allocation Audit"
                    category: "Finance"
                    completed: false
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}