/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WINDOWSDAEMON_H
#define WINDOWSDAEMON_H

#include "daemon/daemon.h"
#include "windowstunnelmonitor.h"

class WindowsDaemon final : public Daemon {
  Q_DISABLE_COPY_MOVE(WindowsDaemon)

 public:
  WindowsDaemon();
  ~WindowsDaemon();

  QByteArray getStatus() override;

 private:
  bool run(Op op, const InterfaceConfig& config) override;

  bool supportServerSwitching(const InterfaceConfig& config) const override;

  bool switchServer(const InterfaceConfig& config) override;

  QString interfaceName(int hopindex) override;

 private:
  void monitorBackendFailure();

 private:
  QMap<int, WindowsTunnelMonitor*> m_tunnelMonitors;
};

#endif  // WINDOWSDAEMON_H
