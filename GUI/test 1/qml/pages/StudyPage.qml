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
            title: "Study & Knowledge Matrix"
            subtitle: "Manage learning decks, technical notes, and skill progression."
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.xs

            Chip { text: "All Decks"; selected: true; onClicked: console.log("Study: All") }
            Chip { text: "Qt 6 & QML"; selected: false; onClicked: console.log("Study: Qt") }
            Chip { text: "C++ Architecture"; selected: false; onClicked: console.log("Study: C++") }
            Chip { text: "Design Systems"; selected: false; onClicked: console.log("Study: Design") }
        }

        GlassCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            hoverable: false

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.md

                TaskCard {
                    Layout.fillWidth: true
                    title: "Advanced QML Singletons and Custom C++ Visual Items"
                    category: "Qt 6 & QML"
                    completed: true
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Memory Management and Smart Pointers in Modern C++"
                    category: "C++ Architecture"
                    completed: false
                }
            }
        }
    }
}