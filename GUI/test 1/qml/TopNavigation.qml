import QtQuick
import QtQuick.Layouts
import "theme"
import "components"

Rectangle {
    id: root

    signal searchAccepted(string query)
    signal notificationClicked()
    signal settingsClicked()

    color: Colors.bgSurface
    border.color: Colors.borderSubtle
    border.width: 1

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Spacing.paddingLarge
        anchors.rightMargin: Spacing.paddingLarge
        spacing: Spacing.md

        SearchBar {
            id: searchBar
            placeholderText: "Search workspace..."
            onAccepted: function(query) {
                root.searchAccepted(query)
            }
        }

        Item { Layout.fillWidth: true }

        RowLayout {
            spacing: Spacing.sm

            IconButton {
                iconSource: Icons.bell
                size: 38
                iconSize: Spacing.iconSizeMd
                onClicked: root.notificationClicked()
            }

            IconButton {
                iconSource: Icons.settings
                size: 38
                iconSize: Spacing.iconSizeMd
                onClicked: root.settingsClicked()
            }
        }
    }
}