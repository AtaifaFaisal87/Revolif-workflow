pragma Singleton
import QtQuick

QtObject {
    id: root

    readonly property Colors colors: Colors
    readonly property Typography typography: Typography
    readonly property Spacing spacing: Spacing
    readonly property Animations animations: Animations
    readonly property Icons icons: Icons
}