/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "windowsdaemon.h"
#include "leakdetector.h"
#include "logger.h"
#include "platforms/windows/windowscommons.h"
#include "wgquickprocess.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLocalSocket>
#include <QScopeGuard>
#include <QTextStream>
#include <QtGlobal>

#include <Windows.h>

#define TUNNEL_NAMED_PIPE \
  "\\\\."                 \
  "\\pipe\\ProtectedPrefix\\Administrators\\WireGuard\\MozillaVPN"

namespace {

Logger logger(LOG_WINDOWS, "WindowsDaemon");

HANDLE createPipe() {
  HANDLE pipe = INVALID_HANDLE_VALUE;

  auto guard = qScopeGuard([&] {
    if (pipe != INVALID_HANDLE_VALUE) {
      CloseHandle(pipe);
    }
  });

  LPTSTR tunnelName = (LPTSTR)TEXT(TUNNEL_NAMED_PIPE);

  uint32_t tries = 0;
  while (tries < 30) {
    pipe = CreateFile(tunnelName, GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                      OPEN_EXISTING, 0, nullptr);

    if (pipe != INVALID_HANDLE_VALUE) {
      break;
    }

    if (GetLastError() != ERROR_PIPE_BUSY) {
      WindowsCommons::windowsLog("Failed to create a named pipe");
      return INVALID_HANDLE_VALUE;
    }

    logger.log() << "Pipes are busy. Let's wait";

    if (!WaitNamedPipe(tunnelName, 1000)) {
      WindowsCommons::windowsLog("Failed to wait for named pipes");
      return INVALID_HANDLE_VALUE;
    }

    ++tries;
  }

  DWORD mode = PIPE_READMODE_BYTE;
  if (!SetNamedPipeHandleState(pipe, &mode, nullptr, nullptr)) {
    WindowsCommons::windowsLog("Failed to set the read-mode on pipe");
    return INVALID_HANDLE_VALUE;
  }

  guard.dismiss();
  return pipe;
}

}  // namespace

WindowsDaemon::WindowsDaemon() : Daemon(nullptr) {
  MVPN_COUNT_CTOR(WindowsDaemon);
}

WindowsDaemon::~WindowsDaemon() {
  MVPN_COUNT_DTOR(WindowsDaemon);

  logger.log() << "Daemon released";

  for (int hopindex : m_tunnelMonitors.keys()) {
    WindowsTunnelMonitor* monitor = m_tunnelMonitors.value(hopindex);
    monitor->terminate();
  }
}

