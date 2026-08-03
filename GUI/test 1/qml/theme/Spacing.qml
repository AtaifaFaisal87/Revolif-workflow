pragma Singleton
import QtQuick

QtObject {
    id: root

    // Atomic Spacing Units
    readonly property real xxs: 2
    readonly property real xs: 4
    readonly property real sm: 8
    readonly property real md: 16
    readonly property real lg: 24
    readonly property real xl: 32
    readonly property real xxl: 48

    // Padding Standard Constants
    readonly property real paddingXsmall: 6
    readonly property real paddingSmall: 10
    readonly property real paddingMedium: 16
    readonly property real paddingLarge: 24

    // Border Radius Tokens
    readonly property real radiusSm: 4
    readonly property real radiusMd: 8
    readonly property real radiusLg: 12
    readonly property real radiusXl: 16
    readonly property real radiusFull: 9999

    // Layout Sizing Architecture
    readonly property real sidebarWidth: 260
    readonly property real headerHeight: 64
    readonly property real inputHeight: 40
    readonly property real buttonHeight: 40
    readonly property real iconSizeSm: 16
    readonly property real iconSizeMd: 20
    readonly property real iconSizeLg: 24
    readonly property real iconSizeXl: 32
}