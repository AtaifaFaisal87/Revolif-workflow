import QtQuick
import "theme"

Item {
    id: root

    property string currentRoute: "dashboard"

    Item {
        id: wrapper
        anchors.fill: parent
        anchors.margins: Spacing.paddingLarge

        NavigationRouter {
            id: router
            anchors.fill: parent
            route: root.currentRoute

            Behavior on opacity {
                NumberAnimation {
                    duration: Animations.durationFast
                    easing.type: Animations.easeInOut
                }
            }
        }
    }

    onCurrentRouteChanged: {
        router.opacity = 0.0
        routeChangeAnimation.restart()
    }

    NumberAnimation {
        id: routeChangeAnimation
        target: router
        property: "opacity"
        from: 0.0
        to: 1.0
        duration: Animations.durationNormal
        easing.type: Animations.easeInOut
    }
}