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

QString Client::sendData(const QString &data, int timeout)
{
//    QNetworkAccessManager* networkManager = new QNetworkAccessManager;
    QUrl url(mUrl);
    QNetworkRequest request(url);
    if (0 < timeout)
        m_networkManager->setTransferTimeout(timeout);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"Content-Type: text/xml");
    QNetworkReply *reply = m_networkManager->post(request, data.toUtf8());
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec(QEventLoop::ExcludeUserInputEvents);
    reply->deleteLater();

//    while (reply->isRunning())
//    {
//        if (QCoreApplication::instance() != nullptr)
//            QCoreApplication::instance()->processEvents(QEventLoop::ExcludeUserInputEvents);
//    }    
//    networkManager->deleteLater();

    auto replyData = QByteArray{};

    if (0 < timeout)
        m_networkManager->setTransferTimeout(0);
    if (reply->error() != QNetworkReply::TimeoutError)
        replyData = reply->readAll();

    return replyData;
}
