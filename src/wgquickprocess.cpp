/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "wgquickprocess.h"
#include "../../src/logger.h"

#include <QCoreApplication>
#include <QTemporaryDir>
#include <QProcess>

namespace {
Logger logger(
#if defined(MVPN_LINUX)
    LOG_LINUX
#elif defined(MVPN_MACOS_DAEMON)
    LOG_MACOS
#elif defined(MVPN_WINDOWS)
    LOG_WINDOWS
#endif
    ,
    "WgQuickProcess");

QString scriptPath() {
#if defined(MVPN_LINUX)
  QDir appPath(MVPN_DATA_PATH);
  return appPath.filePath("helper.sh");
#elif defined(MVPN_MACOS_DAEMON)
  QDir appPath(QCoreApplication::applicationDirPath());
  appPath.cdUp();
  appPath.cd("Resources");
  appPath.cd("utils");
  return appPath.filePath("helper.sh");
#endif
  return QString();
}

}  // namespace

// static
bool WgQuickProcess::validateWgArgs(const QString& serverIpv4Gateway,
                                    const QString& serverIpv6Gateway) {
  // We may want to do more advanced validation in the future

#define VALIDATE(x) \
  if (x.contains("\n")) return false;
  VALIDATE(serverIpv4Gateway);
  VALIDATE(serverIpv6Gateway);
#undef VALIDATE

  return true;
}

QString WgQuickProcess::writeWgConfigFile(QTemporaryDir& tmpDir,
                                          const QString& serverIpv4Gateway,
                                          const QString& serverIpv6Gateway,
                                          bool ipv6Enabled) {
  QByteArray content;
  content.append("[Interface]\nDNS = ");
  content.append(serverIpv4Gateway.toUtf8());

  if (ipv6Enabled) {
    content.append(", ");
    content.append(serverIpv6Gateway.toUtf8());
  }

#ifdef QT_DEBUG
  logger.log() << content;
#endif

  QDir dir(tmpDir.path());
  QFile file(dir.filePath(QString("%1.conf").arg(WG_INTERFACE)));

  if (!file.open(QIODevice::WriteOnly)) {
    qWarning("Unable to create a file in the temporary folder");
    return QString();
  }

  qint64 written = file.write(content);

  if (written != content.length()) {
    qWarning("Unable to write the whole configuration file");
    return QString();
  }

  return file.fileName();
}

// static
bool WgQuickProcess::run(Daemon::Op op, const QString& serverIpv4Gateway,
                         const QString& serverIpv6Gateway, bool ipv6Enabled) {
  // Validate
  if (!validateWgArgs(serverIpv4Gateway, serverIpv6Gateway)) {
    qWarning("Config arguments are not valid.");
    return false;
  }

  // Make config file
  QTemporaryDir tmpDir;
  if (!tmpDir.isValid()) {
    qWarning("Cannot create a temporary directory");
    return false;
  }
  QString configFile = writeWgConfigFile(tmpDir, serverIpv4Gateway,
                                         serverIpv6Gateway, ipv6Enabled);
  if (configFile.isEmpty()) {
    logger.log() << "Failed to create the config file";
    return false;
  }

  // Run
  QStringList arguments;
  arguments.append(op == Daemon::Up ? "up" : "down");
  arguments.append(configFile);

  QString app = scriptPath();
  logger.log() << "Start:" << app << " - arguments:" << arguments;

  QProcess wgQuickProcess;
  wgQuickProcess.start(app, arguments);

  if (!wgQuickProcess.waitForFinished(-1)) {
    logger.log() << "Error occurred:" << wgQuickProcess.errorString();
    return false;
  }

  logger.log() << "Execution finished" << wgQuickProcess.exitCode();

  logger.log() << "wg-quick stdout:" << Qt::endl
               << qUtf8Printable(wgQuickProcess.readAllStandardOutput())
               << Qt::endl;
  logger.log() << "wg-quick stderr:" << Qt::endl
               << qUtf8Printable(wgQuickProcess.readAllStandardError())
               << Qt::endl;

  return wgQuickProcess.exitCode() == 0;
}
