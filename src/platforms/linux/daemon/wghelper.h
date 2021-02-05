/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WIREGUARDHELPER_H
#define WIREGUARDHELPER_H

class QStringList;

class WireguardHelper final {
 public:
  // utils
  static bool interfaceExists();
  static QStringList currentDevices();
  // up
  static bool addIf();
  static bool setConf();
  // down
  static bool delDev();
};

#endif  // WIREGUARDHELPER_H
