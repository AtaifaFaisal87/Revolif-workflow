import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../"

Dialog {
    id: dialog
    title: "Add Goal"
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
        for (var i = y; i <= y + 5; i++) a.push(i);
        return a;
    }

    function resetDate() {
        var now = new Date();
        goalDay.currentIndex = now.getDate() - 1;
        goalMonth.currentIndex = now.getMonth();
        goalYear.currentIndex = 0;
    }

    onOpened: resetDate()

    contentItem: ColumnLayout {
        spacing: 12

        Label { text: "Title"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: goalTitle; Layout.fillWidth: true }

        Label { text: "Category"; font.pixelSize: 12; color: Theme.textSecondary }
        ComboBox {
            id: goalCategory
            Layout.fillWidth: true
            model: ["Academic", "Career", "Health", "Personal Development", "Financial", "Other"]
        }

        Label { text: "Description"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: goalDesc; Layout.fillWidth: true }

        Label { text: "Deadline"; font.pixelSize: 12; color: Theme.textSecondary }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ComboBox { id: goalDay; Layout.preferredWidth: 80; model: dialog.dayNumbers }
            ComboBox { id: goalMonth; Layout.fillWidth: true; model: dialog.monthNames }
            ComboBox { id: goalYear; Layout.preferredWidth: 100; model: dialog.yearNumbers }
        }

        AnimatedButton {
            Layout.fillWidth: true
            text: "Add Goal"
            primary: true
            onClicked: {
                revolif.addGoal(goalTitle.text, goalDesc.text, goalCategory.currentText,
                                dialog.dayNumbers[goalDay.currentIndex],
                                goalMonth.currentIndex + 1,
                                dialog.yearNumbers[goalYear.currentIndex]);
                dialog.close();
            }
        }
    }
}
