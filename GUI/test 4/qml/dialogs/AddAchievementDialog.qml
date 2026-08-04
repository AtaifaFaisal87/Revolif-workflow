import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../"

Dialog {
    id: dialog
    title: "Add Achievement"
    modal: true
    standardButtons: Dialog.Cancel
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 420

    function reset() {
        achName.text = ""
        achDesc.text = ""
        achGoals.value = 1
    }

    onOpened: reset()

    contentItem: ColumnLayout {
        spacing: 12

        Label { text: "Name"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: achName; Layout.fillWidth: true }

        Label { text: "Description"; font.pixelSize: 12; color: Theme.textSecondary }
        TextField { id: achDesc; Layout.fillWidth: true }

        Label { text: "Required Goals"; font.pixelSize: 12; color: Theme.textSecondary }
        SpinBox { id: achGoals; from: 1; to: 1000; value: 1 }

        AnimatedButton {
            Layout.fillWidth: true
            text: "Add Achievement"
            primary: true
            onClicked: {
                revolif.addAchievement(achName.text, achDesc.text, achGoals.value);
                dialog.close();
            }
        }
    }
}
