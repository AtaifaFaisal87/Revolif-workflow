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
    width: 480

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

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Label { text: "Deadline (DD/MM/YYYY)"; font.pixelSize: 12; color: Theme.textSecondary }
            TextField { id: taskDate; placeholderText: "15/07/2026"; Layout.preferredWidth: 120 }
            Label { text: "Hour"; font.pixelSize: 12; color: Theme.textSecondary }
            SpinBox { id: taskHour; from: 1; to: 12; value: 12 }
            Label { text: "Min"; font.pixelSize: 12; color: Theme.textSecondary }
            SpinBox { id: taskMin; from: 0; to: 59; value: 0 }
            ComboBox { id: taskMeridiem; model: ["AM", "PM"] }
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
                var parts = taskDate.text.split('/');
                var typeNum = taskType.currentIndex + 1;
                revolif.addTask(typeNum, taskTitle.text, taskDesc.text,
                                parseInt(parts[0]), parseInt(parts[1]), parseInt(parts[2]),
                                taskHour.value, taskMin.value, taskMeridiem.currentText,
                                taskPriority.currentText, taskRecurring.checked, taskInterval.currentText);
                dialog.close();
            }
        }
    }
}
