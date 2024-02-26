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

Client::~Client()
{

}

QString Client::sendData(const QString &data, int timeout)
{
    QUrl url(mUrl);
    QNetworkRequest request(url);
    if (0 < timeout)
        m_networkManager->setTransferTimeout(timeout);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"Content-Type: text/xml");
    m_lastReply = m_networkManager->post(request, data.toUtf8());
    QEventLoop loop;
    connect(m_lastReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    if (!m_lastReply->isFinished())
    {
#ifdef QT_DEBUG
        qDebug() << "[QOnvifManager] Thread exited while QNetworkReply is still running!";
#endif
        m_lastReply->abort();
    }

    if (0 < timeout)
        m_networkManager->setTransferTimeout(0);
    if (m_lastReply->error() != QNetworkReply::TimeoutError)
        return m_lastReply->readAll();

    return {};
}
