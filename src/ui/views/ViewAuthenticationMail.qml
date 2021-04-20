/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import Mozilla.VPN 1.0
import "../components"
import "../themes/themes.js" as Theme

Item {
    id: viewAuthenticationMail

    VPNMenu {
        id: menu
        objectName: "authenticationMailBackButton"

        title: "Subscribe by Mail"
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
          height: 250
          logo: "../resources/auth_email.png"
          logoTitle: "Send this unique code to:"
          logoSubtitle: "Metal Box Factory\nSuite 441, 4th floor\n30 Great Guildford Street\nLondon\nSE1 0HS"
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
              text: "1masa003SDz"
              font.family: "Helvetica"
              font.pointSize: 15
              focus: true

              anchors.fill: parent
              anchors.leftMargin: 10
              anchors.topMargin: 10
          }
      }

      Text {
          anchors.top: email.bottom
          anchors.topMargin: 10

          text: "We will enable your account in 20 days"
          anchors.horizontalCenterOffset: 0
          anchors.horizontalCenter: parent.horizontalCenter
      }
    }
}
