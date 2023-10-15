#include "devicesearcher.h"
#include <QXmlQuery>
#include <QBuffer>
#include "messageparser.h"
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QRandomGenerator>

#ifdef WIN32
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#endif

using namespace ONVIF;

const QHash<QString, QString> DeviceSearcher::namespaces = {
    {"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope"},
    {"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding"},
    {"xsi", "http://www.w3.org/2001/XMLSchema-instance"},
    {"xsd", "http://www.w3.org/2001/XMLSchema"},
    {"c14n", "http://www.w3.org/2001/10/xml-exc-c14n#"},
    {"wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"},
    {"xenc", "http://www.w3.org/2001/04/xmlenc#"},
    {"ds", "http://www.w3.org/2000/09/xmldsig#"},
    {"wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"},
    {"wsa5", "http://schemas.xmlsoap.org/ws/2004/08/addressing"},
    {"xmime", "http://tempuri.org/xmime.xsd"},
    {"xop", "http://www.w3.org/2004/08/xop/include"},
    {"wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing"},
    {"tt", "http://www.onvif.org/ver10/schema"},
    {"wsbf", "http://docs.oasis-open.org/wsrf/bf-2"},
    {"wstop", "http://docs.oasis-open.org/wsn/t-1"},
    {"d", "http://schemas.xmlsoap.org/ws/2005/04/discovery"},
    {"wsr", "http://docs.oasis-open.org/wsrf/r-2"},
    {"dndl", "http://www.onvif.org/ver10/network/wsdl/DiscoveryLookupBinding"},
    {"dnrd", "http://www.onvif.org/ver10/network/wsdl/RemoteDiscoveryBinding"},
    {"dn", "http://www.onvif.org/ver10/network/wsdl"},
    {"tad", "http://www.onvif.org/ver10/analyticsdevice/wsdl"},
    {"tanae", "http://www.onvif.org/ver20/analytics/wsdl/AnalyticsEngineBinding"},
    {"tanre", "http://www.onvif.org/ver20/analytics/wsdl/RuleEngineBinding"},
    {"tan", "http://www.onvif.org/ver20/analytics/wsdl"},
    {"tds", "http://www.onvif.org/ver10/device/wsdl"},
    {"tetcp", "http://www.onvif.org/ver10/events/wsdl/CreatePullPointBinding"},
    {"tete", "http://www.onvif.org/ver10/events/wsdl/EventBinding"},
    {"tetnc", "http://www.onvif.org/ver10/events/wsdl/NotificationConsumerBinding"},
    {"tetnp", "http://www.onvif.org/ver10/events/wsdl/NotificationProducerBinding"},
    {"tetpp", "http://www.onvif.org/ver10/events/wsdl/PullPointBinding"},
    {"tetpps", "http://www.onvif.org/ver10/events/wsdl/PullPointSubscriptionBinding"},
    {"tev", "http://www.onvif.org/ver10/events/wsdl"},
    {"tetps", "http://www.onvif.org/ver10/events/wsdl/PausableSubscriptionManagerBinding"},
    {"wsnt", "http://docs.oasis-open.org/wsn/b-2"},
    {"tetsm", "http://www.onvif.org/ver10/events/wsdl/SubscriptionManagerBinding"},
    {"timg", "http://www.onvif.org/ver20/imaging/wsdl"},
    {"timg10", "http://www.onvif.org/ver10/imaging/wsdl"},
    {"tls", "http://www.onvif.org/ver10/display/wsdl"},
    {"tmd", "http://www.onvif.org/ver10/deviceIO/wsdl"},
    {"tptz", "http://www.onvif.org/ver20/ptz/wsdl"},
    {"trc", "http://www.onvif.org/ver10/recording/wsdl"},
    {"trp", "http://www.onvif.org/ver10/replay/wsdl"},
    {"trt", "http://www.onvif.org/ver10/media/wsdl"},
    {"trv", "http://www.onvif.org/ver10/receiver/wsdl"},
    {"tse", "http://www.onvif.org/ver10/search/wsdl"},
    {"tns1", "http://www.onvif.org/ver10/schema"},
    {"tnsn", "http://www.eventextension.com/2011/event/topics"},
    {"tnsavg", "http://www.avigilon.com/onvif/ver10/topics"},
};

DeviceSearcher* DeviceSearcher::searcher = NULL;

DeviceSearcher* DeviceSearcher::instance(QHostAddress &addr __unused)
{
    if(searcher == NULL) {
        searcher = new DeviceSearcher();
    }
    return searcher;
}

QList<QHostAddress> DeviceSearcher::getHostAddress()
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    QList<QHostAddress> ipAddressesIPV4;

    // skip IPV6 address
    QList<QHostAddress>::iterator i;
    for(i=ipAddressesList.begin(); i!=ipAddressesList.end(); i++) {
        std::string ip = (*i).toString().toStdString();
        size_t p1 = ip.find("127.");
        if (p1 != std::string::npos) {
            continue;
        }
        size_t p2 = ip.find(":");
        if (p2 != std::string::npos) {
            continue;
        }
        ipAddressesIPV4.append(*i);
    }

    return ipAddressesIPV4;
}

DeviceSearcher::DeviceSearcher(/*QHostAddress &addr, */QObject *parent) : QObject(parent)
{
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(sendSearchMsg()),
            Qt::QueuedConnection);
}

DeviceSearcher::~DeviceSearcher()
{

}


