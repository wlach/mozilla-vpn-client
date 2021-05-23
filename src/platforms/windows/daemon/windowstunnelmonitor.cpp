/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "windowstunnelmonitor.h"
#include "leakdetector.h"
#include "logger.h"
#include "platforms/windows/windowscommons.h"
#include "windowsdaemon.h"

#include <QScopeGuard>

#include <Windows.h>

constexpr uint32_t WINDOWS_TUNNEL_MONITOR_TIMEOUT_MSEC = 2000;

namespace {
Logger logger(LOG_WINDOWS, "WindowsTunnelMonitor");
static SC_HANDLE s_scm = nullptr;

QString exitCodeToFailure(DWORD exitCode) {
  // The order of this error code is taken from wireguard.
  switch (exitCode) {
    case 0:
      return "No error";
    case 1:
      return "Error when opening the ringlogger log file";
    case 2:
      return "Error while loading the WireGuard configuration file from "
             "path.";
    case 3:
      return "Error while creating a WinTun device.";
    case 4:
      return "Error while listening on a named pipe.";
    case 5:
      return "Error while resolving DNS hostname endpoints.";
    case 6:
      return "Error while manipulating firewall rules.";
    case 7:
      return "Error while setting the device configuration.";
    case 8:
      return "Error while binding sockets to default routes.";
    case 9:
      return "Unable to set interface addresses, routes, dns, and/or "
             "interface settings.";
    case 10:
      return "Error while determining current executable path.";
    case 11:
      return "Error while opening the NUL file.";
    case 12:
      return "Error while attempting to track tunnels.";
    case 13:
      return "Error while attempting to enumerate current sessions.";
    case 14:
      return "Error while dropping privileges.";
    case 15:
      return "Windows internal error.";
    default:
      return "Unknown error";
  }
}

}  // namespace

WindowsTunnelMonitor::WindowsTunnelMonitor(const QString& serviceName,
                                           const QString& displayName,
                                           QObject* parent) :
                                           m_serviceName(serviceName),
                                           m_displayName(displayName),
                                           QObject(parent) {
  MVPN_COUNT_CTOR(WindowsTunnelMonitor);
  connect(&m_timer, &QTimer::timeout, this, &WindowsTunnelMonitor::timeout);
}

WindowsTunnelMonitor::~WindowsTunnelMonitor() {
  MVPN_COUNT_CTOR(WindowsTunnelMonitor);
  if (m_service) {
    CloseServiceHandle(m_service);
  }
}

bool WindowsTunnelMonitor::start(const QString& servicePath) {
  logger.log() << "Starting tunnel service:" << m_serviceName;
  
  auto failguard = qScopeGuard([&] {
    if (m_service) {
      CloseServiceHandle(m_service);
      m_service = nullptr;
    }
  });

  const wchar_t* serviceName = (const wchar_t*)m_serviceName.utf16();
  const wchar_t* displayName = (const wchar_t*)m_displayName.utf16();
  m_service = OpenService(scm(), serviceName, SERVICE_ALL_ACCESS);
  if (m_service) {
    logger.log() << "An existing service has been detected. Let's close it.";
    if (!terminate()) {
      return false;
    }
  }

  m_service = CreateService(scm(), serviceName, displayName,
                            SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                            SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
                            (const wchar_t*)servicePath.utf16(), nullptr, 0,
                            TEXT("Nsi\0TcpIp\0"), nullptr, nullptr);
  if (!m_service) {
    WindowsCommons::windowsLog("Failed to create the tunnel service");
    return false;
  }

  SERVICE_DESCRIPTION sd = {
      (wchar_t*)L"Manages the Mozilla VPN tunnel connection"};

  if (!ChangeServiceConfig2(m_service, SERVICE_CONFIG_DESCRIPTION, &sd)) {
    WindowsCommons::windowsLog(
        "Failed to set the description to the tunnel service");
    return false;
  }

  SERVICE_SID_INFO ssi;
  ssi.dwServiceSidType = SERVICE_SID_TYPE_UNRESTRICTED;
  if (!ChangeServiceConfig2(m_service, SERVICE_CONFIG_SERVICE_SID_INFO, &ssi)) {
    WindowsCommons::windowsLog("Failed to set the SID to the tunnel service");
    return false;
  }

  if (!StartService(m_service, 0, nullptr)) {
    WindowsCommons::windowsLog("Failed to start the service");
    return false;
  }

  if (!waitForStatus(SERVICE_RUNNING)) {
    logger.log() << "Failed to run the tunnel service";
    SERVICE_STATUS status;
    if (!QueryServiceStatus(m_service, &status)) {
      WindowsCommons::windowsLog("Failed to retrieve the service status");
      return false;
    }

    logger.log() << "The tunnel service exits with status code:"
                << status.dwWin32ExitCode << "-"
                << exitCodeToFailure(status.dwWin32ExitCode);
    return false;
  }

  logger.log() << "The tunnel service is up and running";
  m_timer.start(WINDOWS_TUNNEL_MONITOR_TIMEOUT_MSEC);
  failguard.dismiss();
  return true;
}

