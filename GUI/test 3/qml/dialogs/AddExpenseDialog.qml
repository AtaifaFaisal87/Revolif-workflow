import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../"

Dialog {
    id: dialog
    title: "Add Expense"
    modal: true
    standardButtons: Dialog.Cancel
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 420

    contentItem: ColumnLayout {
        spacing: 12

        Label { text: "Title"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: expTitle; Layout.fillWidth: true }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Label { text: "Category"; font.pixelSize: 12; color: Theme.textSecondary }
                ComboBox {
                    id: expCategory
                    Layout.fillWidth: true
                    model: ["Food", "Transport", "Education", "Shopping", "Bills", "Entertainment", "Health", "Other"]
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Label { text: "Amount"; font.pixelSize: 12; color: Theme.textSecondary }
                TextField { id: expAmount; Layout.fillWidth: true; text: "0.00" }
            }
        }

        Label { text: "Date (DD/MM/YYYY)"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: expDate; placeholderText: "15/07/2026"; Layout.fillWidth: true }

        Label { text: "Description"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: expDesc; Layout.fillWidth: true }

        AnimatedButton {
            Layout.fillWidth: true
            text: "Add Expense"
            primary: true
            onClicked: {
                var parts = expDate.text.split('/');
                revolif.addExpense(expTitle.text, parseFloat(expAmount.text), expCategory.currentText,
                                   parseInt(parts[0]), parseInt(parts[1]), parseInt(parts[2]), expDesc.text);
                dialog.close();
            }
        }
    }
}
