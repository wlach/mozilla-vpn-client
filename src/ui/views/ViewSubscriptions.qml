/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import Mozilla.VPN 1.0
import "../components"
import "../themes/themes.js" as Theme

Item {
    id: viewSubscriptions

    VPNMenu {
        id: menu
        objectName: "subscriptionsBackButton"

        title: "Payment options"
        isSettingsView: false
        onActiveFocusChanged: if (focus) forceFocus = true
    }

    VPNFlickable {
      anchors.top: menu.bottom
      height: parent.height
      width: parent.width

      VPNPanel {
          id: authCard
          logoSize: 70
          height: 120
          logo: "../resources/subscription_cards.png"
          anchors.top: parent.top
          anchors.topMargin: 10

          VPNMouseArea {
              onClicked: VPN.authenticate()
          }
      }

      VPNPanel {
          id: authPaypal
          logoSize: 70
          height: 120
          logo: "../resources/subscription_paypal.png"
          anchors.top: authCard.bottom
          anchors.topMargin: 40

          VPNMouseArea {
              onClicked: stackview.push("ViewAuthenticationEmail.qml")
          }

      }

      VPNPanel {
          id: authApplePay
          logoSize: 70
          height: 120
          logo: "../resources/subscription_applepay.png"
          anchors.top: authPaypal.bottom
          anchors.topMargin: 40
      }

    }

}
