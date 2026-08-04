import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../"

Dialog {
    id: dialog
    title: "Add Task"
    modal: true
    standardButtons: Dialog.Cancel

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 520

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
        taskDay.currentIndex = now.getDate() - 1;
        taskMonth.currentIndex = now.getMonth();
        taskYear.currentIndex = 0;
    }

    onOpened: resetDate()

    contentItem: ColumnLayout {
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Label { text: "Type"; font.pixelSize: 12; color: Theme.textSecondary }
                ComboBox {
                    id: taskType
                    Layout.fillWidth: true
                    model: ["Academic", "Daily"]
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Label { text: "Priority"; font.pixelSize: 12; color: Theme.textSecondary }
                ComboBox {
                    id: taskPriority
                    Layout.fillWidth: true
                    model: ["High", "Medium", "Low"]
                }
            }
        }

        Label { text: "Title"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: taskTitle; Layout.fillWidth: true }

        Label { text: "Description"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: taskDesc; Layout.fillWidth: true }

        Label { text: "Deadline"; font.pixelSize: 12; color: Theme.textSecondary }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ComboBox { id: taskDay; Layout.preferredWidth: 80; model: dialog.dayNumbers }
            ComboBox { id: taskMonth; Layout.fillWidth: true; model: dialog.monthNames }
            ComboBox { id: taskYear; Layout.preferredWidth: 100; model: dialog.yearNumbers }
        }

        Label { text: "Time"; font.pixelSize: 12; color: Theme.textSecondary }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            SpinBox { id: taskHour; from: 1; to: 12; value: 12; Layout.preferredWidth: 90 }
            Label { text: ":"; font.pixelSize: 14; color: Theme.textSecondary }
            SpinBox { id: taskMin; from: 0; to: 59; value: 0; Layout.preferredWidth: 90 }
            ComboBox { id: taskMeridiem; Layout.preferredWidth: 90; model: ["AM", "PM"] }
            Item { Layout.fillWidth: true }
        }

        RowLayout {
            spacing: 12
            CheckBox { id: taskRecurring; text: "Recurring" }
            ComboBox {
                id: taskInterval
                enabled: taskRecurring.checked
                model: ["Daily", "Weekly", "Monthly"]
            }
        }

        AnimatedButton {
            Layout.fillWidth: true
            text: "Add Task"
            primary: true
            onClicked: {
                var typeNum = taskType.currentIndex + 1;
                revolif.addTask(typeNum, taskTitle.text, taskDesc.text,
                                dialog.dayNumbers[taskDay.currentIndex],
                                taskMonth.currentIndex + 1,
                                dialog.yearNumbers[taskYear.currentIndex],
                                taskHour.value, taskMin.value, taskMeridiem.currentText,
                                taskPriority.currentText, taskRecurring.checked, taskInterval.currentText);
                dialog.close();
            }
        }
    }
}
