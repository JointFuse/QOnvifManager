#include "client.h"
#include <QEventLoop>
#include <QUrl>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QUrlQuery>
#include <QCoreApplication>
using namespace ONVIF;

Client::Client(const QString &url)
    : m_networkManager{ new decltype(m_networkManager)::element_type{} }
{
    mUrl = url;
}

QString Client::sendData(const QString &data)
{
//    QNetworkAccessManager* networkManager = new QNetworkAccessManager;
    QUrl url(mUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"Content-Type: text/xml");
    QNetworkReply *reply = m_networkManager->post(request, data.toUtf8());
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec(QEventLoop::ExcludeUserInputEvents);
//    while (reply->isRunning())
//    {
//        if (QCoreApplication::instance() != nullptr)
//            QCoreApplication::instance()->processEvents(QEventLoop::ExcludeUserInputEvents);
//    }
    auto replyData = reply->readAll();
    reply->deleteLater();
//    networkManager->deleteLater();
    return replyData;
}
