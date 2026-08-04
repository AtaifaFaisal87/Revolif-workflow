import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"

Rectangle {
    id: miniCal
    color: Theme.cardBg
    radius: Theme.radiusLg
    border.color: Theme.border
    border.width: 1

    property date currentDate: new Date()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: Qt.formatDate(miniCal.currentDate, "MMMM yyyy")
                font.bold: true
                font.pixelSize: Theme.fontSizeBase
                color: Theme.textPrimary
            }
            Item { Layout.fillWidth: true }
        }

        GridLayout {
            columns: 7
            rowSpacing: 4
            columnSpacing: 4
            Layout.fillWidth: true

            Repeater {
                model: ["S","M","T","W","T","F","S"]
                delegate: Label {
                    text: modelData
                    font.pixelSize: 11
                    color: Theme.textMuted
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            Repeater {
                model: 35
                delegate: Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 28
                    radius: 6
                    color: {
                        var d = index - 3;
                        var today = new Date().getDate();
                        return d === today ? Theme.primary : "transparent";
                    }

                    Label {
                        anchors.centerIn: parent
                        text: {
                            var d = index - 3;
                            return d > 0 && d <= 31 ? d : "";
                        }
                        font.pixelSize: 12
                        color: {
                            var d = index - 3;
                            var today = new Date().getDate();
                            return d === today ? "white" : Theme.textPrimary;
                        }
                    }
                }
            }
        }
    }
}
