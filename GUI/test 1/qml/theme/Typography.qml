pragma Singleton
import QtQuick

QtObject {
    id: root

    // Primary Font Family
    readonly property string fontFamily: "Inter"

    // Font Weights
    readonly property int weightLight: Font.Light
    readonly property int weightNormal: Font.Normal
    readonly property int weightMedium: Font.Medium
    readonly property int weightSemiBold: Font.DemiBold
    readonly property int weightBold: Font.Bold

    // Font Sizes (pt)
    readonly property real sizeDisplay: 32
    readonly property real sizeH1: 24
    readonly property real sizeH2: 20
    readonly property real sizeH3: 16
    readonly property real sizeBody: 14
    readonly property real sizeSmall: 12
    readonly property real sizeTiny: 10

    // Font Styles - Standardized Font Descriptors
    function applyDisplay(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeDisplay
        fontTarget.weight = weightBold
    }

    function applyH1(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeH1
        fontTarget.weight = weightBold
    }

    function applyH2(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeH2
        fontTarget.weight = weightSemiBold
    }

    function applyH3(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeH3
        fontTarget.weight = weightMedium
    }

    function applyBody(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeBody
        fontTarget.weight = weightNormal
    }

    function applyBodyBold(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeBody
        fontTarget.weight = weightBold
    }

    function applySmall(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeSmall
        fontTarget.weight = weightNormal
    }

    function applyTiny(fontTarget) {
        fontTarget.family = fontFamily
        fontTarget.pointSize = sizeTiny
        fontTarget.weight = weightMedium
    }
}