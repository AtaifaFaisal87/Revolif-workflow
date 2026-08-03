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

        // Header and Actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.md

            SectionHeader {
                Layout.fillWidth: true
                title: "Task Management"
                subtitle: "Organize, prioritize, and execute your operational pipeline."
            }

            PrimaryButton {
                text: "New Task"
                iconSource: Icons.plus
                onClicked: console.log("New task dialog opened")
            }
        }

        // Filter and Search Toolbar
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.sm

            SearchBar {
                id: taskSearch
                placeholderText: "Search tasks..."
                Layout.preferredWidth: 320
            }

            Item { Layout.fillWidth: true }

            RowLayout {
                spacing: Spacing.xs

                Chip {
                    text: "All"
                    selected: true
                    onClicked: console.log("Filter: All")
                }

                Chip {
                    text: "Pending"
                    selected: false
                    onClicked: console.log("Filter: Pending")
                }

                Chip {
                    text: "Completed"
                    selected: false
                    onClicked: console.log("Filter: Completed")
                }

                Chip {
                    text: "High Priority"
                    selected: false
                    onClicked: console.log("Filter: High Priority")
                }
            }
        }

        // Task List Container inside GlassCard
        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.sm

                TaskCard {
                    Layout.fillWidth: true
                    title: "Implement QML Navigation Router and Page Container Transitions"
                    category: "Engineering"
                    completed: true
                    onToggled: console.log("Task toggled")
                    onDeleted: console.log("Task deleted")
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Configure CMake Build Rules for Asset and Resource Bundles"
                    category: "Engineering"
                    completed: false
                    onToggled: console.log("Task toggled")
                    onDeleted: console.log("Task deleted")
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Perform Weekly Financial Audit and Categorization Review"
                    category: "Finance"
                    completed: false
                    onToggled: console.log("Task toggled")
                    onDeleted: console.log("Task deleted")
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Draft Phase 6 Integration Testing Specifications"
                    category: "Operations"
                    completed: false
                    onToggled: console.log("Task toggled")
                    onDeleted: console.log("Task deleted")
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}