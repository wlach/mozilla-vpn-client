/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "localsocketcontroller.h"
#include "errorhandler.h"
#include "ipaddressrange.h"
#include "leakdetector.h"
#include "logger.h"
#include "models/device.h"
#include "models/keys.h"
#include "models/server.h"
#include "mozillavpn.h"
#include "settingsholder.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>

namespace {
Logger logger(LOG_CONTROLLER, "LocalSocketController");
}

LocalSocketController::LocalSocketController() {
  MVPN_COUNT_CTOR(LocalSocketController);

  m_socket = new QLocalSocket(this);
  connect(m_socket, &QLocalSocket::connected, this,
          &LocalSocketController::daemonConnected);
  connect(m_socket, &QLocalSocket::disconnected, this,
          &LocalSocketController::disconnected);
  connect(m_socket, &QLocalSocket::errorOccurred, this,
          &LocalSocketController::errorOccurred);
  connect(m_socket, &QLocalSocket::readyRead, this,
          &LocalSocketController::readData);
}

LocalSocketController::~LocalSocketController() {
  MVPN_COUNT_DTOR(LocalSocketController);
}

void LocalSocketController::errorOccurred(
    QLocalSocket::LocalSocketError error) {
  logger.log() << "Error occurred:" << error;

  if (m_state == eInitializing) {
    emit initialized(false, false, QDateTime());
  }

  m_state = eDisconnected;
  MozillaVPN::instance()->errorHandle(ErrorHandler::ControllerError);
  emit disconnected();
}

void LocalSocketController::initialize(const Device* device, const Keys* keys) {
  logger.log() << "Initializing";

  Q_UNUSED(device);
  Q_UNUSED(keys);

  Q_ASSERT(m_state == eUnknown);
  m_state = eInitializing;

#ifdef MVPN_WINDOWS
  QString path = "\\\\.\\pipe\\mozillavpn";
#else
  QString path = "/var/run/mozillavpn/daemon.socket";
  if (!QFileInfo::exists(path)) {
    path = "/tmp/mozillavpn.socket";
  }
#endif

  logger.log() << "Connecting to:" << path;
  m_socket->connectToServer(path);
}

void LocalSocketController::daemonConnected() {
  logger.log() << "Daemon connected";
  Q_ASSERT(m_state == eInitializing);
  checkStatus();
}

