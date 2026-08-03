import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.paddingLarge
        spacing: Spacing.lg

        SectionHeader {
            title: "System Settings"
            subtitle: "Configure application appearance, behavior, and telemetry."
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingLarge
                spacing: Spacing.lg

                Text {
                    text: "Appearance & Theme"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH3
                    font.weight: Typography.weightBold
                }

                RowLayout {
                    spacing: Spacing.md
                    Chip { text: "Emerald Odyssey (Dark)"; selected: true }
                    Chip { text: "Minimalist Light"; selected: false }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: Colors.borderSubtle }

                Text {
                    text: "Hardware Acceleration"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH3
                    font.weight: Typography.weightBold
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Enable GPU Rendering Pipeline via Qt Quick Compiler"
                    category: "Performance"
                    completed: true
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}