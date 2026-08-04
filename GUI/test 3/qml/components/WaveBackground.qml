import QtQuick

Rectangle {
    id: root
    color: "#E8F0EE"

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#F0F7F5" }
            GradientStop { position: 1.0; color: "#D4E9E3" }
        }
    }

    Canvas {
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            var w = width;
            var h = height;
            ctx.clearRect(0, 0, w, h);

            ctx.fillStyle = "#C8E6E0";
            ctx.beginPath();
            ctx.moveTo(0, h * 0.6);
            for (var x = 0; x <= w; x += 10) {
                ctx.lineTo(x, h * 0.6 + Math.sin(x / w * Math.PI * 2) * 20);
            }
            ctx.lineTo(w, h);
            ctx.lineTo(0, h);
            ctx.closePath();
            ctx.fill();

            ctx.fillStyle = "#A8D8CE";
            ctx.beginPath();
            ctx.moveTo(0, h * 0.7);
            for (var x2 = 0; x2 <= w; x2 += 10) {
                ctx.lineTo(x2, h * 0.7 + Math.sin(x2 / w * Math.PI * 2 + 1) * 25);
            }
            ctx.lineTo(w, h);
            ctx.lineTo(0, h);
            ctx.closePath();
            ctx.fill();

            ctx.fillStyle = "#88CABE";
            ctx.beginPath();
            ctx.moveTo(0, h * 0.8);
            for (var x3 = 0; x3 <= w; x3 += 10) {
                ctx.lineTo(x3, h * 0.8 + Math.sin(x3 / w * Math.PI * 2 + 2) * 30);
            }
            ctx.lineTo(w, h);
            ctx.lineTo(0, h);
            ctx.closePath();
            ctx.fill();
        }
    }
}