void LocalSocketController::activate(
    const QList<Server>& serverList, const Device* device, const Keys* keys,
    const QList<IPAddressRange>& allowedIPAddressRanges,
    const QList<QString>& vpnDisabledApps, Reason reason) {
  Q_UNUSED(vpnDisabledApps);
  Q_UNUSED(reason);

  if (m_state != eReady) {
    emit disconnected();
    return;
  }

  m_hopcount = serverList.count();
  for (int hopindex = m_hopcount - 1; hopindex > 0; hopindex--) {
    QJsonObject hopJson;
    const Server& hop = serverList[hopindex];
    const Server& next = serverList[hopindex - 1];
    hopJson.insert("type", "activate");
    hopJson.insert("hopindex", QJsonValue((double)hopindex));
    hopJson.insert("privateKey", QJsonValue(keys->privateKey()));
    hopJson.insert("serverIpv4Gateway", QJsonValue(hop.ipv4Gateway()));
    hopJson.insert("serverIpv6Gateway", QJsonValue(hop.ipv6Gateway()));
    hopJson.insert("serverPublicKey", QJsonValue(hop.publicKey()));
    hopJson.insert("serverIpv4AddrIn", QJsonValue(hop.ipv4AddrIn()));
    hopJson.insert("serverIpv6AddrIn", QJsonValue(hop.ipv6AddrIn()));
    hopJson.insert("serverPort", QJsonValue((double)hop.choosePort()));
    hopJson.insert("ipv6Enabled",
                   QJsonValue(SettingsHolder::instance()->ipv6Enabled()));

    // FIXME: Windows requires distinct IP addresses on each tunnel
    // interface, but at the same time the API to mullvad only provides
    // us with a single device address to use. How to resolve?
#if 1
    hopJson.insert("deviceIpv4Address", QString("10.64.255.%1/32").arg(hopindex));
    hopJson.insert("deviceIpv6Address", QString("fc00:bbbb:bbbb:bb01::%1:beef/128").arg(hopindex));
#else
    hopJson.insert("deviceIpv4Address", QJsonValue(device->ipv4Address()));
    hopJson.insert("deviceIpv6Address", QJsonValue(device->ipv6Address()));
#endif

    QJsonArray hopIPAddesses;
    hopIPAddesses.append(QJsonObject({{"address", hop.ipv4Gateway()},
      {"range", 32}, {"isIpv6", false}}));
    hopIPAddesses.append(QJsonObject({{"address", hop.ipv6Gateway()},
      {"range", 128}, {"isIpv6", true}}));
    hopIPAddesses.append(QJsonObject({{"address", next.ipv4AddrIn()},
      {"range", 32}, {"isIpv6", false}}));
    hopIPAddesses.append(QJsonObject({{"address", next.ipv6AddrIn()},
      {"range", 128}, {"isIpv6", true}}));
    hopJson.insert("allowedIPAddressRanges", hopIPAddesses);
    write(hopJson);
  }

  const Server& server = serverList[0];
  QJsonObject json;
  json.insert("type", "activate");
  json.insert("hopindex", QJsonValue((double)0));
  json.insert("privateKey", QJsonValue(keys->privateKey()));
  json.insert("deviceIpv4Address", QJsonValue(device->ipv4Address()));
  json.insert("deviceIpv6Address", QJsonValue(device->ipv6Address()));
  json.insert("serverIpv4Gateway", QJsonValue(server.ipv4Gateway()));
  json.insert("serverIpv6Gateway", QJsonValue(server.ipv6Gateway()));
  json.insert("serverPublicKey", QJsonValue(server.publicKey()));
  json.insert("serverIpv4AddrIn", QJsonValue(server.ipv4AddrIn()));
  json.insert("serverIpv6AddrIn", QJsonValue(server.ipv6AddrIn()));
  json.insert("serverPort", QJsonValue((double)server.choosePort()));
  json.insert("ipv6Enabled",
              QJsonValue(SettingsHolder::instance()->ipv6Enabled()));

  QJsonArray allowedIPAddesses;
  for (const IPAddressRange& i : allowedIPAddressRanges) {
    QJsonObject range;
    range.insert("address", QJsonValue(i.ipAddress()));
    range.insert("range", QJsonValue((double)i.range()));
    range.insert("isIpv6", QJsonValue(i.type() == IPAddressRange::IPv6));
    allowedIPAddesses.append(range);
  };
  json.insert("allowedIPAddressRanges", allowedIPAddesses);
  write(json);
}

void LocalSocketController::deactivate(Reason reason) {
  logger.log() << "Deactivating";

  if (m_state != eReady) {
    emit disconnected();
    return;
  }

  if (reason == ReasonSwitching) {
    logger.log() << "No disconnect for quick server switching";
    emit disconnected();
    return;
  }

  for (int hopindex = 0; hopindex < m_hopcount; hopindex++) {
    QJsonObject json;
    json.insert("type", "deactivate");
    json.insert("hopindex", hopindex);
    write(json);
  }
  m_hopcount = 0;
}

void LocalSocketController::checkStatus() {
  logger.log() << "Check status";

  if (m_state == eReady || m_state == eInitializing) {
    Q_ASSERT(m_socket);

    QJsonObject json;
    json.insert("type", "status");
    write(json);
  }
}

void LocalSocketController::getBackendLogs(
    std::function<void(const QString&)>&& a_callback) {
  logger.log() << "Backend logs";

  if (m_logCallback) {
    m_logCallback("");
    m_logCallback = nullptr;
  }

  if (m_state != eReady) {
    std::function<void(const QString&)> callback = a_callback;
    callback("");
    return;
  }

  m_logCallback = std::move(a_callback);

  QJsonObject json;
  json.insert("type", "logs");
  write(json);
}

void LocalSocketController::cleanupBackendLogs() {
  logger.log() << "Cleanup logs";

  if (m_logCallback) {
    m_logCallback("");
    m_logCallback = nullptr;
  }

  if (m_state != eReady) {
    return;
  }

  QJsonObject json;
  json.insert("type", "cleanlogs");
  write(json);
}

