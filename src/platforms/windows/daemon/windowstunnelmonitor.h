/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WINDOWSTUNNELMONITOR_H
#define WINDOWSTUNNELMONITOR_H

#include <QObject>
#include <QTimer>

#include <Windows.h>

class WindowsTunnelMonitor final : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(WindowsTunnelMonitor)

 public:
  WindowsTunnelMonitor(const QString& serviceName, const QString& displayName,
                       QObject* parent);
  ~WindowsTunnelMonitor();

  bool start(const QString& servicePath);
  bool terminate();

 signals:
  void backendFailure();

 private:
  static SC_HANDLE scm();
  bool waitForStatus(DWORD expectedStatus);
  void timeout();

 private:
  const QString m_serviceName;
  const QString m_displayName;
  SC_HANDLE m_service = nullptr;
  QTimer m_timer;
};

#endif  // WINDOWSTUNNELMONITOR_H
