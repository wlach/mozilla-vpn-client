/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import Mozilla.VPN 1.0
import "../components"
import "../themes/themes.js" as Theme

Item {
    id: viewAuthenticationEmail

    VPNMenu {
        id: menu
        objectName: "authenticationEmailBackButton"

        title: "Authentication By Email"
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
          height: 140
          logo: "../resources/auth_email.png"
          logoTitle: "Email address"
          anchors.top: parent.top
          anchors.topMargin: 10
      }

      Rectangle {
          id: email

          anchors.top: panel.bottom
          anchors.left: parent.left
          anchors.topMargin: 10
          anchors.leftMargin: 30
          color: Theme.bgColor
          border.color: Theme.fontColor
          border.width: 2
          radius: 4
          antialiasing: true
          height: 50
          width: Math.min(parent.width * 0.83, Theme.maxHorizontalContentWidth)

          TextEdit {
              text: "baku@mozilla.com"
              font.family: "Helvetica"
              font.pointSize: 15
              focus: true

              anchors.fill: parent
              anchors.leftMargin: 10
              anchors.topMargin: 10
          }
      }

      VPNButton {
          anchors.top: email.bottom
          anchors.topMargin: 10

          text: "Continue"
          anchors.horizontalCenterOffset: 0
          anchors.horizontalCenter: parent.horizontalCenter
          radius: 5
          onClicked: VPN.authenticate()

      }
    }
}
