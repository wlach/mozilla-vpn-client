/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DNSUTILS_H
#define DNSUTILS_H

#include "interfaceconfig.h"

#include <QObject>
#include <QCoreApplication>

class DNSUtils : public QObject {
 public:
  explicit DNSUtils(QObject* parent) : QObject(parent){};
  virtual ~DNSUtils() = default;

  virtual bool setDNS(const InterfaceConfig& config) {
    Q_UNUSED(config);
    qFatal("Have you forgotten to implement DNSUtils::setDNS?");
    return false;
  };

  virtual bool unsetDNS() {
    qFatal("Have you forgotten to implement DNSUtils::unsetDNS?");
    return false;
  };
};

#endif  // DNSUTILS_H
