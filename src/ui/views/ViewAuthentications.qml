/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import Mozilla.VPN 1.0
import "../components"
import "../themes/themes.js" as Theme

Item {
    id: viewAuthentications

    VPNMenu {
        id: menu
        objectName: "authenticationsBackButton"

        title: "Authentication Providers"
        isSettingsView: false
        onActiveFocusChanged: if (focus) forceFocus = true
    }

    VPNFlickable {
      anchors.top: menu.bottom
      height: parent.height
      width: parent.width

      VPNPanel {
          id: authFxA
          logoSize: 70
          height: 140
          logo: "../resources/auth_firefox.png"
          logoTitle: "FxA"
          logoSubtitle: "Firefox Account Provider"
          anchors.top: parent.top
          anchors.topMargin: 10

          VPNMouseArea {
              onClicked: VPN.authenticate()
          }
      }

      VPNPanel {
          id: authEmail
          logoSize: 70
          height: 140
          logo: "../resources/auth_email.png"
          logoTitle: "Email Address"
          logoSubtitle: "Use your email address"
          anchors.top: authFxA.bottom
          anchors.topMargin: 40

          VPNMouseArea {
              onClicked: stackview.push("ViewAuthenticationEmail.qml")
          }

      }

      VPNPanel {
          id: authGoogle
          logoSize: 70
          height: 140
          logo: "../resources/auth_google.png"
          logoTitle: "Google"
          logoSubtitle: "Use your google account"
          anchors.top: authEmail.bottom
          anchors.topMargin: 40
      }

      VPNPanel {
          id: authFacebook
          logoSize: 40
          height: 140
          logo: "../resources/auth_facebook.png"
          logoTitle: "Facebook"
          logoSubtitle: "Use your facebook account"
          anchors.top: authGoogle.bottom
          anchors.topMargin: 40
      }

      VPNPanel {
          id: authMail
          logoSize: 70
          height: 140
          logo: "../resources/auth_email.png"
          logoTitle: "Mail subscription"
          logoSubtitle: "Anonymous payment"
          anchors.top: authFacebook.bottom
          anchors.topMargin: 40

          VPNMouseArea {
              onClicked: stackview.push("ViewAuthenticationMail.qml")
          }

      }
    }

}
