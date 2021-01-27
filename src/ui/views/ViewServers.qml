/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQml.Models 2.2
import Mozilla.VPN 1.0
import "../components"
import "../themes/themes.js" as Theme

Item {
    id: root
    VPNMenu {
        id: menu

        title: qsTrId("vpn.servers.selectLocation")
    }

    ButtonGroup {
        id: radioButtonGroup
    }


    VPNFlickable {
        id: vpnFlickable
        flickContentHeight: serverList.y + serverList.implicitHeight + (Theme.rowHeight * 2)
        height: parent.height - menu.height
        anchors.top: menu.bottom
        width: parent.width

        NumberAnimation on contentY {
            id: scrollAnimation

            duration: 300
            easing.type: Easing.OutQuad
        }

        Column {
            spacing: 14
            id: serverList
            width: parent.width
            Component.onCompleted: {
                for (let i = 0; i < repeater.count; i++) {
                    const countryItem = repeater.itemAt(i);
                    const serverListYCenter = vpnFlickable.height / 2;
                    const currentCityIndex = countryItem.currentCityIndex;
                    const currentCityYPosition = countryItem.y + Theme.rowHeight * 2 + (54 * currentCityIndex) - serverListYCenter;

                    if (countryItem.cityListVisible) {
                        if (serverListYCenter > currentCityYPosition) {
                            // current city y position is above the list vertical center
                            return;
                        }

                        vpnFlickable.contentY = currentCityYPosition;
                    }
                }
            }

            function scrollDelegateIntoView(item) {
                if (window.height > repeater.count * 40) {
                    return;
                }
                const yPosition = item.mapToItem(vpnFlickable.contentItem, 0, 0).y;
                const ext = item.height + yPosition;
                if (yPosition < vpnFlickable.contentY || yPosition > vpnFlickable.contentY + vpnFlickable.height || ext < vpnFlickable.contentY || ext > vpnFlickable.contentY + height) {
                    let destinationY = Math.max(0, Math.min(yPosition - vpnFlickable.height + item.height, vpnFlickable.contentHeight - vpnFlickable.height));
                    scrollAnimation.to = destinationY;
                    scrollAnimation.start();
                }
            }

            Repeater {
                id: repeater
                model: VPNServerCountryModel
                delegate: VPNServerCountry{}
            }
        }
    }

}
