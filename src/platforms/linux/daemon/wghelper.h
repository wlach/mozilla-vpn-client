/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WIREGUARDHELPER_H
#define WIREGUARDHELPER_H

#include "daemon/daemon.h"

class QStringList;

class WireguardHelper final {
 public:
  // utils
  static bool interfaceExists();
  static QStringList currentDevices();
  static bool setPeerEndpoint(struct sockaddr* peerEndpoint,
                              const QString& address, int port);
  static bool setAllowedIpsOnPeer(struct wg_peer* peer,
                                  QList<IPAddressRange> allowedIPAddressRanges);
  // up
  static bool addIf();
  static bool setConf(const Daemon::Config& config);
  // down
  static bool delDev();
};

#endif  // WIREGUARDHELPER_H
