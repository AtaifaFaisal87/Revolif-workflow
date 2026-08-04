import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"

Dialog {
    id: dialog
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 380
    standardButtons: Dialog.NoButton

    property string message: "Are you sure?"
    property string confirmText: "Delete"
    signal confirmed()

    contentItem: ColumnLayout {
        spacing: 20

        Label {
            text: dialog.message
            wrapMode: Text.Wrap
            color: Theme.textPrimary
            font.pixelSize: Theme.fontSizeBase
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            AnimatedButton {
                Layout.fillWidth: true
                text: "Cancel"
                primary: false
                onClicked: dialog.close()
            }

            AnimatedButton {
                Layout.fillWidth: true
                text: dialog.confirmText
                primary: true
                onClicked: {
                    dialog.confirmed();
                    dialog.close();
                }
            }
        }
    }
}
