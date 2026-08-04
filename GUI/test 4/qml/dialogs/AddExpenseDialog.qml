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

    readonly property var monthNames: ["January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"]
    readonly property var dayNumbers: Array.from({length: 31}, function(_, i) { return i + 1 })
    readonly property var yearNumbers: {
        var y = new Date().getFullYear();
        var a = [];
        for (var i = y - 1; i <= y + 5; i++) a.push(i);
        return a;
    }

    function resetDate() {
        var now = new Date();
        expDay.currentIndex = now.getDate() - 1;
        expMonth.currentIndex = now.getMonth();
        expYear.currentIndex = yearNumbers.indexOf(now.getFullYear());
    }

    onOpened: resetDate()

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

        Label { text: "Date"; font.pixelSize: 12; color: Theme.textSecondary }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ComboBox { id: expDay; Layout.preferredWidth: 80; model: dialog.dayNumbers }
            ComboBox { id: expMonth; Layout.fillWidth: true; model: dialog.monthNames }
            ComboBox { id: expYear; Layout.preferredWidth: 100; model: dialog.yearNumbers }
        }

        Label { text: "Description"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: expDesc; Layout.fillWidth: true }

        AnimatedButton {
            Layout.fillWidth: true
            text: "Add Expense"
            primary: true
            onClicked: {
                revolif.addExpense(expTitle.text, parseFloat(expAmount.text), expCategory.currentText,
                                   dialog.dayNumbers[expDay.currentIndex],
                                   expMonth.currentIndex + 1,
                                   dialog.yearNumbers[expYear.currentIndex], expDesc.text);
                dialog.close();
            }
        }
    }
}
