/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import Mozilla.VPN 1.0
import "../components"
import "../themes/themes.js" as Theme

Item {
    id: viewAuthenticationQRCode

    VPNMenu {
        id: menu
        objectName: "authenticationQRCodeBackButton"

        title: "Use a QR-Code"
        isSettingsView: false
        onActiveFocusChanged: if (focus) forceFocus = true
    }

    VPNFlickable {
      anchors.top: menu.bottom
      height: parent.height
      width: parent.width

      VPNPanel {
          id: panel
          logoSize: 70
          height: 270
          logo: "../resources/auth_qrcode.png"
          logoTitle: "Scan a QR-Code"
          anchors.top: parent.top
          anchors.topMargin: 10
      }

      VPNButton {
          id: sendButton

          anchors.top: panel.bottom
          anchors.topMargin: 10

          text: "Activate the camera"
          anchors.horizontalCenterOffset: 0
          anchors.horizontalCenter: parent.horizontalCenter
          radius: 5
          onClicked: stackview.push("ViewSubscriptions.qml")

      }
    }
}

