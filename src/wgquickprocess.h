/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WGQUICKPROCESS_H
#define WGQUICKPROCESS_H

#include "daemon/daemon.h"

#include <QObject>
class QTemporaryDir;

constexpr const char* WG_INTERFACE = "moz0";

class WgQuickProcess final {
  Q_DISABLE_COPY_MOVE(WgQuickProcess)

 public:
  static bool run(Daemon::Op op, const QString& serverIpv4Gateway,
                  const QString& serverIpv6Gateway, bool ipv6Enabled);
  static bool validateWgArgs(const QString& serverIpv4Gateway,
                             const QString& serverIpv6Gateway);
  static QString writeWgConfigFile(QTemporaryDir& tmpDir,
                                   const QString& serverIpv4Gateway,
                                   const QString& serverIpv6Gateway,
                                   bool ipv6Enabled);
};

#endif  // WGQUICKPROCESS_H
