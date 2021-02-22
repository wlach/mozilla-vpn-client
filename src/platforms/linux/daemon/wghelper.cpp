/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "wghelper.h"
#include "wgquickprocess.h"  // source of WG_INTERFACE

#include <QScopeGuard>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QHostAddress>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "logger.h"
#include "loghandler.h"
#include "daemon/daemon.h"

// Import wireguard C library
#if defined(__cplusplus)
extern "C" {
#endif
#include "../../3rdparty/wireguard-tools/contrib/embeddable-wg-library/wireguard.h"
#if defined(__cplusplus)
}
#endif

namespace {
Logger logger(LOG_LINUX, "WireguardHelper");
}

/*
 * HELPERS
 */

char* iface_name() { return QString(WG_INTERFACE).toLocal8Bit().data(); }

char* addrToText(struct sockaddr_in* addr) { return inet_ntoa(addr->sin_addr); }

// END HELPERS

/*
 * PRIVATE METHODS
 */

// static
bool WireguardHelper::setPeerEndpoint(struct sockaddr* peerEndpoint,
                                      const QString& address, int port) {
  QString portString = QString::number(port);

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  struct addrinfo* resolved = nullptr;
  int retries = 15;

  for (unsigned int timeout = 1000000;;
       timeout = std::min((unsigned int)20000000, timeout * 6 / 5)) {
    int rv = getaddrinfo(address.toLocal8Bit(), portString.toLocal8Bit(),
                         &hints, &resolved);
    if (!rv) {
      break;
    }

    /* The set of return codes that are "permanent failures". All other
     * possibilities are potentially transient.
     *
     * This is according to https://sourceware.org/glibc/wiki/NameResolver which
     * states: "From the perspective of the application that calls getaddrinfo()
     * it perhaps doesn't matter that much since EAI_FAIL, EAI_NONAME and
     * EAI_NODATA are all permanent failure codes and the causes are all
     * permanent failures in the sense that there is no point in retrying
     * later."
     *
     * So this is what we do, except FreeBSD removed EAI_NODATA some time ago,
     * so that's conditional.
     */
    if (rv == EAI_NONAME || rv == EAI_FAIL ||
#ifdef EAI_NODATA
        rv == EAI_NODATA ||
#endif
        (retries >= 0 && !retries--)) {
      logger.log() << "Failed to resolve the address endpoint";
      return false;
    }

    logger.log() << "Trying again in" << (timeout / 1000000.0) << "seconds";
    usleep(timeout);
  }

  if ((resolved->ai_family == AF_INET &&
       resolved->ai_addrlen == sizeof(struct sockaddr_in)) ||
      (resolved->ai_family == AF_INET6 &&
       resolved->ai_addrlen == sizeof(struct sockaddr_in6))) {
    memcpy(peerEndpoint, resolved->ai_addr, resolved->ai_addrlen);
    freeaddrinfo(resolved);
    return true;
  }

  logger.log() << "Invalid endpoint" << address;
  freeaddrinfo(resolved);
  return false;
}

// static
bool WireguardHelper::setAllowedIpsOnPeer(
    wg_peer* peer, QList<IPAddressRange> allowedIPAddressRanges) {
  for (const IPAddressRange& ip : allowedIPAddressRanges) {
    wg_allowedip* allowedip =
        static_cast<wg_allowedip*>(calloc(1, sizeof(*allowedip)));
    if (!allowedip) {
      logger.log() << "Allocation failure";
      return false;
    }

    if (!peer->first_allowedip) {
      peer->first_allowedip = allowedip;
    } else {
      peer->last_allowedip->next_allowedip = allowedip;
    }

    peer->last_allowedip = allowedip;
    allowedip->cidr = ip.range();

    bool ok = false;
    if (ip.type() == IPAddressRange::IPv4) {
      allowedip->family = AF_INET;
      ok = inet_pton(AF_INET, ip.ipAddress().toLocal8Bit(), &allowedip->ip4) ==
           1;
    } else if (ip.type() == IPAddressRange::IPv6) {
      allowedip->family = AF_INET6;
      ok = inet_pton(AF_INET6, ip.ipAddress().toLocal8Bit(), &allowedip->ip6) ==
           1;
    } else {
      logger.log() << "Invalid IPAddressRange type";
      return false;
    }
    if (!ok) {
      logger.log() << "Invalid IP address:" << ip.ipAddress();
      return false;
    }
  }

  return true;
}

//

