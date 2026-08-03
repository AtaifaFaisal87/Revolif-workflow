pragma Singleton
import QtQuick

QtObject {
    id: root

    // Duration Tokens (ms)
    readonly property int durationFast: 150
    readonly property int durationNormal: 250
    readonly property int durationSlow: 400

    // Easing Curves
    readonly property int easeInOut: Easing.InOutCubic
    readonly property int easeOut: Easing.OutCubic
    readonly property int easeIn: Easing.InCubic
    readonly property int easeBounce: Easing.OutBack
}