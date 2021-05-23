/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "daemonlocalserverconnection.h"
#include "daemon.h"
#include "leakdetector.h"
#include "logger.h"

#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace {
Logger logger(LOG_MAIN, "DaemonLocalServerConnection");
}

DaemonLocalServerConnection::DaemonLocalServerConnection(QObject* parent,
                                                         QLocalSocket* socket)
    : QObject(parent) {
  MVPN_COUNT_CTOR(DaemonLocalServerConnection);

  logger.log() << "Connection created";

  Q_ASSERT(socket);
  m_socket = socket;

  connect(m_socket, &QLocalSocket::readyRead, this,
          &DaemonLocalServerConnection::readData);

  Daemon* daemon = Daemon::instance();
  connect(daemon, &Daemon::connected, this,
          &DaemonLocalServerConnection::connected);
  connect(daemon, &Daemon::disconnected, this,
          &DaemonLocalServerConnection::disconnected);
  connect(daemon, &Daemon::backendFailure, this,
          &DaemonLocalServerConnection::backendFailure);
}

DaemonLocalServerConnection::~DaemonLocalServerConnection() {
  MVPN_COUNT_DTOR(DaemonLocalServerConnection);

  logger.log() << "Connection released";
}

void DaemonLocalServerConnection::readData() {
  logger.log() << "Read Data";

  Q_ASSERT(m_socket);
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

void DaemonLocalServerConnection::parseCommand(const QByteArray& data) {
  logger.log() << "Command received:" << data.left(20);

  QJsonDocument json = QJsonDocument::fromJson(data);
  if (!json.isObject()) {
    logger.log() << "Invalid input";
    return;
  }

  QJsonObject obj = json.object();
  QJsonValue typeValue = obj.value("type");
  if (!typeValue.isString()) {
    logger.log() << "No type command. Ignoring request.";
    return;
  }

  QString type = typeValue.toString();
  if (type == "activate") {
    InterfaceConfig config;
    if (!Daemon::instance()->parseConfig(obj, config)) {
      logger.log() << "Invalid configuration";
      emit disconnected(config.m_hopindex);
      return;
    }

    if (!Daemon::instance()->activate(config)) {
      logger.log() << "Failed to activate the interface";
      emit disconnected(config.m_hopindex);
    }
    return;
  }

  if (type == "deactivate") {
    QJsonValue hopValue = obj.value("hopindex");
    int hopindex = 0;
    if (hopValue.isDouble()) {
      hopindex = hopValue.toInt();
    }
    Daemon::instance()->deactivate(hopindex);
    return;
  }

  if (type == "status") {
    m_socket->write(Daemon::instance()->getStatus());
    m_socket->write("\n");
    return;
  }

  if (type == "logs") {
    QJsonObject obj;
    obj.insert("type", "logs");
    obj.insert("logs", Daemon::instance()->logs().replace("\n", "|"));
    m_socket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    m_socket->write("\n");
    return;
  }

  if (type == "cleanlogs") {
    Daemon::instance()->cleanLogs();
    return;
  }

  logger.log() << "Invalid command:" << type;
}

void DaemonLocalServerConnection::connected(int hopindex) {
  QJsonObject obj;
  obj.insert("type", "connected");
  obj.insert("hopindex", QJsonValue((double)hopindex));
  write(obj);
}

void DaemonLocalServerConnection::disconnected(int hopindex) {
  QJsonObject obj;
  obj.insert("type", "disconnected");
  obj.insert("hopindex", QJsonValue((double)hopindex));
  write(obj);
}

void DaemonLocalServerConnection::backendFailure() {
  QJsonObject obj;
  obj.insert("type", "backendFailure");
  write(obj);
}

void DaemonLocalServerConnection::write(const QJsonObject& obj) {
  m_socket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  m_socket->write("\n");
}
