import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../dialogs"
import "../"

Rectangle {
    id: expensesPage
    color: Theme.bg

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "Expenses"
                font.pixelSize: Theme.fontSizeXl
                font.bold: true
                color: Theme.textPrimary
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "Total: $" + revolif.totalExpenses.toFixed(2)
                font.pixelSize: Theme.fontSizeLg
                font.bold: true
                color: Theme.primary
            }

            AnimatedButton {
                text: "+ Add Expense"
                primary: true
                onClicked: addExpenseDialog.open()
            }
        }

        ListView {
            id: expenseListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12
            clip: true
            model: []

            function refresh() { expenseListView.model = revolif.getExpenses() }

            Component.onCompleted: refresh()

            Connections {
                target: revolif
                function onStatsChanged() { expenseListView.refresh() }
            }

            delegate: Rectangle {
                width: ListView.view.width
                height: 70
                radius: Theme.radiusLg
                color: Theme.cardBg
                border.color: Theme.border
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    Rectangle {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        radius: 10
                        color: Theme.accentLight

                        Label {
                            anchors.centerIn: parent
                            text: "$"
                            font.bold: true
                            color: Theme.primary
                        }
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.fillWidth: true

                        Label {
                            text: modelData.title
                            font.bold: true
                            font.pixelSize: Theme.fontSizeBase
                            color: Theme.textPrimary
                        }

                        Label {
                            text: modelData.category + " · " + modelData.date
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }

                    Label {
                        text: "$" + modelData.amount.toFixed(2)
                        font.bold: true
                        font.pixelSize: Theme.fontSizeBase
                        color: Theme.textPrimary
                    }

                    Label {
                        text: "🗑"
                        font.pixelSize: 16
                        color: Theme.textMuted
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                deleteConfirm.expenseId = modelData.id
                                deleteConfirm.message = "Delete expense \"" + modelData.title + "\"? This cannot be undone."
                                deleteConfirm.open()
                            }
                        }
                    }
                }
            }
        }
    }

    AddExpenseDialog { id: addExpenseDialog }

    ConfirmDialog {
        id: deleteConfirm
        property int expenseId: -1
        confirmText: "Delete"
        onConfirmed: revolif.deleteExpense(expenseId)
    }
}