void LocalSocketController::readData() {
  logger.log() << "Reading";

  Q_ASSERT(m_socket);
  Q_ASSERT(m_state == eInitializing || m_state == eReady);
  QByteArray input = m_socket->readAll();
  m_buffer.append(input);

  while (true) {
    int pos = m_buffer.indexOf("\n");
    if (pos == -1) {
      break;
    }

    QByteArray line = m_buffer.left(pos);
    m_buffer.remove(0, pos + 1);

    QByteArray command(line);
    command = command.trimmed();

    if (command.isEmpty()) {
      continue;
    }

    parseCommand(command);
  }
}

void LocalSocketController::parseCommand(const QByteArray& command) {
  logger.log() << "Parse command:" << command.left(20);

  QJsonDocument json = QJsonDocument::fromJson(command);
  if (!json.isObject()) {
    logger.log() << "Invalid JSON - object expected";
    return;
  }

  QJsonObject obj = json.object();
  QJsonValue typeValue = obj.value("type");
  if (!typeValue.isString()) {
    logger.log() << "Invalid JSON - no type";
    return;
  }

  QString type = typeValue.toString();

  if (m_state == eInitializing && type == "status") {
    m_state = eReady;

    QJsonValue connected = obj.value("connected");
    if (!connected.isBool()) {
      logger.log() << "Invalid JSON for status - connected expected";
      return;
    }

    QDateTime datetime;
    if (connected.toBool()) {
      QJsonValue date = obj.value("date");
      if (!date.isString()) {
        logger.log() << "Invalid JSON for status - date expected";
        return;
      }

      datetime = QDateTime::fromString(date.toString());
      if (!datetime.isValid()) {
        logger.log() << "Invalid JSON for status - date is invalid";
        return;
      }
    }

    emit initialized(true, connected.toBool(), datetime);
    return;
  }

  if (m_state != eReady) {
    logger.log() << "Unexpected command";
    return;
  }

  if (type == "status") {
    QJsonValue serverIpv4Gateway = obj.value("serverIpv4Gateway");
    if (!serverIpv4Gateway.isString()) {
      logger.log() << "Unexpected serverIpv4Gateway value";
      return;
    }

    QJsonValue deviceIpv4Address = obj.value("deviceIpv4Address");
    if (!deviceIpv4Address.isString()) {
      logger.log() << "Unexpected deviceIpv4Address value";
      return;
    }

    QJsonValue txBytes = obj.value("txBytes");
    if (!txBytes.isDouble()) {
      logger.log() << "Unexpected txBytes value";
      return;
    }

    QJsonValue rxBytes = obj.value("rxBytes");
    if (!rxBytes.isDouble()) {
      logger.log() << "Unexpected rxBytes value";
      return;
    }

    emit statusUpdated(serverIpv4Gateway.toString(),
                       deviceIpv4Address.toString(), txBytes.toDouble(),
                       rxBytes.toDouble());
    return;
  }

  if (type == "disconnected") {
    QJsonValue hopindex = obj.value("hopindex");
    if (!hopindex.isDouble()) {
      logger.log() << "Unexpected hopindex value";
      return;
    }
    if (hopindex.toInt() == 0) {
      emit disconnected();
    }
    return;
  }

  if (type == "connected") {
    QJsonValue hopindex = obj.value("hopindex");
    if (!hopindex.isDouble()) {
      logger.log() << "Unexpected hopindex value";
      return;
    }
    if (hopindex.toInt() == 0) {
      emit connected();
    }
    return;
  }

  if (type == "backendFailure") {
    MozillaVPN::instance()->errorHandle(ErrorHandler::ControllerError);
    return;
  }

  if (type == "logs") {
    // We don't care if we are not waiting for logs.
    if (!m_logCallback) {
      return;
    }

    QJsonValue logs = obj.value("logs");
    m_logCallback(logs.isString() ? logs.toString().replace("|", "\n")
                                  : QString());
    m_logCallback = nullptr;
    return;
  }

  logger.log() << "Invalid command received:" << command;
}

void LocalSocketController::write(const QJsonObject& json) {
  Q_ASSERT(m_socket);
  m_socket->write(QJsonDocument(json).toJson(QJsonDocument::Compact));
  m_socket->write("\n");
}
