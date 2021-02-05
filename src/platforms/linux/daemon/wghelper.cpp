/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include "wghelper.h"
#include "wgquickprocess.h"  // source of WG_INTERFACE

#include <QString>
#include <QByteArray>
#include <QStringList>

#include "logger.h"
#include "loghandler.h"

// Import wireguard C library
#if defined(__cplusplus)
extern "C" {
#endif
#include "../../3rdparty/wireguard-tools/contrib/embeddable-wg-library/wireguard.h"
#if defined(__cplusplus)
}
#endif

namespace {
Logger logger(LOG_LINUX, "WireguardHelper");
}

// static
bool WireguardHelper::interfaceExists() {
  // Also confirms it is wireguard.
  return WireguardHelper::currentDevices().contains(WG_INTERFACE);
};

// static
QStringList WireguardHelper::currentDevices() {
  char *deviceNames, *deviceName;
  size_t len;
  QStringList devices;
  deviceNames = wg_list_device_names();
  wg_for_each_device_name(deviceNames, deviceName, len) {
    devices.append(deviceName);
  }
  free(deviceNames);
  return devices;
}

// static
bool WireguardHelper::addIf() {
  int returnCode = wg_add_device(WG_INTERFACE);
  if (returnCode != 0) {
    qWarning("Adding interface `%s` failed with return code: %d", WG_INTERFACE,
             returnCode);
    return false;
  }
  return true;
}

// static
bool WireguardHelper::setConf() { return false; }
