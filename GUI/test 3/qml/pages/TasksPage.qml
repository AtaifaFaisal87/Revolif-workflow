import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../dialogs"
import "../"

Rectangle {
    id: tasksPage
    color: Theme.bg

    property string filter: "all"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: "Tasks"
                font.pixelSize: Theme.fontSizeXl
                font.bold: true
                color: Theme.textPrimary
            }

            Item { Layout.fillWidth: true }

            Row {
                spacing: 8
                Repeater {
                    model: ["All", "Pending", "Completed"]
                    delegate: Rectangle {
                        width: label.implicitWidth + 24
                        height: 36
                        radius: Theme.radiusFull
                        color: tasksPage.filter.toLowerCase() === modelData.toLowerCase() ? Theme.primary : Theme.cardBg
                        border.color: Theme.border
                        border.width: 1

                        Label {
                            id: label
                            anchors.centerIn: parent
                            text: modelData
                            color: tasksPage.filter.toLowerCase() === modelData.toLowerCase() ? "white" : Theme.textPrimary
                            font.pixelSize: Theme.fontSizeSm
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                tasksPage.filter = modelData.toLowerCase();
                                taskListView.refresh();
                            }
                        }
                    }
                }
            }

            AnimatedButton {
                text: "+ Add Task"
                primary: true
                onClicked: addTaskDialog.open()
            }
        }

        ListView {
            id: taskListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12
            clip: true
            model: []

            function refresh() {
                var all = revolif.getTasks();
                if (tasksPage.filter === "all") {
                    taskListView.model = all;
                } else {
                    taskListView.model = all.filter(function(t) {
                        return tasksPage.filter === "pending" ? t.status === "Pending" : t.status === "Completed";
                    });
                }
            }

            Component.onCompleted: refresh()

            Connections {
                target: revolif
                function onStatsChanged() { taskListView.refresh() }
            }

            delegate: Rectangle {
                width: ListView.view.width
                height: 80
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
                                if (modelData.status !== "Completed") revolif.completeTask(modelData.id);
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
                            color: modelData.status === "Completed" ? Theme.textMuted : Theme.textPrimary
                        }

                        Label {
                            text: modelData.category + " · " + modelData.deadline + " · " + modelData.priority
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 28
                        radius: Theme.radiusFull
                        color: modelData.priority === "High" ? "#FDE8E8" : modelData.priority === "Medium" ? "#FEF3C7" : "#D1FAE5"

                        Label {
                            anchors.centerIn: parent
                            text: modelData.priority
                            font.pixelSize: 11
                            font.bold: true
                            color: modelData.priority === "High" ? Theme.danger : modelData.priority === "Medium" ? Theme.warning : Theme.success
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
                                deleteConfirm.taskId = modelData.id
                                deleteConfirm.message = "Delete task \"" + modelData.title + "\"? This cannot be undone."
                                deleteConfirm.open()
                            }
                        }
                    }
                }
            }
        }
    }

    AddTaskDialog {
        id: addTaskDialog
    }

    ConfirmDialog {
        id: deleteConfirm
        property int taskId: -1
        confirmText: "Delete"
        onConfirmed: revolif.deleteTask(taskId)
    }
}
