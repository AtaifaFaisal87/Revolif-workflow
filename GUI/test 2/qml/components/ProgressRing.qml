import QtQuick
import "../theme"

Item {
    id: root

    property real value: 0.0 // 0.0 to 1.0
    property real strokeWidth: 8
    property color trackColor: Colors.bgSurfaceElevated
    property color progressColor: Colors.primary

    implicitWidth: 100
    implicitHeight: 100

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            var x = width / 2;
            var y = height / 2;
            var radius = Math.min(width, height) / 2 - root.strokeWidth / 2;
            var startAngle = -Math.PI / 2;
            var endAngle = startAngle + (2 * Math.PI * Math.max(0, Math.min(1, root.value)));

            // Draw Background Track
            ctx.beginPath();
            ctx.arc(x, y, radius, 0, 2 * Math.PI);
            ctx.lineWidth = root.strokeWidth;
            ctx.strokeStyle = root.trackColor;
            ctx.stroke();

            // Draw Progress Ring
            ctx.beginPath();
            ctx.arc(x, y, radius, startAngle, endAngle);
            ctx.lineWidth = root.strokeWidth;
            ctx.strokeStyle = root.progressColor;
            ctx.lineCap = "round";
            ctx.stroke();
        }

        Connections {
            target: root
            function onValueChanged() { canvas.requestPaint(); }
            function onProgressColorChanged() { canvas.requestPaint(); }
        }
    }
}