// static
bool WireguardHelper::addIP4AddressToDevice(const Daemon::Config& config) {
  struct ifreq ifr;
  struct sockaddr_in* ifrAddr = (struct sockaddr_in*)&ifr.ifr_addr;

  // Name the interface and set family
  strncpy(ifr.ifr_name, iface_name(), IFNAMSIZ);
  ifr.ifr_addr.sa_family =
      AF_INET;  // TODO c++ question - why can't I use ifrAddr here?

  // Get the device address to add to interface
  QPair<QHostAddress, int> parsedAddr =
      QHostAddress::parseSubnet(config.m_deviceIpv4Address);
  QByteArray _deviceAddr = parsedAddr.first.toString().toLocal8Bit();
  char* deviceAddr = _deviceAddr.data();
  inet_pton(AF_INET, deviceAddr, &ifrAddr->sin_addr);
  logger.log() << addrToText(ifrAddr);

  // Create IPv4 socket to perform the ioctl operations on
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  int ret = ioctl(sockfd, SIOCSIFADDR, &ifr);
  if (ret) {
    logger.log() << "Failed to set IPv4: " << deviceAddr
                 << " -- Return code: " << ret;
    return false;
  }

  /*
   * TODO - Open question. We are not setting the netmask based on the /32
   * do we want to? Could there be other cidr values?
   */

  close(sockfd);
  // TODO c++ question - do I need to free any other objects?
  return true;
}

struct in6_ifreq {
  struct in6_addr addr;
  uint32_t prefixlen;
  unsigned int ifindex;
};

// static
bool WireguardHelper::addIP6AddressToDevice(const Daemon::Config& config) {
  // Set up the ifr and the companion ifr6
  struct in6_ifreq ifr6;
  ifr6.prefixlen = 64;

  // Get the device address to add to ifr6 interface
  QPair<QHostAddress, int> parsedAddr =
      QHostAddress::parseSubnet(config.m_deviceIpv6Address);
  QByteArray _deviceAddr = parsedAddr.first.toString().toLocal8Bit();
  char* deviceAddr = _deviceAddr.data();
  inet_pton(AF_INET6, deviceAddr, &ifr6.addr);

  // Create IPv6 socket to perform the ioctl operations on
  int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP);

  // Get the index of named ifr and link with ifr6
  struct ifreq ifr;
  strncpy(ifr.ifr_name, iface_name(), IFNAMSIZ);
  ifr.ifr_addr.sa_family = AF_INET6;
  int ret = ioctl(sockfd, SIOGIFINDEX, &ifr);
  if (ret) {
    logger.log() << "Failed to get ifrindex. Return code: " << ret;
    return false;
  }
  ifr6.ifindex = ifr.ifr_ifindex;

  // Set ifr6 to the interface
  ret = ioctl(sockfd, SIOCSIFADDR, &ifr6);
  if (ret) {
    logger.log() << "Failed to set IPv6: " << deviceAddr
                 << " -- Return code: " << ret;
    return false;
  }

  close(sockfd);
  return true;
}

wg_peer* WireguardHelper::buildPeerForDevice(wg_device* device,
                                             const Daemon::Config& config) {
  // PEER
  wg_peer* peer = static_cast<wg_peer*>(calloc(1, sizeof(*peer)));
  // TODO - If you deliberately set this to `if(peer)` to make this fail
  // then activation cycle does not behave well.
  if (!peer) {
    logger.log() << "Allocation failure";
    return nullptr;
  }
  device->first_peer = device->last_peer = peer;

  // Public Key
  wg_key_from_base64(peer->public_key, config.m_serverPublicKey.toLocal8Bit());
  // Endpoint
  if (!setPeerEndpoint(&peer->endpoint.addr, config.m_serverIpv4AddrIn,
                       config.m_serverPort)) {
    return nullptr;
  }
  if (!setAllowedIpsOnPeer(peer, config.m_allowedIPAddressRanges)) {
    logger.log() << "Failed to set allowed IPs on Peer";
    return nullptr;
  }
  return peer;
}

/*
 * PUBLIC METHODS
 */

// static
bool WireguardHelper::deviceExists() {
  // Also confirms it is wireguard.
  return WireguardHelper::currentDevices().contains(iface_name());
};

// static
QStringList WireguardHelper::currentDevices() {
  char *deviceNames, *deviceName;
  size_t len;
  QStringList devices;
  deviceNames = wg_list_device_names();
  wg_for_each_device_name(deviceNames, deviceName, len) {
    devices.append(deviceName);
  }
  free(deviceNames);
  return devices;
}