void DeviceSearcher::startSearch()
{
    static constexpr auto MAX_DEL_MS = 500;
    if (m_timer.isActive()) return;

    m_sockets.clear();
    auto interfaces = QNetworkInterface::allInterfaces();
    for (auto& intr : interfaces)
    {
        if (!intr.isValid() ||
            (intr.type() != QNetworkInterface::Ethernet &&
             intr.type() != QNetworkInterface::Loopback &&
             intr.type() != QNetworkInterface::Wifi))
            continue;
        auto emplRes = m_sockets.emplace(
                        decltype(m_sockets)::key_type{
                            new decltype(m_sockets)::key_type::element_type });
        if (!emplRes.second)
            continue;
        auto skct = emplRes.first->get();
        auto res = skct->bind(QHostAddress{ intr.hardwareAddress() },
                              0,
                              QUdpSocket::ReuseAddressHint |
                              QUdpSocket::ShareAddress);
        skct->setMulticastInterface(intr);
#ifdef QT_DEBUG
        qDebug() << "[" << intr.humanReadableName() << "]"
                 << " socket state = " << res
                 << "; last error: " << skct->errorString();
#endif
        connect(skct, SIGNAL(readyRead()),
                this, SLOT(readPendingDatagrams()));
    }

    msg = std::unique_ptr<Message>(Message::getOnvifSearchMessage());
    m_recall = 0;
    m_timer.setInterval(QRandomGenerator::global()->bounded(MAX_DEL_MS));
    m_timer.start();
}

void DeviceSearcher::sendSearchMsg()
{
    static constexpr auto MAX_SENDS = 2;
    if (MAX_SENDS <= m_recall)
    {
        m_timer.stop();
        m_sockets.clear();
        emit deviceSearchingEnded();
        return;
    }
    ++m_recall;
    QString msg_str = msg->toXmlStr();
    for (auto& sckt : m_sockets)
    {
        auto sendedSize = sckt->writeDatagram(msg_str.toUtf8(),
                                              QHostAddress("239.255.255.250"),
                                              3702);
#ifdef QT_DEBUG
        qDebug() << "[" << sckt->multicastInterface().humanReadableName() << "]" << "sended search message size = " << sendedSize;
        if (sendedSize < 0)
            qDebug() << "[" << sckt->multicastInterface().humanReadableName() << "]" << "error while writing datagram: " << sckt->errorString();
        if (sendedSize)
            qDebug() << "REQQQQQQQ: " << msg->toXmlStr() << "\n"; // todolog
#endif
    }
}

void DeviceSearcher::readPendingDatagrams()
{
    auto sckt = m_sockets.find(std::unique_ptr<
                    decltype(m_sockets)::key_type::element_type
                >(qobject_cast<
                    decltype(m_sockets)::key_type::element_type*
                >(sender())));

    if (sckt == m_sockets.end())
#ifndef QT_DEBUG
        return;
#else
    {
        if (sckt->get() != nullptr)
            qDebug() << "[" << sckt->get()->multicastInterface().humanReadableName() << "]"
                     << " got packet from unregistered network interface";
        else
            qDebug() << "[device searcher] got packet from unregistered network interface";
        return;
    }
#endif
    do {
        auto datagram = std::unique_ptr<QByteArray>();
        datagram->resize(sckt->get()->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        auto msgSize = sckt->get()->readDatagram(datagram->data(), datagram->size(),
                                                 &sender, &senderPort);
#ifdef QT_DEBUG
        qDebug() << "resolved search message size = " << msgSize;
        qDebug() << "[" << sender << "] "
                 << "RESSSSSSS: "
                 << QString::fromStdString(datagram->toStdString()) << "\n";
#endif
        if (msgSize)
            m_recievedPackets.push(std::move(*(datagram.release())));
    } while((sckt->get()->hasPendingDatagrams()));

    m_sockets.erase(sckt);
    QMetaObject::invokeMethod(this,
                              SLOT(processDatagramQueue),
                              Qt::QueuedConnection);
}

void DeviceSearcher::processDatagramQueue()
{
    if (m_recievedPackets.empty())
        return;

    while (m_recievedPackets.size() != 0)
    {
        QHash<QString, QString> device_infos;

        {
            auto& datagram = m_recievedPackets.front();
            MessageParser parser(QString(datagram), namespaces);
            device_infos.insert("ep_address", parser.getValue("//d:ProbeMatches/d:ProbeMatch/wsa:EndpointReference/wsa:Address"));
            device_infos.insert("types", parser.getValue("//d:ProbeMatches/d:ProbeMatch/d:Types"));
            device_infos.insert("device_ip", parser.getValue("//d:ProbeMatches/d:ProbeMatch/d:Scopes"));
            QString address = parser.getValue("//d:ProbeMatches/d:ProbeMatch/d:XAddrs");
            QStringList addresses = address.split(' ');
            address = addresses.value(0);
            device_infos.insert("device_service_address", address);
            device_infos.insert("scopes", parser.getValue("//d:ProbeMatches/d:ProbeMatch/wsa:EndpointReference/wsa:Address"));
            device_infos.insert("metadata_version", parser.getValue("//d:ProbeMatches/d:ProbeMatch/d:MetadataVersion"));
        }

        m_recievedPackets.pop();
        emit receiveData(device_infos);
    }

    if (m_sockets.empty())
        emit deviceSearchingEnded();
}
