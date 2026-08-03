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
                title: "Financial Ledger"
                subtitle: "Monitor cash flow, income streams, and capital allocation."
            }

            PrimaryButton {
                text: "Add Transaction"
                iconSource: Icons.plus
                onClicked: console.log("New transaction dialog opened")
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: Spacing.md
            columnSpacing: Spacing.md

            FinanceCard {
                Layout.fillWidth: true
                title: "Operating Revenue"
                amount: "4,850.00"
                isIncome: true
            }

            FinanceCard {
                Layout.fillWidth: true
                title: "Infrastructure Costs"
                amount: "1,240.00"
                isIncome: false
            }

            FinanceCard {
                Layout.fillWidth: true
                title: "Net Capital Surplus"
                amount: "3,610.00"
                isIncome: true
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
                    text: "Recent Ledger Entries"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH3
                    font.weight: Typography.weightBold
                }

                TimelineCard {
                    Layout.fillWidth: true
                    title: "Client Retainer Payout"
                    timestamp: "Yesterday, 04:15 PM"
                    description: "+$2,500.00 deposited to primary reserve account."
                    isLast: true
                }
            }
        }
    }
}