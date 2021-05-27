/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14

import "../themes/themes.js" as Theme
Item {
    property alias logo: logo.source
    property alias logoTitle: logoTitle.text
    property alias logoSubtitle: logoSubtitle.text
    property var logoSize: 76
    property var maskImage: false
    property var isSettingsView: false

    anchors.horizontalCenter: parent.horizontalCenter
    width: Math.min(parent.width - Theme.windowMargin * 3, Theme.maxHorizontalContentWidth)
    height: panel.height

    Column {
        id: panel

        anchors.left: parent.left
        anchors.right: parent.right
        width: parent.width

        anchors.verticalCenter: parent.verticalCenter
        spacing: 0

        Rectangle {
            id: logoWrapper

            color: "transparent"
            height: Math.max(logoSize, 76);
            width: height
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                id: logo

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: logoWrapper.bottom
                verticalAlignment: Image.AlignBottom
                anchors.bottomMargin: 0
                sourceSize.height: isSettingsView ? undefined : logoSize
                sourceSize.width: isSettingsView  ? undefined : logoSize
                fillMode: Image.PreserveAspectFit
                layer.enabled: true
                Component.onCompleted: {
                    if (isSettingsView ) {
                        logo.height = logoSize;
                        logo.width = logoSize;
                        logo.smooth = true;
                    }
                }

                Rectangle {
                    id: mask

                    anchors.fill: parent
                    radius: logoSize / 2
                    visible: false
                }
                layer.effect: OpacityMask {
                    maskSource: maskImage ? mask : undefined
                }

            }

        }

        VPNVerticalSpacer {
            height: 24
        }

        VPNHeadline {
            id: logoTitle

            // In Settings, the headline wrapMode is set to 'WrapAtWordBoundaryOrAnywhere' to
            // prevent very long, unbroken display names from throwing the layout
            wrapMode: isSettingsView ? Text.WrapAtWordBoundaryOrAnywhere : Text.WordWrap
            width: parent.width
        }

        VPNVerticalSpacer {
            height: 8
        }

        VPNSubtitle {
            id: logoSubtitle

            anchors.topMargin: 12
            width: parent.width
        }

    }

}
