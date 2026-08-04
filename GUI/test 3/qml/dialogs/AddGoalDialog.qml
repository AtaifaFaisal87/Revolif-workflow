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

        Label { text: "Deadline (DD/MM/YYYY)"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: goalDate; placeholderText: "15/07/2026"; Layout.fillWidth: true }

        AnimatedButton {
            Layout.fillWidth: true
            text: "Add Goal"
            primary: true
            onClicked: {
                var parts = goalDate.text.split('/');
                revolif.addGoal(goalTitle.text, goalDesc.text, goalCategory.currentText,
                                parseInt(parts[0]), parseInt(parts[1]), parseInt(parts[2]));
                dialog.close();
            }
        }
    }
}
