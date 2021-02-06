/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dbusservice.h"
#include "dbus_adaptor.h"
#include "leakdetector.h"
#include "logger.h"
#include "loghandler.h"
#include "polkithelper.h"
#include "wgquickprocess.h"
#include "wghelper.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>

#include <arpa/inet.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include "../../3rdparty/wireguard-tools/contrib/embeddable-wg-library/wireguard.h"

#if defined(__cplusplus)
}
#endif

namespace {
Logger logger(LOG_LINUX, "DBusService");
}

DBusService::DBusService(QObject* parent) : Daemon(parent) {
  MVPN_COUNT_CTOR(DBusService);
}

DBusService::~DBusService() { MVPN_COUNT_DTOR(DBusService); }

void DBusService::setAdaptor(DbusAdaptor* adaptor) {
  Q_ASSERT(!m_adaptor);
  m_adaptor = adaptor;
}

bool DBusService::checkInterface() {
  // TODO: Replace with new helper methods
  logger.log() << "Checking interface";

  wg_device* device = nullptr;
  if (wg_get_device(&device, WG_INTERFACE) == 0) {
    logger.log() << "Device already exists. Let's remove it.";
    wg_free_device(device);

    if (wg_del_device(WG_INTERFACE) != 0) {
      logger.log() << "Failed to remove the device.";
      return false;
    }
  }

  return true;
}

QString DBusService::version() {
  logger.log() << "Version request";
  return PROTOCOL_VERSION;
}

bool DBusService::activate(const QString& jsonConfig) {
  logger.log() << "Activate";

  if (!PolkitHelper::instance()->checkAuthorization(
          "org.mozilla.vpn.activate")) {
    logger.log() << "Polkit rejected";
    return false;
  }

  QJsonDocument json = QJsonDocument::fromJson(jsonConfig.toLocal8Bit());
  if (!json.isObject()) {
    logger.log() << "Invalid input";
    return false;
  }

  QJsonObject obj = json.object();

  Config config;
  if (!parseConfig(obj, config)) {
    logger.log() << "Invalid configuration";
    return false;
  }

  return Daemon::activate(config);
}

bool DBusService::deactivate(bool emitSignals) {
  logger.log() << "Deactivate";
  return Daemon::deactivate(emitSignals);
}

QString DBusService::status() { return QString(getStatus()); }

QByteArray DBusService::getStatus() {
  logger.log() << "Status request";

  QJsonObject json;

  wg_device* device = nullptr;
  // TODO - Replace with helper methods
  if (wg_get_device(&device, WG_INTERFACE) != 0) {
    logger.log() << "Unable to get device";
    json.insert("status", QJsonValue(false));
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
  }

  uint64_t txBytes = 0;
  uint64_t rxBytes = 0;

  wg_peer* peer;
  wg_for_each_peer(device, peer) {
    txBytes += peer->tx_bytes;
    rxBytes += peer->rx_bytes;
  }

  wg_free_device(device);

  json.insert("status", QJsonValue(true));
  json.insert("serverIpv4Gateway",
              QJsonValue(m_lastConfig.m_serverIpv4Gateway));
  json.insert("txBytes", QJsonValue(double(txBytes)));
  json.insert("rxBytes", QJsonValue(double(rxBytes)));

  return QJsonDocument(json).toJson(QJsonDocument::Compact);
}

QString DBusService::getLogs() {
  logger.log() << "Log request";
  return Daemon::logs();
}

bool DBusService::run(Op op, const Config& config) {
  // WG Up commands
  if (op == Up) {
    // We could call checkInterface instead of this check.
    if (WireguardHelper::deviceExists()) {
      qWarning("Interface already exists.");
      // ToDo - do we want to try and do clean-up here?
      return false;
    }
    // add_if
    if (!WireguardHelper::addDevice()) {
      return false;
    }
    // set conf
    if (!WireguardHelper::configureDevice(config)) {
      return false;
    }
    // add device ids
    if (!WireguardHelper::addDeviceIps(config)) {
      return false;
    }
  }

  // WG Down commands
  if (op == Down) {
    if (!WireguardHelper::deviceExists()) {
      qWarning("Wireguard interface `%s` does not exist. Cannot proceed.",
               WG_INTERFACE);
      return false;
    }
  }

  bool run_status =
      WgQuickProcess::run(op, config.m_deviceIpv4Address,
                          config.m_deviceIpv6Address, config.m_ipv6Enabled);

  if (op == Down && run_status == true) {
    if (!WireguardHelper::delDevice()) {
      return false;
    }
  }

  return run_status;
}

bool DBusService::switchServer(const Config& config) {
  logger.log() << "Switching server";
  return WireguardHelper::configureDevice(config);
}

bool DBusService::supportServerSwitching(const Config& config) const {
  return m_lastConfig.m_deviceIpv4Address == config.m_deviceIpv4Address &&
         m_lastConfig.m_deviceIpv6Address == config.m_deviceIpv6Address &&
         m_lastConfig.m_serverIpv4Gateway == config.m_serverIpv4Gateway &&
         m_lastConfig.m_serverIpv6Gateway == config.m_serverIpv6Gateway;
}
