#include "service.h"
#include <QDebug>
#include <QFile>

using namespace ONVIF;

Service::Service(
    const QString& wsdlUrl, const QString& username, const QString& password) {
    mUsername = username;
    mPassword = password;
    mClient   = new Client(wsdlUrl);
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
    return Message::getMessageWithUserInfo(namespaces, mUsername, mPassword);
}
