/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import QtQuick.Controls 2.5
import "../themes/themes.js" as Theme

ListView {
    id: list

    required property var listName

    Accessible.role: Accessible.List
    Accessible.name: listName
    activeFocusOnTab: true
    interactive: false // disable scrolling on list since the entire window is scrollable
    highlightFollowsCurrentItem: true
    Keys.onDownPressed: list.incrementCurrentIndex()
    Keys.onUpPressed: list.decrementCurrentIndex()

    highlight: VPNFocus {
        itemToFocus: list
        itemToAnchor: list.currentItem
    }

}