bool WindowsTunnelMonitor::terminate() {
  logger.log() << "Terminating the tunnel service:" << m_serviceName;

  if (!m_service) {
    logger.log() << "Service is not opened.";
    return false;
  }

  SERVICE_STATUS status;
  if (!QueryServiceStatus(m_service, &status)) {
    WindowsCommons::windowsLog("Failed to retrieve the service status");
    return false;
  }

  logger.log() << "The current service is stopped:"
               << (status.dwCurrentState == SERVICE_STOPPED);

  if (status.dwCurrentState != SERVICE_STOPPED) {
    logger.log() << "The service is not stopped yet.";
    if (!ControlService(m_service, SERVICE_CONTROL_STOP, &status)) {
      WindowsCommons::windowsLog("Failed to control the service");
      return false;
    }

    if (!waitForStatus(SERVICE_STOPPED)) {
      logger.log() << "Unable to stop the service";
      return false;
    }
  }

  logger.log() << "Proceeding with the deletion";

  if (!DeleteService(m_service)) {
    WindowsCommons::windowsLog("Failed to delete the service");
    return false;
  }
  CloseServiceHandle(m_service);
  m_service = nullptr;

  m_timer.stop();
  return true;
}

bool WindowsTunnelMonitor::waitForStatus(DWORD expectedStatus) {
  Q_ASSERT(m_service);
  int tries = 0;
  while (tries < 30) {
    SERVICE_STATUS status;
    if (!QueryServiceStatus(m_service, &status)) {
      WindowsCommons::windowsLog("Failed to retrieve the service status");
      return false;
    }

    if (status.dwCurrentState == expectedStatus) {
      return true;
    }

    logger.log() << "The service is not in the right status yet:" << status.dwCurrentState;

    Sleep(1000);
    ++tries;
  }

  return false;
}

void WindowsTunnelMonitor::timeout() {
  Q_ASSERT(m_service);

  SERVICE_STATUS status;
  if (!QueryServiceStatus(m_service, &status)) {
    WindowsCommons::windowsLog("Failed to retrieve the service status");
    emit backendFailure();
    return;
  }

  if (status.dwCurrentState == SERVICE_RUNNING) {
    // The service is active
    return;
  }

  logger.log() << "Tunnel service" << m_serviceName << "is not active";
  emit backendFailure();
  CloseServiceHandle(m_service);
  m_service = nullptr;
}

SC_HANDLE WindowsTunnelMonitor::scm() {
  if (s_scm) {
    return s_scm;
  }
  s_scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
  if (!s_scm) {
    WindowsCommons::windowsLog("Failed to open SCManager");
  }
  return s_scm;
}