QByteArray WindowsDaemon::getStatus() {
  logger.log() << "Status request";

  bool status = m_connections.contains(0);
  QJsonObject obj;
  obj.insert("type", "status");
  obj.insert("connected", status);

  HANDLE pipe = createPipe();

  auto guard = qScopeGuard([&] {
    if (pipe != INVALID_HANDLE_VALUE) {
      CloseHandle(pipe);
    }
  });

  if (pipe == INVALID_HANDLE_VALUE || !status) {
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
  }

  QByteArray message = "get=1\n\n";
  DWORD written;
  const ConnectionState& state = m_connections.value(0);

  if (!WriteFile(pipe, message.constData(), message.length(), &written,
                 nullptr)) {
    WindowsCommons::windowsLog("Failed to write into the pipe");
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
  }

  QByteArray data;
  while (true) {
    char buffer[512];
    DWORD read = 0;
    if (!ReadFile(pipe, buffer, sizeof(buffer), &read, nullptr)) {
      break;
    }

    data.append(buffer, read);
  }

  uint64_t txBytes = 0;
  uint64_t rxBytes = 0;

  int steps = 0;
  constexpr const int TxFound = 0x01;
  constexpr const int RxFound = 0x02;

  for (const QByteArray& line : data.split('\n')) {
    if (!line.contains('=')) {
      continue;
    }

    QList<QByteArray> parts = line.split('=');
    if (parts[0] == "tx_bytes") {
      txBytes = parts[1].toLongLong();
      steps |= TxFound;
    } else if (parts[0] == "rx_bytes") {
      rxBytes = parts[1].toLongLong();
      steps |= RxFound;
    }

    if (steps >= (TxFound | RxFound)) {
      break;
    }
  }

  obj.insert("status", true);
  obj.insert("serverIpv4Gateway", state.m_config.m_serverIpv4Gateway);
  obj.insert("deviceIpv4Address", state.m_config.m_deviceIpv4Address);
  obj.insert("date", state.m_date.toString());
  obj.insert("txBytes", QJsonValue(double(txBytes)));
  obj.insert("rxBytes", QJsonValue(double(rxBytes)));

  return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

bool WindowsDaemon::run(Daemon::Op op, const InterfaceConfig& config) {
  logger.log() << (op == Daemon::Up ? "Activate" : "Deactivate") << "the vpn";

  WindowsTunnelMonitor* monitor = m_tunnelMonitors.value(config.m_hopindex);
  if (op == Daemon::Down) {
    if (!monitor) {
      logger.log() << "Nothing to do. The tunnel service is down";
      return true;
    }
    monitor->terminate();
    m_tunnelMonitors.remove(config.m_hopindex);

    delete monitor;
    return true;
  }

  Q_ASSERT(op == Daemon::Up);

  if (monitor) {
    logger.log() << "Nothing to do. The tunnel service is up";
    return true;
  }

  QString configFile = WindowsCommons::tunnelConfigFile(config.m_ifname);
  if (configFile.isEmpty()) {
    logger.log() << "Failed to choose the tunnel config file";
    return false;
  }

  QStringList addresses;
  for (const IPAddressRange& ip : config.m_allowedIPAddressRanges) {
    addresses.append(ip.toString());
  }

  if (!WgQuickProcess::createConfigFile(
          configFile, config.m_hopindex, config.m_privateKey,
          config.m_deviceIpv4Address, config.m_deviceIpv6Address,
          config.m_serverIpv4Gateway, config.m_serverIpv6Gateway,
          config.m_serverPublicKey, config.m_serverIpv4AddrIn,
          config.m_serverIpv6AddrIn, addresses.join(", "),
          config.m_serverPort, config.m_ipv6Enabled)) {
    logger.log() << "Failed to create a config file";
    return false;
  }

  QString serviceName = QString("WireGuardTunnel$%1").arg(config.m_ifname);
  QString displayName = "MozillaVPN (tunnel)";
  if (config.m_hopindex > 0) {
    displayName = QString("MozillaVPN (hop %1)").arg(config.m_hopindex);
  }
  monitor = new WindowsTunnelMonitor(serviceName, displayName, this);

  QString servicePath;
  {
    QTextStream out(&servicePath);
    out << "\"" << qApp->applicationFilePath() << "\" tunneldaemon \""
        << configFile << "\"";
  }
  if (!monitor->start(servicePath)) {
    logger.log() << "Failed to activate the tunnel service";
    delete monitor;
    emit backendFailure();
    return false;
  }
  m_tunnelMonitors[config.m_hopindex] = monitor;
  connect(monitor, &WindowsTunnelMonitor::backendFailure, this,
          &WindowsDaemon::monitorBackendFailure);

  logger.log() << "Registration completed";
  return true;
}

bool WindowsDaemon::supportServerSwitching(
    const InterfaceConfig& config) const {
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

bool WindowsDaemon::switchServer(const InterfaceConfig& config) {
  logger.log() << "Switching server";

  Q_ASSERT(m_connections.contains(config.m_hopindex));

  HANDLE pipe = createPipe();

  auto guard = qScopeGuard([&] {
    if (pipe != INVALID_HANDLE_VALUE) {
      CloseHandle(pipe);
    }
  });

  if (pipe == INVALID_HANDLE_VALUE) {
    return false;
  }

  QByteArray message;
  {
    QTextStream out(&message);
    out << "set=1\n";
    out << "replace_peers=true\n";

    QByteArray publicKey =
        QByteArray::fromBase64(config.m_serverPublicKey.toLocal8Bit()).toHex();
    out << "public_key=" << publicKey << "\n";

    out << "endpoint=" << config.m_serverIpv4AddrIn << ":"
        << config.m_serverPort << "\n";

    for (const IPAddressRange& ip : config.m_allowedIPAddressRanges) {
      out << "allowed_ip=" << ip.toString() << "\n";
    }

    out << "\n";
  }

  DWORD written;
  if (!WriteFile(pipe, message.constData(), message.length(), &written,
                 nullptr)) {
    WindowsCommons::windowsLog("Failed to write into the pipe");
    return false;
  }

  QByteArray data;
  while (true) {
    char buffer[512];
    DWORD read = 0;
    if (!ReadFile(pipe, buffer, sizeof(buffer), &read, nullptr)) {
      break;
    }

    data.append(buffer, read);
  }

  logger.log() << "DATA:" << data;
  guard.dismiss();
  return true;
}

void WindowsDaemon::monitorBackendFailure() {
  logger.log() << "Tunnel service is down";

  for (int hopindex : m_tunnelMonitors.keys()) {
    WindowsTunnelMonitor* monitor = m_tunnelMonitors.value(hopindex);
    m_tunnelMonitors.remove(hopindex);
    monitor->terminate();
    delete monitor;
  }

  emit backendFailure();
  deactivateAll();
}

QString WindowsDaemon::interfaceName(int hopindex) {
  if (hopindex == 0) {
    return "MozillaVPN";
  } else {
    return QString("MozillaVPN-hop%1").arg(hopindex);
  }
}