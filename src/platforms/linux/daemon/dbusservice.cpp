/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dbusservice.h"
#include "dbus_adaptor.h"
#include "leakdetector.h"
#include "logger.h"
#include "loghandler.h"
#include "polkithelper.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
Logger logger(LOG_LINUX, "DBusService");
}

DBusService::DBusService(QObject* parent) : Daemon(parent) {
  MVPN_COUNT_CTOR(DBusService);
  if (!removeInterfaceIfExists()) {
    qFatal("Interface `%s` exists and cannot be removed. Cannot proceed!",
           WG_INTERFACE);
  }
}

DBusService::~DBusService() { MVPN_COUNT_DTOR(DBusService); }

WireguardUtils* DBusService::wgutils() {
  if (!m_wgutils) {
    m_wgutils = new WireguardUtilsLinux(this);
  }
  return m_wgutils;
}

IPUtils* DBusService::iputils() {
  if (!m_iputils) {
    m_iputils = new IPUtilsLinux(this);
  }
  return m_iputils;
}

DnsUtils* DBusService::dnsutils() {
  if (!m_dnsutils) {
    m_dnsutils = new DnsUtilsLinux(this);
  }
  return m_dnsutils;
}

QString DBusService::interfaceName(int hopindex) {
  if (hopindex == 0) {
    return WG_INTERFACE;
  } else {
    return QString("mozhop%1").arg(hopindex);
  }
}

void DBusService::setAdaptor(DbusAdaptor* adaptor) {
  Q_ASSERT(!m_adaptor);
  m_adaptor = adaptor;
}

bool DBusService::removeInterfaceIfExists() {
  if (wgutils()->interfaceExists(WG_INTERFACE)) {
    logger.log() << "Device already exists. Let's remove it.";
    if (!wgutils()->deleteInterface(WG_INTERFACE)) {
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

  InterfaceConfig config;
  if (!parseConfig(obj, config)) {
    logger.log() << "Invalid configuration";
    return false;
  }

  return Daemon::activate(config);
}

bool DBusService::deactivate(int hopindex, bool emitSignals) {
  logger.log() << "Deactivate";
  return Daemon::deactivate(hopindex, emitSignals);
}

QString DBusService::status() { return QString(getStatus()); }

QByteArray DBusService::getStatus() {
  logger.log() << "Status request";
  QJsonObject json;
  if (!m_connections.contains(0)) {
    json.insert("status", QJsonValue(false));
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
  }

  const InterfaceConfig& config = m_connections.value(0).m_config;
  if (!wgutils()->interfaceExists(config.m_ifname)) {
    logger.log() << "Unable to get device";
    json.insert("status", QJsonValue(false));
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
  }

  json.insert("status", QJsonValue(true));
  json.insert("serverIpv4Gateway", QJsonValue(config.m_serverIpv4Gateway));
  json.insert("deviceIpv4Address", QJsonValue(config.m_deviceIpv4Address));
  WireguardUtilsLinux::peerBytes pb =
      wgutils()->getThroughputForInterface(config.m_ifname);
  json.insert("txBytes", QJsonValue(pb.txBytes));
  json.insert("rxBytes", QJsonValue(pb.rxBytes));

  return QJsonDocument(json).toJson(QJsonDocument::Compact);
}

QString DBusService::getLogs() {
  logger.log() << "Log request";
  return Daemon::logs();
}

bool DBusService::switchServer(const InterfaceConfig& config) {
  QString ifname = interfaceName(config.m_hopindex);
  logger.log() << "Switching server for" << ifname;
  return wgutils()->configureInterface(config);
}

bool DBusService::supportServerSwitching(const InterfaceConfig& config) const {
  if (!m_connections.contains(config.m_hopindex)) {
    return false;
  }
  const InterfaceConfig& current =
      m_connections.value(config.m_hopindex).m_config;

  return current.m_privateKey == config.m_privateKey &&
         current.m_deviceIpv4Address == config.m_deviceIpv4Address &&
         current.m_deviceIpv6Address == config.m_deviceIpv6Address &&
         current.m_serverIpv4Gateway == config.m_serverIpv4Gateway &&
         current.m_serverIpv6Gateway == config.m_serverIpv6Gateway;
}
