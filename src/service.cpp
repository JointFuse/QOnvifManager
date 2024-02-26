#include <thread>
#include "service.h"
#include <QDebug>
#include <QFile>
#include "device_management/systemdateandtime.h"

using namespace ONVIF;

static constexpr auto UPDATEPERIODSEC = 10;

static const auto NAMESPACES_MESSAGE = QHash<QString, QString>{
    {"wsdl", "http://www.onvif.org/ver10/device/wsdl"},
    {"sch", "http://www.onvif.org/ver10/schema"},
    {"wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"},
    {"wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"},
};

Service::Service(
    const QString& wsdlUrl, const QString& username, const QString& password) {
    mUsername = username;
    mPassword = password;
    mClient   = new Client(wsdlUrl);
    mLastDeviceTimeUpdate = std::chrono::system_clock::now() -
                            std::chrono::seconds(UPDATEPERIODSEC * 2);
}

Service::~Service() {
    if (mClient != NULL) {
        delete mClient;
        mClient = NULL;
    }
}

MessageParser*
Service::sendMessage(Message& message, const QString& namespaceKey) {
    return sendMessage(&message, namespaceKey);
}

MessageParser*
Service::sendMessage(Message* message, const QString& namespaceKey, int* sendDuration) {
    if (message == NULL) {
        return NULL;
    }
#ifdef QT_DEBUG
    auto resFile = new QFile("wsdl_messages_log.txt");
    qDebug() << "REQQQQQQQ: " << message->toXmlStr(); // todolog
    qInfo() << resFile->open(QIODevice::WriteOnly | QIODevice::Append);
    qInfo() << resFile->write(message->toXmlStr().toStdString().c_str());
#endif
    if (sendDuration != nullptr)
        *sendDuration = QDateTime::currentMSecsSinceEpoch();
    QString result = mClient->sendData(message->toXmlStr());
    if (sendDuration != nullptr)
        *sendDuration = QDateTime::currentMSecsSinceEpoch() - *sendDuration;
#ifdef QT_DEBUG
    qDebug() << "RESSSSSSS: " << result;
    qInfo() << resFile->write(result.toStdString().c_str());
    resFile->close();
    delete resFile;
#endif
    if (result == "") {
        return NULL;
    }
    const QHash<QString, QString>& names = namespaces(namespaceKey);
    return new MessageParser(result, names);
}

Message*
Service::createMessage(const QHash<QString, QString>& namespaces) {
    if (UPDATEPERIODSEC < std::chrono::duration_cast<std::chrono::seconds>(
                              std::chrono::system_clock::now() -
                              mLastDeviceTimeUpdate).count() ||
        !mDeviceDateTime.isValid())
    {
        SystemDateAndTime* systemDateAndTime = NULL;
        Message*           msg               = new Message{ NAMESPACES_MESSAGE };
        msg->appendToBody(newElement("wsdl:GetSystemDateAndTime"));
        MessageParser* result = sendMessage(msg);

        if (result != NULL) {
            systemDateAndTime = new SystemDateAndTime();
            systemDateAndTime->setUtcTime(
                result->getValue("//tt:UTCDateTime/tt:Date/tt:Year").toInt(),
                result->getValue("//tt:UTCDateTime/tt:Date/tt:Month").toInt(),
                result->getValue("//tt:UTCDateTime/tt:Date/tt:Day").toInt(),
                result->getValue("//tt:UTCDateTime/tt:Time/tt:Hour").toInt(),
                result->getValue("//tt:UTCDateTime/tt:Time/tt:Minute").toInt(),
                result->getValue("//tt:UTCDateTime/tt:Time/tt:Second").toInt());
            mDeviceDateTime = systemDateAndTime->utcTime();
            delete systemDateAndTime;
            delete result;
        }
        else
            mDeviceDateTime = QDateTime::currentDateTimeUtc();

        mLastDeviceTimeUpdate = std::chrono::system_clock::now();
        delete msg;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return Message::getMessageWithUserInfo(namespaces,
                                           mUsername,
                                           mPassword,
                                           mDeviceDateTime);
}
