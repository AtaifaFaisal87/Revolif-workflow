pragma Singleton
import QtQuick

QtObject {
    property color primaryDark: "#0A3C30"
    property color primary: "#00674F"
    property color accent: "#3EBB9E"
    property color accentLight: "#73E6CB"
    property color accentHover: "#2DA88C"

    property color bg: "#F5F3EE"
    property color sidebarBg: "#F7FAF9"
    property color cardBg: "#FFFFFF"
    property color hoverBg: "#EEF3F1"
    property color selectedBg: "#E3F0EC"

    property color textPrimary: "#1A2E2A"
    property color textSecondary: "#6B7B77"
    property color textMuted: "#A9B6B2"
    property color textOnPrimary: "#FFFFFF"

    property color success: "#3EBB9E"
    property color warning: "#E0A030"
    property color danger: "#D9534F"
    property color info: "#3EBB9E"

    property color border: "#E3EBE8"
    property color borderLight: "#F0F4F3"

    property string fontFamily: "Segoe UI, Helvetica, Arial, sans-serif"
    property int fontSizeXs: 11
    property int fontSizeSm: 13
    property int fontSizeBase: 15
    property int fontSizeLg: 18
    property int fontSizeXl: 24
    property int fontSize2xl: 32

    property int radiusSm: 8
    property int radiusBase: 12
    property int radiusLg: 16
    property int radiusXl: 20
    property int radiusFull: 999

    property int sidebarWidth: 260
}
