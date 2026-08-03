import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "theme"
import "pages"

Window {
    id: mainWindow

    width: 1280
    height: 800
    minimumWidth: 1024
    minimumHeight: 700
    visible: true
    title: "REVOLIF — Emerald Odyssey"
    color: Colors.bgBase

    // ------------------------------------------------------------------
    // Application flow
    //
    // Pre-authentication:  Landing -> (Login | Register) -> ... -> Welcome
    // Main application:    Welcome -> AppShell (dashboard/tasks/goals/...)
    //
    // The page components below wire together the signals that already
    // existed on each page (getStartedClicked, loginSuccessful, etc.) but
    // were previously never connected to anything, so none of these
    // screens were reachable.
    // ------------------------------------------------------------------

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: landingPageComponent

        pushEnter: Transition {
            PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: Animations.durationNormal; easing.type: Animations.easeInOut }
        }
        pushExit: Transition {
            PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
        popEnter: Transition {
            PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: Animations.durationNormal; easing.type: Animations.easeInOut }
        }
        popExit: Transition {
            PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
        replaceEnter: Transition {
            PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: Animations.durationNormal; easing.type: Animations.easeInOut }
        }
        replaceExit: Transition {
            PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: Animations.durationFast; easing.type: Animations.easeInOut }
        }
    }

    // ---- Pre-authentication flow ----

    Component {
        id: landingPageComponent
        LandingPage {
            onGetStartedClicked: stackView.push(registerPageComponent)
            onLoginClicked: stackView.push(loginPageComponent)
        }
    }

    Component {
        id: loginPageComponent
        LoginPage {
            // Successful login clears the auth stack and moves into the
            // post-login welcome screen.
            onLoginSuccessful: {
                stackView.pop(null)
                stackView.push(welcomePageComponent)
            }
            onForgotPasswordClicked: stackView.push(forgotPasswordPageComponent)
            onRegisterClicked: stackView.push(registerPageComponent)
        }
    }

    Component {
        id: registerPageComponent
        RegisterPage {
            onRegisterSuccessful: stackView.push(emailVerificationPageComponent)
            onLoginClicked: stackView.push(loginPageComponent)
        }
    }

    Component {
        id: forgotPasswordPageComponent
        ForgotPasswordPage {
            onResetRequested: stackView.push(resetPasswordPageComponent)
            onBackToLogin: stackView.pop()
        }
    }

    Component {
        id: resetPasswordPageComponent
        ResetPasswordPage {
            // Password reset finished - return to a clean Login screen.
            onPasswordResetComplete: {
                stackView.pop(null)
                stackView.push(loginPageComponent)
            }
        }
    }

    Component {
        id: emailVerificationPageComponent
        EmailVerificationPage {
            onVerified: {
                stackView.pop(null)
                stackView.push(welcomePageComponent)
            }
        }
    }

    // ---- Post-login / entry into the main application ----

    Component {
        id: welcomePageComponent
        WelcomePage {
            onContinueClicked: {
                stackView.pop(null)
                stackView.push(appShellComponent)
            }
        }
    }

    Component {
        id: appShellComponent
        AppShell {}
    }
}
