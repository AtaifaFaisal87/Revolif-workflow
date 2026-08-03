import QtQuick
import QtQuick.Layouts
import "../theme"
import "../components"

Flickable {
    id: root

    contentWidth: width
    contentHeight: mainLayout.implicitHeight + (Spacing.paddingLarge * 2)
    clip: true

    ColumnLayout {
        id: mainLayout
        width: parent.width - (Spacing.paddingLarge * 2)
        x: Spacing.paddingLarge
        y: Spacing.paddingLarge
        spacing: Spacing.lg

        // 1. Welcome Header & Date/Time
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.md

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Spacing.xxs

                Text {
                    text: "Welcome back, Operator"
                    color: Colors.textPrimary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeH1
                    font.weight: Typography.weightBold
                }

                Text {
                    text: "Saturday, August 1, 2026 — Emerald Odyssey v1.0"
                    color: Colors.textSecondary
                    font.family: Typography.fontFamily
                    font.pointSize: Typography.sizeSmall
                }
            }

            PrimaryButton {
                text: "Quick Capture"
                iconSource: Icons.plus
                onClicked: console.log("Quick capture triggered")
            }
        }

        // 2. Motivational Quote & Streaks Banner
        GlassCard {
            Layout.fillWidth: true
            implicitHeight: 90
            hoverable: false

            RowLayout {
                anchors.fill: parent
                anchors.margins: Spacing.paddingMedium
                spacing: Spacing.lg

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Spacing.xxs

                    Text {
                        text: "“Discipline is choosing between what you want now and what you want most.”"
                        color: Colors.textPrimary
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeBody
                        font.weight: Typography.weightMedium
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Daily Stoic Stream"
                        color: Colors.primaryHover
                        font.family: Typography.fontFamily
                        font.pointSize: Typography.sizeTiny
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: Colors.borderSubtle
                }

                RowLayout {
                    spacing: Spacing.lg

                    ColumnLayout {
                        spacing: 2
                        Text { text: "14 Days"; color: Colors.textPrimary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeH3; font.weight: Typography.weightBold; horizontalAlignment: Text.AlignHCenter }
                        Text { text: "Login Streak"; color: Colors.textMuted; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    }

                    ColumnLayout {
                        spacing: 2
                        Text { text: "42 Days"; color: Colors.textPrimary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeH3; font.weight: Typography.weightBold; horizontalAlignment: Text.AlignHCenter }
                        Text { text: "Best Streak"; color: Colors.textMuted; font.family: Typography.fontFamily; font.pointSize: Typography.sizeTiny }
                    }
                }
            }
        }

        // 3. Core Metrics Grid (StatCards & Life Score)
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            rowSpacing: Spacing.md
            columnSpacing: Spacing.md

            StatCard {
                Layout.fillWidth: true
                title: "Life Score"
                value: "88%"
                subtext: "+4.2% this week"
                iconSource: Icons.goals
                accentColor: Colors.primary
            }

            StatCard {
                Layout.fillWidth: true
                title: "Focus Time"
                value: "4.5 hrs"
                subtext: "Goal: 6.0 hrs"
                iconSource: Icons.focus
                accentColor: Colors.secondary
            }

            StatCard {
                Layout.fillWidth: true
                title: "Tasks Done"
                value: "12 / 15"
                subtext: "80% completed"
                iconSource: Icons.tasks
                accentColor: Colors.success
            }

            StatCard {
                Layout.fillWidth: true
                title: "Budget Balance"
                value: "$2,450"
                subtext: "+$320 today"
                iconSource: Icons.finance
                accentColor: Colors.warning
            }
        }

        // 4. Main Section Split (Left: Tasks/Goals, Right: Widgets/Widgets)
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.lg

            // Left Column: Pinned Tasks & Goals
            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                spacing: Spacing.md

                SectionHeader {
                    title: "Pinned Tasks"
                    subtitle: "High priority operational items for today"
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Finalize C++ Backend Integration for QML Engine"
                    category: "Engineering"
                    completed: false
                    onToggled: console.log("Task toggled")
                    onDeleted: console.log("Task deleted")
                }

                TaskCard {
                    Layout.fillWidth: true
                    title: "Review Financial Audit & Expense Projections"
                    category: "Finance"
                    completed: true
                    onToggled: console.log("Task toggled")
                    onDeleted: console.log("Task deleted")
                }

                Item { Layout.preferredHeight: Spacing.sm }

                SectionHeader {
                    title: "Active Goals"
                    subtitle: "Milestones tracking toward ultimate targets"
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    rowSpacing: Spacing.md
                    columnSpacing: Spacing.md

                    GoalCard {
                        Layout.fillWidth: true
                        title: "Launch v1.0 Desktop App"
                        progress: 0.85
                        targetDate: "Aug 15, 2026"
                    }

                    GoalCard {
                        Layout.fillWidth: true
                        title: "Master Advanced Qt 6 & QML Architecture"
                        progress: 0.60
                        targetDate: "Sep 30, 2026"
                    }
                }
            }

            // Right Column: Focus Timer, Budget Snapshot, Quick Notes, Activity
            ColumnLayout {
                Layout.preferredWidth: 340
                Layout.alignment: Qt.AlignTop
                spacing: Spacing.md

                // Focus Timer Widget
                GlassCard {
                    Layout.fillWidth: true
                    implicitHeight: 180
                    hoverable: false

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: Spacing.paddingMedium
                        spacing: Spacing.sm

                        RowLayout {
                            Layout.fillWidth: true
                            Text { text: "Focus Session"; color: Colors.textPrimary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeH3; font.weight: Typography.weightBold }
                            Item { Layout.fillWidth: true }
                            Badge { text: "Pomodoro"; badgeColor: Colors.primarySubtle; textColor: Colors.primaryHover }
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: "24:59"
                            color: Colors.textPrimary
                            font.family: Typography.fontFamily
                            font.pointSize: Typography.sizeDisplay
                            font.weight: Typography.weightBold
                        }

                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: Spacing.md

                            PrimaryButton {
                                text: "Pause"
                                iconSource: Icons.pause
                                implicitWidth: 100
                                onClicked: console.log("Timer paused")
                            }

                            SecondaryButton {
                                text: "Reset"
                                implicitWidth: 100
                                onClicked: console.log("Timer reset")
                            }
                        }
                    }
                }

                // Budget Snapshot Widget
                GlassCard {
                    Layout.fillWidth: true
                    implicitHeight: 150
                    hoverable: false

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: Spacing.paddingMedium
                        spacing: Spacing.sm

                        Text { text: "Budget Snapshot"; color: Colors.textPrimary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeH3; font.weight: Typography.weightBold }

                        FinanceCard {
                            Layout.fillWidth: true
                            title: "Monthly Revenue"
                            amount: "4,850.00"
                            isIncome: true
                        }

                        FinanceCard {
                            Layout.fillWidth: true
                            title: "Operational Costs"
                            amount: "1,240.00"
                            isIncome: false
                        }
                    }
                }

                // Recent Activity / Timeline Preview
                GlassCard {
                    Layout.fillWidth: true
                    implicitHeight: 200
                    hoverable: false

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: Spacing.paddingMedium
                        spacing: Spacing.sm

                        Text { text: "Recent Activity"; color: Colors.textPrimary; font.family: Typography.fontFamily; font.pointSize: Typography.sizeH3; font.weight: Typography.weightBold }

                        TimelineCard {
                            Layout.fillWidth: true
                            title: "Completed Milestone"
                            timestamp: "2 hours ago"
                            description: "Finished QML component library refactoring."
                            isLast: true
                        }
                    }
                }
            }
        }
    }
}