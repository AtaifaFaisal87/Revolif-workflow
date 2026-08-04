import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "components"

ApplicationWindow {
    id: root
    visible: true
    width: 1366
    height: 768
    minimumWidth: 1024
    minimumHeight: 600
    title: "REVOLIF - Life, Beautifully Aligned"
    color: Theme.bg

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: revolif.isLoggedIn || revolif.isAdmin ? mainShell : authScreen
        replaceEnter: Transition {
            PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 250 }
        }
        replaceExit: Transition {
            PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: 250 }
        }
    }

    Component { id: authScreen; AuthScreen {} }
    Component { id: mainShell; MainShell {} }

    Connections {
        target: revolif
        function onIsLoggedInChanged() {
            if (revolif.isLoggedIn || revolif.isAdmin) {
                stackView.replace(mainShell)
            } else {
                stackView.replace(authScreen)
            }
        }
    }

    Toast {
        id: globalToast
        anchors.top: parent.top
        anchors.topMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Connections {
        target: revolif
        function onErrorOccurred(message) { globalToast.show(message, "error") }
        function onSuccessMessage(message) { globalToast.show(message, "success") }
    }
}
