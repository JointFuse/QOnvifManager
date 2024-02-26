﻿#include "message.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <QTime>
#include <QUuid>
#include <time.h>
#include <QRandomGenerator64>

using namespace ONVIF;


QDomElement
ONVIF::hashToXml(const QString& name, const QHash<QString, QString>& hash) {
    QDomElement element = newElement(name);
    QHashIterator<QString, QString> i(hash);
    while (i.hasNext()) {
        i.next();
        element.appendChild(newElement(i.key(), i.value()));
    }
    return element;
}

QDomElement
ONVIF::listToXml(
    const QString& name, const QString& itemName, const QStringList& list) {
    QDomElement element = newElement(name);
    for (int i = 0; i < list.length(); i++) {
        element.appendChild(newElement(itemName, list.at(i)));
    }
    return element;
}

QDomElement
ONVIF::newElement(const QString& name, const QString& value) {
    QDomDocument doc;
    QDomElement  element = doc.createElement(name);
    if (value != "") {
        QDomText textNode = doc.createTextNode(value);
        element.appendChild(textNode);
    }
    doc.appendChild(element);
    return doc.firstChildElement();
}


Message*
Message::getOnvifSearchMessage() {
    QHash<QString, QString> namespaces;
    namespaces.insert("a", "http://schemas.xmlsoap.org/ws/2004/08/addressing");
    Message*    msg    = new Message(namespaces);
    QDomElement action = newElement("a:Action", "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe");
    action.setAttribute("soap:mustUnderstand", "1");
    QDomElement message_id = newElement("a:MessageID", "uuid:" + msg->uuid());
    QDomElement to = newElement("a:To", "urn:schemas-xmlsoap-org:ws:2005:04:discovery");
    to.setAttribute("soap:mustUnderstand", "1");
    msg->appendToHeader(action);
    msg->appendToHeader(message_id);
    msg->appendToHeader(to);

    QDomElement probe = newElement("Probe");
    probe.setAttribute("xmlns", "http://schemas.xmlsoap.org/ws/2005/04/discovery");
    QDomElement types = newElement("d:Types", "dp0:NetworkVideoTransmitter");
    types.setAttribute("xmlns:d", "http://schemas.xmlsoap.org/ws/2005/04/discovery");
    types.setAttribute("xmlns:dp0", "http://www.onvif.org/ver10/network/wsdl");
    probe.appendChild(types);
    msg->appendToBody(probe);

    return msg;
}

#define SOAP_WSSE_NONCELEN (20)
static bool
CalcWssePassword(
    const QString& timeStr,
    const QString& passwd,
    QString&       passwdDigest,
    QString&       nonceBase64) {
    char                buf[SOAP_WSSE_NONCELEN];
    int                 nonceInt = QRandomGenerator64::system()->generate64();
    QByteArray          sha1Output;
    QCryptographicHash* hash = NULL;

    // sprintf(buf, "%8.8x%4.4hx%8.8x", (int)time(NULL), count++, nonceInt);
    memset(buf, 1, SOAP_WSSE_NONCELEN);
    memcpy(buf, &nonceInt, sizeof(nonceInt));
    QByteArray nonce(buf, SOAP_WSSE_NONCELEN);
    QString    nonceStr(nonce.toBase64());
    // qDebug() << "nonceStr:";
    // qDebug() <<  nonceStr;
    hash = new QCryptographicHash(QCryptographicHash::Sha1);
    // QByteArray sha1Output = QCryptographicHash::hash(sha1Input.toLatin1(),
    // QCryptographicHash::Sha1);

    hash->addData(buf, SOAP_WSSE_NONCELEN);
    hash->addData(timeStr.toLocal8Bit().data(), strlen(timeStr.toLocal8Bit().data()));
    hash->addData(passwd.toLocal8Bit().data(), strlen(passwd.toLocal8Bit().data()));
    sha1Output = hash->result();
    QString passwdDigestStr(sha1Output.toBase64());
//    QString sha1OutputStr(sha1Output.toHex());
    // qDebug() << "sha1Output" << sha1OutputStr;
    // qDebug() << "passwdDigestStr:";
    // qDebug() << passwdDigestStr;

    nonceBase64  = nonceStr;
    passwdDigest = passwdDigestStr;

    delete hash;

    return true;
}

Message*
Message::getMessageWithUserInfo(
    const QHash<QString, QString>& namespaces,
    const QString& name,
    const QString& passwd,
    QDateTime current) {
    Message*    msg      = new Message(namespaces);
    QDomElement security = newElement("wsse:Security");

    QDomElement usernameToken = newElement("wsse:UsernameToken");
    // usernameToken.setAttribute("wsu:Id", "UsernameToken-1");
    QString   timeString = current.toString(Qt::ISODate);
    /* PasswordDigest  */
    QDomElement username = newElement("wsse:Username", name);
    QString     passwdDigest;
    QString     nonceBase64;
    /* calc passwd Digest and nonce */
    CalcWssePassword(timeString, passwd, passwdDigest, nonceBase64);

    QDomElement password = newElement("wsse:Password", passwdDigest);
    QDomElement nonce    = newElement("wsse:Nonce", nonceBase64);
    password.setAttribute(
        "Type",
        "http://docs.oasis-open.org/wss/2004/01/"
        "oasis-200401-wss-username-token-profile-1.0#"
        "PasswordDigest");
    usernameToken.appendChild(username);
    usernameToken.appendChild(password);
    usernameToken.appendChild(nonce);
    usernameToken.appendChild(newElement("wsu:Created", timeString));
    security.appendChild(usernameToken);
#if 0
    QDomElement timestamp = newElement("wsu:Timestamp");
    timestamp.setAttribute("wsu:Id", "Timestamp-2");
    timestamp.appendChild(newElement("wsu:Created", current.toTimeSpec(Qt::UTC).toString("yyyy-MM-ddThh:mm:ss")));
    timestamp.appendChild(newElement("wsu:Expires", current.toTimeSpec(Qt::UTC).addSecs(60).toString("yyyy-MM-ddThh:mm:ss")));
    security.appendChild(timestamp);
#endif
    msg->appendToHeader(security);
    return msg;
}


Message::Message(const QHash<QString, QString>& namespaces, QObject* parent)
    : QObject(parent) {
    this->mNamespaces = namespaces;
    mDoc.appendChild(mDoc.createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"UTF-8\""));
    mEnv = mDoc.createElementNS(
        "http://www.w3.org/2003/05/soap-envelope", "soap:Envelope");
    mHeader = mDoc.createElement("soap:Header");
    mBody   = mDoc.createElement("soap:Body");
}

QString
Message::toXmlStr() {
    QHashIterator<QString, QString> i(mNamespaces);
    while (i.hasNext()) {
        i.next();
        mEnv.setAttribute("xmlns:" + i.key(), i.value());
    }

    mEnv.appendChild(mHeader);
    mEnv.appendChild(mBody);
    mDoc.appendChild(mEnv);
    return mDoc.toString();
}

QString
Message::uuid() {
    QUuid id = QUuid::createUuid();
    return id.toString();
}

void
Message::appendToBody(const QDomElement& body) {
    mBody.appendChild(body);
}

void
Message::appendToHeader(const QDomElement& header) {
    mHeader.appendChild(header);
}
