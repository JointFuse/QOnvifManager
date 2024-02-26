#ifndef ONVIF_CLIENT_H
#define ONVIF_CLIENT_H

#include <memory>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
namespace ONVIF
{
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const QString &url);
    ~Client();
    QString sendData(const QString &data, int timeoutMs = 0);
private:
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
    QNetworkReply* m_lastReply{ nullptr };
    QString mUrl;
    bool mTimerIsTrue;
};
}

#endif // ONVIF_CLIENT_H
