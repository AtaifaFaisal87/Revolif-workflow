import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../dialogs"
import "../"

Rectangle {
    id: goalsPage
    color: Theme.bg

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "Goals"
                font.pixelSize: Theme.fontSizeXl
                font.bold: true
                color: Theme.textPrimary
            }

            Item { Layout.fillWidth: true }

            AnimatedButton {
                text: "+ Add Goal"
                primary: true
                onClicked: addGoalDialog.open()
            }
        }

        ListView {
            id: goalListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12
            clip: true
            model: []

            function refresh() { goalListView.model = revolif.getGoals() }

            Component.onCompleted: refresh()

            Connections {
                target: revolif
                function onStatsChanged() { goalListView.refresh() }
            }

            delegate: Rectangle {
                width: ListView.view.width
                height: 90
                radius: Theme.radiusLg
                color: Theme.cardBg
                border.color: Theme.border
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    Rectangle {
                        Layout.preferredWidth: 24
                        Layout.preferredHeight: 24
                        radius: 6
                        border.color: modelData.status === "Completed" ? Theme.success : Theme.border
                        border.width: 2
                        color: modelData.status === "Completed" ? Theme.success : "transparent"

                        Label {
                            anchors.centerIn: parent
                            text: "✓"
                            color: "white"
                            font.bold: true
                            font.pixelSize: 12
                            visible: modelData.status === "Completed"
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (modelData.status !== "Completed") revolif.completeGoal(modelData.id);
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.fillWidth: true

                        Label {
                            text: modelData.title
                            font.bold: true
                            font.pixelSize: Theme.fontSizeBase
                            color: Theme.textPrimary
                        }

                        Label {
                            text: modelData.category + " · Due: " + modelData.deadline
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 28
                        radius: Theme.radiusFull
                        color: modelData.displayStatus === "Overdue" ? "#FDE8E8" : modelData.displayStatus === "Completed" ? "#D1FAE5" : "#E0F2FE"

                        Label {
                            anchors.centerIn: parent
                            text: modelData.displayStatus
                            font.pixelSize: 11
                            font.bold: true
                            color: modelData.displayStatus === "Overdue" ? Theme.danger : modelData.displayStatus === "Completed" ? Theme.success : Theme.primary
                        }
                    }

                    Label {
                        text: "🗑"
                        font.pixelSize: 16
                        color: Theme.textMuted
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                deleteConfirm.goalId = modelData.id
                                deleteConfirm.message = "Delete goal \"" + modelData.title + "\"? This cannot be undone."
                                deleteConfirm.open()
                            }
                        }
                    }
                }
            }
        }
    }

    AddGoalDialog { id: addGoalDialog }

    ConfirmDialog {
        id: deleteConfirm
        property int goalId: -1
        confirmText: "Delete"
        onConfirmed: revolif.deleteGoal(goalId)
    }
}
