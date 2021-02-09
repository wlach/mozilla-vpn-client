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
  static bool deviceExists();
  static QStringList currentDevices();
  // up
  static bool addDevice();
  static bool configureDevice(const Daemon::Config& config);
  static bool addDeviceIps(const Daemon::Config& config);
  static bool setMTUAndUp();
  // down
  static bool delDevice();

 private:
  static bool setPeerEndpoint(struct sockaddr* peerEndpoint,
                              const QString& address, int port);
  static bool setAllowedIpsOnPeer(struct wg_peer* peer,
                                  QList<IPAddressRange> allowedIPAddressRanges);
  static bool addIP4AddressToDevice(const Daemon::Config& conf);
  static bool addIP6AddressToDevice(const Daemon::Config& conf);
  static wg_peer* buildPeerForDevice(struct wg_device* device,
                                     const Daemon::Config& conf);
};

#endif  // WIREGUARDHELPER_H
