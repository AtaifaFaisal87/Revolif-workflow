import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../"

Rectangle {
    id: dashboard
    color: Theme.bg

    ScrollView {
        anchors.fill: parent
        contentWidth: parent.width
        clip: true

        ColumnLayout {
            width: dashboard.width
            anchors.margins: 24
            spacing: 24

            // Hero Banner
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 280
                radius: Theme.radiusXl
                clip: true

                WaveBackground {
                    anchors.fill: parent
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 32
                    spacing: 24

                    ColumnLayout {
                        spacing: 12
                        Layout.alignment: Qt.AlignVCenter

                        Label {
                            text: "JOURNEY · DIRECTION · GROWTH"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                            font.letterSpacing: 2
                        }

                        Label {
                            text: "Today's journey"
                            font.pixelSize: Theme.fontSize2xl
                            font.bold: true
                            color: Theme.textPrimary
                        }

                        Label {
                            text: "Small steps today, greater tomorrow."
                            font.pixelSize: Theme.fontSizeBase
                            color: Theme.textSecondary
                            font.italic: true
                        }

                        AnimatedButton {
                            Layout.preferredWidth: 140
                            text: "Let's focus →"
                            primary: true
                            onClicked: {
                                sidebar.currentPage = "tasks";
                                sidebar.navigate("tasks");
                            }
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Column {
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 8

                        ProgressRing {
                            id: dayProgress
                            width: 100
                            height: 100
                            progress: revolif.lifeScore / 100
                        }

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Day progress"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                        }
                    }
                }
            }

            // Today's Focus
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 12

                Label {
                    text: "TODAY'S FOCUS"
                    font.pixelSize: Theme.fontSizeXs
                    color: Theme.textSecondary
                    font.bold: true
                    font.letterSpacing: 1
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Repeater {
                        id: focusRepeater
                        model: []

                        function refresh() {
                            var data = revolif.getDashboardData();
                            focusRepeater.model = data.focusTasks || [];
                        }

                        Component.onCompleted: refresh()

                        Connections {
                            target: revolif
                            function onStatsChanged() { focusRepeater.refresh() }
                        }

                        delegate: Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 80
                            radius: Theme.radiusLg
                            color: Theme.cardBg
                            border.color: Theme.border
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 12

                                Rectangle {
                                    Layout.preferredWidth: 28
                                    Layout.preferredHeight: 28
                                    radius: 8
                                    color: modelData.priority === "High" ? Theme.danger : modelData.priority === "Medium" ? Theme.warning : Theme.success

                                    Label {
                                        anchors.centerIn: parent
                                        text: "✓"
                                        color: "white"
                                        font.bold: true
                                        font.pixelSize: 14
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: revolif.completeTask(modelData.id)
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
                                        text: modelData.deadline
                                        font.pixelSize: Theme.fontSizeSm
                                        color: Theme.textSecondary
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Tasks & Calendar
            RowLayout {
                Layout.fillWidth: true
                spacing: 24

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 320
                    radius: Theme.radiusLg
                    color: Theme.cardBg
                    border.color: Theme.border
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 12

                        Label {
                            text: "Upcoming tasks"
                            font.bold: true
                            font.pixelSize: Theme.fontSizeLg
                            color: Theme.textPrimary
                        }

                        ListView {
                            id: upcomingListView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            spacing: 8
                            model: []

                            function refresh() {
                                var data = revolif.getDashboardData();
                                upcomingListView.model = data.upcomingTasks || [];
                            }

                            Component.onCompleted: refresh()

                            Connections {
                                target: revolif
                                function onStatsChanged() { upcomingListView.refresh() }
                            }

                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 48
                                radius: Theme.radiusBase
                                color: Theme.hoverBg

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 12

                                    Rectangle {
                                        Layout.preferredWidth: 20
                                        Layout.preferredHeight: 20
                                        radius: 6
                                        border.color: Theme.border
                                        border.width: 2
                                        color: "transparent"
                                    }

                                    Label {
                                        text: modelData.title
                                        font.pixelSize: Theme.fontSizeBase
                                        color: Theme.textPrimary
                                        Layout.fillWidth: true
                                    }

                                    Label {
                                        text: modelData.time
                                        font.pixelSize: Theme.fontSizeSm
                                        color: Theme.textSecondary
                                    }
                                }
                            }
                        }
                    }
                }

                MiniCalendar {
                    Layout.preferredWidth: 280
                    Layout.preferredHeight: 320
                }
            }

            // Stats Row
            RowLayout {
                Layout.fillWidth: true
                spacing: 16

                Repeater {
                    model: [
                        { label: "Life Score", value: revolif.lifeScore, suffix: "/100", color: Theme.primary },
                        { label: "Day Streak", value: revolif.currentStreak, suffix: " days", color: Theme.accent },
                        { label: "Pending Tasks", value: revolif.pendingTasks, suffix: "", color: Theme.warning },
                        { label: "Goals Done", value: revolif.completedGoals, suffix: "", color: Theme.success }
                    ]

                    delegate: Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 100
                        radius: Theme.radiusLg
                        color: Theme.cardBg
                        border.color: Theme.border
                        border.width: 1

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                text: modelData.value + modelData.suffix
                                font.bold: true
                                font.pixelSize: 24
                                color: modelData.color
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                text: modelData.label
                                font.pixelSize: Theme.fontSizeSm
                                color: Theme.textSecondary
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }
                }
            }
        }
    }
}
