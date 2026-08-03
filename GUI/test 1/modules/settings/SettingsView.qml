import QtQuick
import QtQuick.Layouts
import "../../qml/theme"
import "../../qml/components"

Item {
    id: root
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.paddingLarge
        spacing: Spacing.lg

        SectionHeader {
            title: "Settings Module View"
            subtitle: "System parameters, performance toggles, and UI options."
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.md

                Text {
                    text: "Settings Subsystem Initialized"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH3
                    font.weight: Typography.weightBold
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}