// static
bool WireguardHelper::addDevice() {
  int returnCode = wg_add_device(iface_name());
  if (returnCode != 0) {
    qWarning("Adding interface `%s` failed with return code: %d", iface_name(),
             returnCode);
    return false;
  }
  return true;
}

// static
bool WireguardHelper::configureDevice(const Daemon::Config& config) {
  /*
   * Set conf:
   * - sets name of device
   * - sets public key on device
   * - sets private key on peer
   * - sets endpoint on peer
   * - sets allowed ips on peer
   *
   *  TODO - I don't think anything here is linux specific
   */

  // DEVICE
  wg_device* device = static_cast<wg_device*>(calloc(1, sizeof(*device)));
  if (!device) {
    logger.log() << "Allocation failure";
    return false;
  }
  auto guard = qScopeGuard([&] { wg_free_device(device); });

  // Name
  strncpy(device->name, iface_name(), IFNAMSIZ);
  // Private Key
  wg_key_from_base64(device->private_key, config.m_privateKey.toLocal8Bit());
  // Peer
  wg_peer* peer = buildPeerForDevice(device, config);
  if (!peer) {
    logger.log() << "Failed to create peer.";
    return false;
  }
  // Set/update device
  device->flags =
      (wg_device_flags)(WGPEER_HAS_PUBLIC_KEY | WGDEVICE_HAS_PRIVATE_KEY |
                        WGDEVICE_REPLACE_PEERS);
  if (wg_set_device(device) != 0) {
    logger.log() << "Failed to set the new peer";
    return false;
  }
  return true;
}

// static
bool WireguardHelper::addDeviceIps(const Daemon::Config& config) {
  // TODO - These are linux specific
  if (!addIP4AddressToDevice(config)) {
    return false;
  }
  if (config.m_ipv6Enabled) {
    if (!addIP6AddressToDevice(config)) {
      return false;
    }
  }
  return true;
}

// static
bool WireguardHelper::setMTUAndUp() {
  // TODO - Linux specific

  // Create socket file descriptor to perform the ioctl operations on
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  // Setup the interface to interact with
  struct ifreq ifr;
  strncpy(ifr.ifr_name, iface_name(), IFNAMSIZ);

  // MTU
  ifr.ifr_mtu = 1420;
  int ret = ioctl(sockfd, SIOCSIFMTU, &ifr);
  if (ret) {
    logger.log() << "Failed to set MTU -- Return code: " << ret;
    return false;
  }
  // Up
  ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
  ret = ioctl(sockfd, SIOCSIFFLAGS, &ifr);
  if (ret) {
    logger.log() << "Failed to set device up -- Return code: " << ret;
    return false;
  }

  close(sockfd);
  // TODO c++ question - do I need to free any other objects?
  return true;
}

// static
bool WireguardHelper::setDNS(const Daemon::Config& config) {
  struct ifreq ifr;
  struct sockaddr_in* ifrAddr = (struct sockaddr_in*)&ifr.ifr_addr;
  strncpy(ifr.ifr_name, iface_name(), IFNAMSIZ);
  ifr.ifr_addr.sa_family = AF_INET;
  int ret = ioctl(sockfd, SIOGIFINDEX, &ifr);
  if (ret) {
    logger.log() << "Failed to get ifrindex. Return code: " << ret;
    return false;
  }
  int IFRINDEX = ifr.ifr_ifindex;

  /*
  The SetLinkDNS() method sets the DNS servers to use on a specific
  interface. This method (and the following ones) may be used by
  network management software to configure per-interface DNS
  settings. It takes a network interface index as well as an array
  of DNS server IP address records. Each array item consists of an
  address family (either AF_INET or AF_INET6), followed by a 4-byte
  or 16-byte array with the raw address data.

  The command that works on command line winth annotations
  sudo
  busctl
  call
  org.freedesktop.resolve1
  /org/freedesktop/resolve1
  org.freedesktop.resolve1.Manager
  SetLinkDNS
  'ia(iay)' <- don't know what that is
  377 <- interface index
  1 <- the next three numbers must be some kind of rep of the array
  2
  4
  8 8 8 8 <- dns ip address

  */
}

// static
bool WireguardHelper::delDevice() {
  // TODO - not linux specific
  int returnCode = wg_del_device(iface_name());
  if (returnCode != 0) {
    qWarning("Deleting interface `%s` failed with return code: %d",
             iface_name(), returnCode);
    return false;
  }
  return true;
}

// static
bool WireguardHelper::unsetDNS() {
  // Stuff
}