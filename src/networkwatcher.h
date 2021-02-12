/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef NETWORKWATCHER_H
#define NETWORKWATCHER_H

#include <QElapsedTimer>
#include <QMap>
#include <QObject>

class NetworkWatcherImpl;

class NetworkWatcher final : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(NetworkWatcher)

 public:
  NetworkWatcher();
  ~NetworkWatcher();

  void initialize();

 private:
  void unsecuredNetwork(const QString& networkName, const QString& networkId);

  void settingsChanged(bool active);

 private:
  bool m_active = false;

  NetworkWatcherImpl* m_impl = nullptr;

  QMap<QString, QElapsedTimer> m_networks;
};

#endif  // NETWORKWATCHER_H
