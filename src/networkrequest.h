/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QTimer>

class QHostAddress;
class QNetworkAccessManager;

class NetworkRequest final : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(NetworkRequest)

 public:
  ~NetworkRequest();

  // This object deletes itself at the end of the operation.

  static NetworkRequest* createForGetUrl(QObject* parent, const QString& url,
                                         int status = 0);

  static NetworkRequest* createForAuthenticationVerification(
      QObject* parent, const QString& pkceCodeSuccess,
      const QString& pkceCodeVerifier);

  static NetworkRequest* createForDeviceCreation(QObject* parent,
                                                 const QString& deviceName,
                                                 const QString& pubKey);

  static NetworkRequest* createForDeviceRemoval(QObject* parent,
                                                const QString& pubKey);

  static NetworkRequest* createForServers(QObject* parent);

  static NetworkRequest* createForAccount(QObject* parent);

  static NetworkRequest* createForVersions(QObject* parent);

  static NetworkRequest* createForIpInfo(QObject* parent,
                                         const QHostAddress& address);

  static NetworkRequest* createForCaptivePortalDetection(
      QObject* parent, const QUrl& url, const QByteArray& host);

  static NetworkRequest* createForCaptivePortalLookup(QObject* parent);

  static NetworkRequest* createForHeartbeat(QObject* parent);

  static NetworkRequest* createForSurveyData(QObject* parent);

#ifdef MVPN_IOS
  static NetworkRequest* createForIOSProducts(QObject* parent);

  static NetworkRequest* createForIOSPurchase(QObject* parent,
                                              const QString& receipt);
#endif

  void disableTimeout();

  int statusCode() const;

  QByteArray rawHeader(const QByteArray& headerName) const;

  void abort();

 private:
  NetworkRequest(QObject* parent, int status);

  void deleteRequest();
  void getRequest();
  void postRequest(const QByteArray& body);

  void handleReply(QNetworkReply* reply);
  void handleHeaderReceived();

 private slots:
  void replyFinished();
  void timeout();

 signals:
  void requestHeaderReceived(NetworkRequest* request);
  void requestFailed(QNetworkReply::NetworkError error, const QByteArray& data);
  void requestCompleted(const QByteArray& data);

 private:
  QNetworkRequest m_request;
  QTimer m_timer;

  QNetworkReply* m_reply = nullptr;
  int m_status = 0;
  bool m_completed = false;
};

#endif  // NETWORKREQUEST_H
