﻿#include "mediamanagement.h"
#include "messageparser.h"
#include "datastruct.hpp"
#include <QDebug>
#include <QXmlResultItems>
using namespace ONVIF;

static const auto NAMESPACES = QHash<QString, QString>{
    {"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope"},
    {"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding"},
    {"xsi", "http://www.w3.org/2001/XMLSchema-instance"},
    {"xsd", "http://www.w3.org/2001/XMLSchema"},
    {"c14n", "http://www.w3.org/2001/10/xml-exc-c14n#"},
    {"wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"},
    {"xenc", "http://www.w3.org/2001/04/xmlenc#"},
    {"ds", "http://www.w3.org/2000/09/xmldsig#"},
    {"wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"},
    {"wsa5", "http://www.w3.org/2005/08/addressing"},
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

static const auto NAMESPACES_MESSAGE = QHash<QString, QString>{
    {"wsdl", "http://www.onvif.org/ver10/media/wsdl"},
    {"tt", "http://www.onvif.org/ver10/schema"},
    {"trt", "http://www.onvif.org/ver10/media/wsdl"},
    {"sch", "http://www.onvif.org/ver10/schema"},
    {"timg", "http://www.onvif.org/ver20/imaging/wsdl"},
    {"wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"},
    {"wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"},
};

MediaManagement::MediaManagement(
    const QString& wsdlUrl, const QString& username, const QString& password)
    : Service(wsdlUrl, username, password) {}

const QHash<QString, QString>&
MediaManagement::namespaces(const QString& key) {
    Q_UNUSED(key);
    return NAMESPACES;
}
Message*
MediaManagement::newMessage() {
    return createMessage(NAMESPACES_MESSAGE);
}
VideoSourceConfigurations*
MediaManagement::getVideoSourceConfigurations() {
    VideoSourceConfigurations* videoSourceConfigurations = NULL;
    Message*                   msg                       = newMessage();
    msg->appendToBody(newElement("wsdl:GetVideoSourceConfigurations"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        videoSourceConfigurations = new VideoSourceConfigurations();
        QXmlQuery* query          = result->query();
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Configurations");

        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem     item = items.next();
        QString      name, useCount, sourceToken, bounds, value;
        QDomDocument doc;
        QRect        rect;
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&name);
            videoSourceConfigurations->setName(name.trimmed());

            query->setQuery(result->nameSpace() + "./tt:UseCount/string()");
            query->evaluateTo(&useCount);
            videoSourceConfigurations->setUseCount(useCount.trimmed().toInt());

            query->setQuery(result->nameSpace() + "./tt:SourceToken/string()");
            query->evaluateTo(&sourceToken);
            videoSourceConfigurations->setSourceToken(sourceToken.trimmed());

            query->setQuery(result->nameSpace() + "./tt:Bounds[@width]");
            query->evaluateTo(&bounds);
            doc.setContent(bounds);
            QDomNodeList itemNodeList = doc.elementsByTagName("tt:Bounds");
            for (int i = 0; i < itemNodeList.size(); i++) {
                QDomNode node = itemNodeList.at(i);
                value         = node.toElement().attribute("width");
                rect.setWidth(value.toInt());
                value = node.toElement().attribute("height");
                rect.setHeight(value.toInt());
                value = node.toElement().attribute("x");
                rect.setLeft(value.toInt());
                value = node.toElement().attribute("y");
                rect.setTop(value.toInt());
            }
            videoSourceConfigurations->setBounds(rect);
            item = items.next();
        }
        delete result;
    }
    delete msg;
    return videoSourceConfigurations;
}

VideoEncoderConfigurations*
MediaManagement::getVideoEncoderConfigurations() {
    VideoEncoderConfigurations* videoEncoderConfigurations = NULL;
    Message*                    msg                        = newMessage();
    msg->appendToBody(newElement("wsdl:GetVideoEncoderConfigurations"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        videoEncoderConfigurations = new VideoEncoderConfigurations();
        QXmlQuery* query           = result->query();
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Configurations");
        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem item = items.next();
        QString  name, useCount, encoding, width, height, quality,
            frameRateLimit, encodingInterval, bitrateLimit, govLength,
            h264Profile, type, ipv4Address, ipv6Address, port, ttl, autoStart,
            sessionTimeout;
        QString      value, xml;
        QDomDocument doc;
        QDomNodeList itemNodeList;
        QDomNode     node;
        while (!item.isNull()) {
            query->setFocus(item);

            query->setQuery(result->nameSpace() + ".");
            query->evaluateTo(&xml);
            doc.setContent(xml);
            itemNodeList = doc.elementsByTagName("trt:Configurations");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node  = itemNodeList.at(i);
                value = node.toElement().attribute("token");
                videoEncoderConfigurations->setToken(value.trimmed());
            }

            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&name);
            videoEncoderConfigurations->setName(name.trimmed());

            query->setQuery(result->nameSpace() + "./tt:UseCount/string()");
            query->evaluateTo(&useCount);
            videoEncoderConfigurations->setUseCount(useCount.trimmed().toInt());

            query->setQuery(result->nameSpace() + "./tt:Encoding/string()");
            query->evaluateTo(&encoding);
            videoEncoderConfigurations->setEncoding(encoding.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:Resolution/tt:Width/string()");
            query->evaluateTo(&width);
            videoEncoderConfigurations->setWidth(width.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:Resolution/tt:Height/string()");
            query->evaluateTo(&height);
            videoEncoderConfigurations->setHeight(height.trimmed().toInt());

            query->setQuery(result->nameSpace() + "./tt:Quality/string()");
            query->evaluateTo(&quality);
            videoEncoderConfigurations->setQuality(quality.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:RateControl/tt:FrameRateLimit/string()");
            query->evaluateTo(&frameRateLimit);
            videoEncoderConfigurations->setFrameRateLimit(
                frameRateLimit.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:RateControl/tt:EncodingInterval/string()");
            query->evaluateTo(&encodingInterval);
            videoEncoderConfigurations->setEncodingInterval(
                encodingInterval.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:RateControl/tt:BitrateLimit/string()");
            query->evaluateTo(&bitrateLimit);
            videoEncoderConfigurations->setBitrateLimit(
                bitrateLimit.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:H264/tt:GovLength/string()");
            query->evaluateTo(&govLength);
            videoEncoderConfigurations->setGovLength(
                govLength.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:H264/tt:H264Profile/string()");
            query->evaluateTo(&h264Profile);
            videoEncoderConfigurations->setH264Profile(h264Profile.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:Multicast/tt:Address/tt:Type/string()");
            query->evaluateTo(&type);
            videoEncoderConfigurations->setType(type.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:Multicast/tt:Address/tt:IPv4Address/string()");
            query->evaluateTo(&ipv4Address);
            videoEncoderConfigurations->setIpv4Address(ipv4Address.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:Multicast/tt:Address/tt:IPv6Address/string()");
            query->evaluateTo(&ipv6Address);
            videoEncoderConfigurations->setIpv6Address(ipv6Address.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:Multicast/tt:Port/string()");
            query->evaluateTo(&port);
            videoEncoderConfigurations->setPort(port.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&ttl);
            videoEncoderConfigurations->setTtl(ttl.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&sessionTimeout);
            videoEncoderConfigurations->setSessionTimeout(
                sessionTimeout.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:SessionTimeout/string()");
            query->evaluateTo(&autoStart);
            videoEncoderConfigurations->setAutoStart(
                autoStart.trimmed() == "true" ? true : false);
            item = items.next();
        }
        delete result;
    }
    delete msg;
    return videoEncoderConfigurations;
}

Profiles*
MediaManagement::getProfiles() {
    Profiles* profiles = NULL;
    Message*  msg      = newMessage();
    msg->appendToBody(newElement("wsdl:GetProfiles"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        profiles         = new Profiles();
        QXmlQuery* query = result->query();
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Profiles");
        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem     item = items.next();
        QDomDocument doc;
        QString      value, bounds, panTilt, zoom, profileNode;
        QRect        rect;
        while (!item.isNull()) {
            query->setFocus(item);

            query->setQuery(result->nameSpace() + ".");
            query->evaluateTo(&profileNode);
            doc.setContent(profileNode);
            QDomNodeList itemNodeList = doc.elementsByTagName("trt:Profiles");
            QDomNode     node;
            for (int i = 0; i < itemNodeList.size(); i++) {
                value = itemNodeList.at(i).toElement().attribute("token");
                profiles->m_toKenPro.push_back(value.trimmed());

                value = itemNodeList.at(i).toElement().attribute("fixed");
                profiles->m_fixed.push_back(
                    value.trimmed() == "true" ? true : false);
            }

            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&value);
            profiles->m_namePro.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profiles->m_nameVsc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profiles->m_useCountVsc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:SourceToken/string()");
            query->evaluateTo(&value);
            profiles->m_sourceTokenVsc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:Bounds");
            query->evaluateTo(&bounds);
            doc.setContent(bounds);
            itemNodeList = doc.elementsByTagName("tt:Bounds");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node  = itemNodeList.at(i);
                value = node.toElement().attribute("width");
                rect.setWidth(value.toInt());
                value = node.toElement().attribute("height");
                rect.setHeight(value.toInt());
                value = node.toElement().attribute("x");
                rect.setLeft(value.toInt());
                value = node.toElement().attribute("y");
                rect.setTop(value.toInt());
            }
            profiles->m_boundsVsc.push_back(rect);

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profiles->m_nameVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profiles->m_useCountVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Encoding/string()");
            query->evaluateTo(&value);
            profiles->m_encodingVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Resolution/tt:Width/string()");
            query->evaluateTo(&value);
            profiles->m_widthVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Resolution/tt:Height/string()");
            query->evaluateTo(&value);
            profiles->m_heightVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Quality/string()");
            query->evaluateTo(&value);
            profiles->m_qualityVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:FrameRateLimit/"
                                      "string()");
            query->evaluateTo(&value);
            profiles->m_frameRateLimitVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:EncodingInterval/"
                                      "string()");
            query->evaluateTo(&value);
            profiles->m_encodingIntervalVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:BitrateLimit/"
                                      "string()");
            query->evaluateTo(&value);
            profiles->m_bitrateLimitVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:H264/tt:GovLength/string()");
            query->evaluateTo(&value);
            profiles->m_govLengthVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/tt:H264/"
                                      "tt:H264Profile/string()");
            query->evaluateTo(&value);
            profiles->m_h264ProfileVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:Type/"
                                      "string()");
            query->evaluateTo(&value);
            profiles->m_typeVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:IPv4Address/"
                                      "string()");
            query->evaluateTo(&value);
            profiles->m_ipv4AddressVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:IPv6Address/"
                                      "string()");
            query->evaluateTo(&value);
            profiles->m_ipv6AddressVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Multicast/tt:Port/string()");
            query->evaluateTo(&value);
            profiles->m_portVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&value);
            profiles->m_ttlVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:AutoStart/string()");
            query->evaluateTo(&value);
            profiles->m_autoStartVec.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:SessionTimeout/string()");
            query->evaluateTo(&value);
            profiles->m_sessionTimeoutVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profiles->m_namePtz.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profiles->m_useCountPtz.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:NodeToken/string()");
            query->evaluateTo(&value);
            profiles->m_nodeToken.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultAbsolutePantTiltPosit"
                                      "ionSpace/string()");
            query->evaluateTo(&value);
            profiles->m_defaultAbsolutePantTiltPositionSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultAbsoluteZoomPositionS"
                                      "pace/string()");
            query->evaluateTo(&value);
            profiles->m_defaultAbsoluteZoomPositionSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultRelativePanTiltTransl"
                                      "ationSpace/string()");
            query->evaluateTo(&value);
            profiles->m_defaultRelativePantTiltTranslationSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultRelativeZoomTranslati"
                                      "onSpace/string()");
            query->evaluateTo(&value);
            profiles->m_defaultRelativeZoomTranslationSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultContinuousPanTiltVelo"
                                      "citySpace/string()");
            query->evaluateTo(&value);
            profiles->m_defaultContinuousPantTiltVelocitySpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultContinuousZoomVelocit"
                                      "ySpace/string()");
            query->evaluateTo(&value);
            profiles->m_defaultContinuousZoomVelocitySpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZSpeed/tt:PanTilt");
            query->evaluateTo(&panTilt);
            doc.setContent(panTilt);
            itemNodeList = doc.elementsByTagName("tt:PanTilt");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node = itemNodeList.at(i);
                profiles->m_panTiltSpace.push_back(
                    node.toElement().attribute("space").trimmed());
                profiles->m_panTiltX.push_back(
                    node.toElement().attribute("x").trimmed().toInt());
                profiles->m_panTiltY.push_back(
                    node.toElement().attribute("y").trimmed().toInt());
            }

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZSpeed/tt:Zoom");
            query->evaluateTo(&zoom);
            doc.setContent(zoom);
            itemNodeList = doc.elementsByTagName("tt:Zoom");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node = itemNodeList.at(i);
                profiles->m_zoomSpace.push_back(
                    node.toElement().attribute("space").trimmed());
                profiles->m_zoomX.push_back(
                    node.toElement().attribute("x").trimmed().toInt());
            }

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZTimeout/string()");
            query->evaluateTo(&value);
            profiles->m_defaultPTZTimeout.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:URI/string()");
            query->evaluateTo(&value);
            profiles->m_panTiltUri.push_back(value.trimmed());


            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:XRange/tt:Min/string()");
            query->evaluateTo(&value);
            profiles->m_xRangeMinPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:XRange/tt:Max/string()");
            query->evaluateTo(&value);
            profiles->m_xRangeMaxPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:YRange/tt:Min/string()");
            query->evaluateTo(&value);
            profiles->m_yRangeMinPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:YRange/tt:Max/string()");
            query->evaluateTo(&value);
            profiles->m_yRangeMaxPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:ZoomLimits/tt:Range/tt:URI/string()");
            query->evaluateTo(&value);
            profiles->m_zoomUri.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:ZoomLimits/tt:Range/"
                                      "tt:XRange/tt:Min/string()");
            query->evaluateTo(&value);
            profiles->m_xRangeMinZm.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:ZoomLimits/tt:Range/"
                                      "tt:XRange/tt:Max/string()");
            query->evaluateTo(&value);
            profiles->m_xRangeMaxZm.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profiles->m_nameMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profiles->m_useCountMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:PTZStatus/tt:Status/string()");
            query->evaluateTo(&value);
            profiles->m_status.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:PTZStatus/tt:Position/string()");
            query->evaluateTo(&value);
            profiles->m_position.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Events/tt:Filter/string()");
            query->evaluateTo(&value);
            profiles->m_filter.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/tt:Events/"
                                      "tt:SubscriptionPolicy/string()");
            query->evaluateTo(&value);
            profiles->m_subscriptionPolicy.push_back(value.trimmed());


            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Analytics/string()");
            query->evaluateTo(&value);
            profiles->m_analytics.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:Type/string()");
            query->evaluateTo(&value);
            profiles->m_typeMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:IPv4Address/string()");
            query->evaluateTo(&value);
            profiles->m_ipv4AddressMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:IPv6Address/string()");
            query->evaluateTo(&value);
            profiles->m_ipv6AddressMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Multicast/tt:Port/string()");
            query->evaluateTo(&value);
            profiles->m_portMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&value);
            profiles->m_ttlMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:AutoStart/"
                                      "string()");
            query->evaluateTo(&value);
            profiles->m_autoStartMc.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:SessionTimeout/string()");
            query->evaluateTo(&value);
            profiles->m_sessionTimeoutMc.push_back(value.trimmed());


            item = items.next();
        }
        delete result;
    }
    delete msg;
    return profiles;
}

Profile*
MediaManagement::getProfile720P() {
    Profile* profile = NULL;
    Message* msg     = newMessage();
    msg->appendToBody(
        newElement("<wsdl:ProfileToken>profile_720P</wsdl:ProfileToken>"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        profile          = new Profile();
        QXmlQuery* query = result->query();
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Profile");
        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem     item = items.next();
        QDomDocument doc;
        QString      value, bounds, panTilt, zoom, profileNode;
        QRect        rect;
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + ".");
            query->evaluateTo(&profileNode);
            doc.setContent(profileNode);
            QDomNodeList itemNodeList = doc.elementsByTagName("trt:Profiles");
            QDomNode     node;
            for (int i = 0; i < itemNodeList.size(); i++) {
                value = itemNodeList.at(i).toElement().attribute("token");
                profile->m_toKenPro.push_back(value.trimmed());
                qDebug() << "===========" << value;
                value = itemNodeList.at(i).toElement().attribute("fixed");
                profile->m_fixed.push_back(
                    value.trimmed() == "true" ? true : false);
            }

            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_namePro.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_nameVsc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountVsc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:SourceToken/string()");
            query->evaluateTo(&value);
            profile->m_sourceTokenVsc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:Bounds");
            query->evaluateTo(&bounds);
            doc.setContent(bounds);
            itemNodeList = doc.elementsByTagName("tt:Bounds");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node  = itemNodeList.at(i);
                value = node.toElement().attribute("width");
                rect.setWidth(value.toInt());
                value = node.toElement().attribute("height");
                rect.setHeight(value.toInt());
                value = node.toElement().attribute("x");
                rect.setLeft(value.toInt());
                value = node.toElement().attribute("y");
                rect.setTop(value.toInt());
            }
            profile->m_boundsVsc.push_back(rect);

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_nameVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Encoding/string()");
            query->evaluateTo(&value);
            profile->m_encodingVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Resolution/tt:Width/string()");
            query->evaluateTo(&value);
            profile->m_widthVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Resolution/tt:Height/string()");
            query->evaluateTo(&value);
            profile->m_heightVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Quality/string()");
            query->evaluateTo(&value);
            profile->m_qualityVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:FrameRateLimit/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_frameRateLimitVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:EncodingInterval/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_encodingIntervalVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:BitrateLimit/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_bitrateLimitVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:H264/tt:GovLength/string()");
            query->evaluateTo(&value);
            profile->m_govLengthVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/tt:H264/"
                                      "tt:H264Profile/string()");
            query->evaluateTo(&value);
            profile->m_h264ProfileVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:Type/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_typeVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:IPv4Address/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_ipv4AddressVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:IPv6Address/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_ipv6AddressVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Multicast/tt:Port/string()");
            query->evaluateTo(&value);
            profile->m_portVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&value);
            profile->m_ttlVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:AutoStart/string()");
            query->evaluateTo(&value);
            profile->m_autoStartVec.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:SessionTimeout/string()");
            query->evaluateTo(&value);
            profile->m_sessionTimeoutVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_namePtz.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountPtz.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:NodeToken/string()");
            query->evaluateTo(&value);
            profile->m_nodeToken.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultAbsolutePantTiltPosit"
                                      "ionSpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultAbsolutePantTiltPositionSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultAbsoluteZoomPositionS"
                                      "pace/string()");
            query->evaluateTo(&value);
            profile->m_defaultAbsoluteZoomPositionSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultRelativePanTiltTransl"
                                      "ationSpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultRelativePantTiltTranslationSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultRelativeZoomTranslati"
                                      "onSpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultRelativeZoomTranslationSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultContinuousPanTiltVelo"
                                      "citySpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultContinuousPantTiltVelocitySpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultContinuousZoomVelocit"
                                      "ySpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultContinuousZoomVelocitySpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZSpeed/tt:PanTilt");
            query->evaluateTo(&panTilt);
            doc.setContent(panTilt);
            itemNodeList = doc.elementsByTagName("tt:PanTilt");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node = itemNodeList.at(i);
                profile->m_panTiltSpace.push_back(
                    node.toElement().attribute("space").trimmed());
                profile->m_panTiltX.push_back(
                    node.toElement().attribute("x").trimmed().toInt());
                profile->m_panTiltY.push_back(
                    node.toElement().attribute("y").trimmed().toInt());
            }

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZSpeed/tt:Zoom");
            query->evaluateTo(&zoom);
            doc.setContent(zoom);
            itemNodeList = doc.elementsByTagName("tt:Zoom");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node = itemNodeList.at(i);
                profile->m_zoomSpace.push_back(
                    node.toElement().attribute("space").trimmed());
                profile->m_zoomX.push_back(
                    node.toElement().attribute("x").trimmed().toInt());
            }

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZTimeout/string()");
            query->evaluateTo(&value);
            profile->m_defaultPTZTimeout.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:URI/string()");
            query->evaluateTo(&value);
            profile->m_panTiltUri.push_back(value.trimmed());


            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:XRange/tt:Min/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMinPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:XRange/tt:Max/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMaxPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:YRange/tt:Min/string()");
            query->evaluateTo(&value);
            profile->m_yRangeMinPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:YRange/tt:Max/string()");
            query->evaluateTo(&value);
            profile->m_yRangeMaxPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:ZoomLimits/tt:Range/tt:URI/string()");
            query->evaluateTo(&value);
            profile->m_zoomUri.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:ZoomLimits/tt:Range/"
                                      "tt:XRange/tt:Min/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMinZm.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:ZoomLimits/tt:Range/"
                                      "tt:XRange/tt:Max/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMaxZm.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_nameMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:PTZStatus/tt:Status/string()");
            query->evaluateTo(&value);
            profile->m_status.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:PTZStatus/tt:Position/string()");
            query->evaluateTo(&value);
            profile->m_position.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Events/tt:Filter/string()");
            query->evaluateTo(&value);
            profile->m_filter.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/tt:Events/"
                                      "tt:SubscriptionPolicy/string()");
            query->evaluateTo(&value);
            profile->m_subscriptionPolicy.push_back(value.trimmed());


            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Analytics/string()");
            query->evaluateTo(&value);
            profile->m_analytics.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:Type/string()");
            query->evaluateTo(&value);
            profile->m_typeMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:IPv4Address/string()");
            query->evaluateTo(&value);
            profile->m_ipv4AddressMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:IPv6Address/string()");
            query->evaluateTo(&value);
            profile->m_ipv6AddressMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Multicast/tt:Port/string()");
            query->evaluateTo(&value);
            profile->m_portMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&value);
            profile->m_ttlMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:AutoStart/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_autoStartMc.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:SessionTimeout/string()");
            query->evaluateTo(&value);
            profile->m_sessionTimeoutMc.push_back(value.trimmed());


            item = items.next();
        }
        delete result;
    }
    delete msg;
    return profile;
}

Profile*
MediaManagement::getProfileD1() {
    Profile* profile = NULL;
    Message* msg     = newMessage();
    msg->appendToBody(
        newElement("<wsdl:ProfileToken>profile_D1</wsdl:ProfileToken>"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        profile          = new Profile();
        QXmlQuery* query = result->query();
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Profiles");
        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem     item = items.next();
        QDomDocument doc;
        QString      value, bounds, panTilt, zoom, profileNode;
        QRect        rect;
        while (!item.isNull()) {
            query->setFocus(item);

            query->setQuery(result->nameSpace() + ".");
            query->evaluateTo(&profileNode);
            doc.setContent(profileNode);
            QDomNodeList itemNodeList = doc.elementsByTagName("trt:Profiles");
            QDomNode     node;
            for (int i = 0; i < itemNodeList.size(); i++) {
                value = itemNodeList.at(i).toElement().attribute("token");
                profile->m_toKenPro.push_back(value.trimmed());

                value = itemNodeList.at(i).toElement().attribute("fixed");
                profile->m_fixed.push_back(
                    value.trimmed() == "true" ? true : false);
            }

            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_namePro.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_nameVsc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountVsc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:SourceToken/string()");
            query->evaluateTo(&value);
            profile->m_sourceTokenVsc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoSourceConfiguration/tt:Bounds");
            query->evaluateTo(&bounds);
            doc.setContent(bounds);
            itemNodeList = doc.elementsByTagName("tt:Bounds");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node  = itemNodeList.at(i);
                value = node.toElement().attribute("width");
                rect.setWidth(value.toInt());
                value = node.toElement().attribute("height");
                rect.setHeight(value.toInt());
                value = node.toElement().attribute("x");
                rect.setLeft(value.toInt());
                value = node.toElement().attribute("y");
                rect.setTop(value.toInt());
            }
            profile->m_boundsVsc.push_back(rect);

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_nameVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Encoding/string()");
            query->evaluateTo(&value);
            profile->m_encodingVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Resolution/tt:Width/string()");
            query->evaluateTo(&value);
            profile->m_widthVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Resolution/tt:Height/string()");
            query->evaluateTo(&value);
            profile->m_heightVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Quality/string()");
            query->evaluateTo(&value);
            profile->m_qualityVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:FrameRateLimit/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_frameRateLimitVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:EncodingInterval/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_encodingIntervalVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:RateControl/tt:BitrateLimit/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_bitrateLimitVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:H264/tt:GovLength/string()");
            query->evaluateTo(&value);
            profile->m_govLengthVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/tt:H264/"
                                      "tt:H264Profile/string()");
            query->evaluateTo(&value);
            profile->m_h264ProfileVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:Type/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_typeVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:IPv4Address/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_ipv4AddressVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:Address/tt:IPv6Address/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_ipv6AddressVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Multicast/tt:Port/string()");
            query->evaluateTo(&value);
            profile->m_portVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&value);
            profile->m_ttlVec.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:VideoEncoderConfiguration/"
                                      "tt:Multicast/tt:AutoStart/string()");
            query->evaluateTo(&value);
            profile->m_autoStartVec.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:VideoEncoderConfiguration/tt:SessionTimeout/string()");
            query->evaluateTo(&value);
            profile->m_sessionTimeoutVec.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_namePtz.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountPtz.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:NodeToken/string()");
            query->evaluateTo(&value);
            profile->m_nodeToken.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultAbsolutePantTiltPosit"
                                      "ionSpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultAbsolutePantTiltPositionSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultAbsoluteZoomPositionS"
                                      "pace/string()");
            query->evaluateTo(&value);
            profile->m_defaultAbsoluteZoomPositionSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultRelativePanTiltTransl"
                                      "ationSpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultRelativePantTiltTranslationSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultRelativeZoomTranslati"
                                      "onSpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultRelativeZoomTranslationSpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultContinuousPanTiltVelo"
                                      "citySpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultContinuousPantTiltVelocitySpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:"
                                      "DefaultContinuousZoomVelocit"
                                      "ySpace/string()");
            query->evaluateTo(&value);
            profile->m_defaultContinuousZoomVelocitySpace.push_back(
                value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZSpeed/tt:PanTilt");
            query->evaluateTo(&panTilt);
            doc.setContent(panTilt);
            itemNodeList = doc.elementsByTagName("tt:PanTilt");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node = itemNodeList.at(i);
                profile->m_panTiltSpace.push_back(
                    node.toElement().attribute("space").trimmed());
                profile->m_panTiltX.push_back(
                    node.toElement().attribute("x").trimmed().toInt());
                profile->m_panTiltY.push_back(
                    node.toElement().attribute("y").trimmed().toInt());
            }

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZSpeed/tt:Zoom");
            query->evaluateTo(&zoom);
            doc.setContent(zoom);
            itemNodeList = doc.elementsByTagName("tt:Zoom");
            for (int i = 0; i < itemNodeList.size(); i++) {
                node = itemNodeList.at(i);
                profile->m_zoomSpace.push_back(
                    node.toElement().attribute("space").trimmed());
                profile->m_zoomX.push_back(
                    node.toElement().attribute("x").trimmed().toInt());
            }

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:DefaultPTZTimeout/string()");
            query->evaluateTo(&value);
            profile->m_defaultPTZTimeout.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:URI/string()");
            query->evaluateTo(&value);
            profile->m_panTiltUri.push_back(value.trimmed());


            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:XRange/tt:Min/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMinPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:XRange/tt:Max/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMaxPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:YRange/tt:Min/string()");
            query->evaluateTo(&value);
            profile->m_yRangeMinPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:PanTiltLimits/tt:Range/"
                                      "tt:YRange/tt:Max/string()");
            query->evaluateTo(&value);
            profile->m_yRangeMaxPt.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:PTZConfiguration/tt:ZoomLimits/tt:Range/tt:URI/string()");
            query->evaluateTo(&value);
            profile->m_zoomUri.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:ZoomLimits/tt:Range/"
                                      "tt:XRange/tt:Min/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMinZm.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:PTZConfiguration/"
                                      "tt:ZoomLimits/tt:Range/"
                                      "tt:XRange/tt:Max/string()");
            query->evaluateTo(&value);
            profile->m_xRangeMaxZm.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Name/string()");
            query->evaluateTo(&value);
            profile->m_nameMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:UseCount/string()");
            query->evaluateTo(&value);
            profile->m_useCountMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:PTZStatus/tt:Status/string()");
            query->evaluateTo(&value);
            profile->m_status.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:PTZStatus/tt:Position/string()");
            query->evaluateTo(&value);
            profile->m_position.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Events/tt:Filter/string()");
            query->evaluateTo(&value);
            profile->m_filter.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/tt:Events/"
                                      "tt:SubscriptionPolicy/string()");
            query->evaluateTo(&value);
            profile->m_subscriptionPolicy.push_back(value.trimmed());


            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Analytics/string()");
            query->evaluateTo(&value);
            profile->m_analytics.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:Type/string()");
            query->evaluateTo(&value);
            profile->m_typeMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:IPv4Address/string()");
            query->evaluateTo(&value);
            profile->m_ipv4AddressMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:Address/"
                                      "tt:IPv6Address/string()");
            query->evaluateTo(&value);
            profile->m_ipv6AddressMc.push_back(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Multicast/tt:Port/string()");
            query->evaluateTo(&value);
            profile->m_portMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&value);
            profile->m_ttlMc.push_back(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:MetadataConfiguration/"
                                      "tt:Multicast/tt:AutoStart/"
                                      "string()");
            query->evaluateTo(&value);
            profile->m_autoStartMc.push_back(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() +
                "./tt:MetadataConfiguration/tt:SessionTimeout/string()");
            query->evaluateTo(&value);
            profile->m_sessionTimeoutMc.push_back(value.trimmed());


            item = items.next();
        }
        delete result;
    }
    delete msg;
    return profile;
}

AudioSourceConfigurations*
MediaManagement::getAudioSourceConfigurations() {
    AudioSourceConfigurations* audioSourceConfigurations = NULL;
    Message*                   msg                       = newMessage();
    msg->appendToBody(newElement("wsdl:GetAudioSourceConfigurations"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        audioSourceConfigurations = new AudioSourceConfigurations();
        QString         xml, value;
        QDomDocument    doc;
        QDomNodeList    itemNodeList;
        QDomNode        node;
        QXmlQuery*      query = result->query();
        QXmlResultItems items;
        QXmlItem        item;
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Configurations");
        query->evaluateTo(&xml);
        doc.setContent(xml);
        itemNodeList = doc.elementsByTagName("trt:Configurations");
        for (int i = 0; i < itemNodeList.size(); i++) {
            node = itemNodeList.at(i);
            audioSourceConfigurations->setToken(
                node.toElement().attribute("token").trimmed());
        }
        query->evaluateTo(&items);
        item = items.next();
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&value);
            audioSourceConfigurations->setName(value.trimmed());
            query->setQuery(result->nameSpace() + "./tt:UseCount/string()");
            query->evaluateTo(&value);
            audioSourceConfigurations->setUseCount(value.trimmed().toInt());
            query->setQuery(result->nameSpace() + "./tt:SourceToken/string()");
            query->evaluateTo(&value);
            audioSourceConfigurations->setSourceToken(value.trimmed());
            item = items.next();
        }
        delete result;
    }

    delete msg;
    return audioSourceConfigurations;
}

AudioEncoderConfigurations*
MediaManagement::getAudioEncoderConfigurations() {
    AudioEncoderConfigurations* audioEncoderConfigurations = NULL;
    Message*                    msg                        = newMessage();
    msg->appendToBody(newElement("wsdl:GetAudioEncoderConfigurations"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        audioEncoderConfigurations = new AudioEncoderConfigurations();
        QXmlQuery* query           = result->query();
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Configurations");
        QXmlResultItems items;
        QXmlItem        item;
        QDomDocument    doc;
        QDomNodeList    itemNodeList;
        QDomNode        node;
        QString         value, xml;
        query->evaluateTo(&xml);
        doc.setContent(xml);
        itemNodeList = doc.elementsByTagName("trt:Configurations");
        for (int i = 0; i < itemNodeList.size(); i++) {
            node  = itemNodeList.at(i);
            value = node.toElement().attribute("token");
            audioEncoderConfigurations->setToken(value.trimmed());
        }

        query->evaluateTo(&items);
        item = items.next();
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setName(value.trimmed());

            query->setQuery(result->nameSpace() + "./tt:UseCount/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setUseCount(value.trimmed().toInt());

            query->setQuery(result->nameSpace() + "./tt:Encoding/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setEncoding(value.trimmed());

            query->setQuery(result->nameSpace() + "./tt:Bitrate/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setBitrate(value.trimmed().toInt());

            query->setQuery(result->nameSpace() + "./tt:SampleRate/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setSampleRate(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() +
                "./tt:Multicast/tt:Address/tt:Type/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setType(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:Multicast/tt:Address/tt:IPv4Address/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setIpv4Address(value.trimmed());

            query->setQuery(
                result->nameSpace() +
                "./tt:Multicast/tt:Address/tt:IPv6Address/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setIpv6Address(value.trimmed());

            query->setQuery(
                result->nameSpace() + "./tt:Multicast/tt:Port/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setPort(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:Multicast/tt:TTL/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setTtl(value.trimmed().toInt());

            query->setQuery(
                result->nameSpace() + "./tt:Multicast/tt:AutoStart/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setAutoStart(
                value.trimmed() == "true" ? true : false);

            query->setQuery(
                result->nameSpace() + "./tt:SessionTimeout/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurations->setSessionTimeout(value.trimmed());
            item = items.next();
        }
        delete result;
    }
    delete msg;
    return audioEncoderConfigurations;
}

VideoSourceConfiguration*
MediaManagement::getVideoSourceConfiguration(QString name) {
    VideoSourceConfiguration* videoSourceConfiguration = NULL;
    Message*                  msg                      = newMessage();
    QDomElement               token =
        newElement("wsdl:ConfigurationToken", name);
    QDomElement body = newElement("wsdl:GetVideoSourceConfiguration");
    body.appendChild(token);
    msg->appendToBody(body);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        videoSourceConfiguration = new VideoSourceConfiguration();
        QXmlQuery*   query       = result->query();
        QString      value, xml;
        QDomDocument doc;
        QDomNodeList itemNodeList;
        QDomNode     node;
        QRect        rect;
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Configuration");
        query->evaluateTo(&xml);
        doc.setContent(xml);
        itemNodeList = doc.elementsByTagName("trt:Configuration");
        for (int i = 0; i < itemNodeList.size(); i++) {
            node  = itemNodeList.at(i);
            value = node.toElement().attribute("token");
            videoSourceConfiguration->setToken(value.trimmed());
        }

        videoSourceConfiguration->setName(
            result->getValue("//tt:Name").trimmed());
        videoSourceConfiguration->setUseCount(
            result->getValue("//tt:UseCount").trimmed().toInt());
        videoSourceConfiguration->setSourceToken(
            result->getValue("//tt:SourceToken").trimmed());

        itemNodeList = doc.elementsByTagName("tt:Bounds");
        for (int i = 0; i < itemNodeList.size(); i++) {
            node  = itemNodeList.at(i);
            value = node.toElement().attribute("height");
            rect.setHeight(value.trimmed().toInt());
            value = node.toElement().attribute("width");
            rect.setWidth(value.trimmed().toInt());
            value = node.toElement().attribute("y");
            rect.setLeft(value.trimmed().toInt());
            value = node.toElement().attribute("x");
            rect.setTop(value.trimmed().toInt());
        }
        videoSourceConfiguration->setBounds(rect);
        delete result;
    }
    delete msg;
    return videoSourceConfiguration;
}

VideoEncoderConfiguration*
MediaManagement::getVideoEncoderConfiguration() {
    VideoEncoderConfiguration* videoEncoderConfiguration = NULL;
    Message*                   msg                       = newMessage();
    QDomElement                token =
        newElement("wsdl:ConfigurationToken", "profile_video_stream_D1");
    QDomElement body = newElement("wsdl:GetVideoEncoderConfiguration");
    body.appendChild(token);
    msg->appendToBody(body);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        videoEncoderConfiguration = new VideoEncoderConfiguration();
        QXmlQuery*   query        = result->query();
        QString      value, xml;
        QDomDocument doc;
        QDomNodeList itemNodeList;
        QDomNode     node;
        QRect        rect;
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Configuration");
        query->evaluateTo(&xml);
        doc.setContent(xml);
        itemNodeList = doc.elementsByTagName("trt:Configuration");
        for (int i = 0; i < itemNodeList.size(); i++) {
            node  = itemNodeList.at(i);
            value = node.toElement().attribute("token");
            videoEncoderConfiguration->setToken(value.trimmed());
        }
        videoEncoderConfiguration->setName(
            result->getValue("//tt:Name").trimmed());
        videoEncoderConfiguration->setUseCount(
            result->getValue("//tt:UseCount").trimmed().toInt());
        videoEncoderConfiguration->setEncoding(
            result->getValue("//tt:Encoding").trimmed());
        videoEncoderConfiguration->setWidth(
            result->getValue("//tt:Resolution/tt:Width").trimmed().toInt());
        videoEncoderConfiguration->setHeight(
            result->getValue("//tt:Resolution/tt:Height").trimmed().toInt());
        videoEncoderConfiguration->setQuality(
            result->getValue("//tt:Quality").trimmed().toInt());
        videoEncoderConfiguration->setFrameRateLimit(
            result->getValue("//tt:RateControl/tt:FrameRateLimit")
                .trimmed()
                .toInt());
        videoEncoderConfiguration->setEncodingInterval(
            result->getValue("//tt:RateControl/tt:EncodingInterval")
                .trimmed()
                .toInt());
        videoEncoderConfiguration->setBitrateLimit(
            result->getValue("//tt:RateControl/tt:BitrateLimit")
                .trimmed()
                .toInt());
        videoEncoderConfiguration->setGovLength(
            result->getValue("//tt:H264/tt:GovLength").trimmed().toInt());
        videoEncoderConfiguration->setH264Profile(
            result->getValue("//tt:H264/tt:H264Profile").trimmed());
        videoEncoderConfiguration->setType(
            result->getValue("//tt:Multicast/tt:Address/tt:Type").trimmed());
        videoEncoderConfiguration->setIpv4Address(
            result->getValue("//tt:Multicast/tt:Address/tt:IPv4Address")
                .trimmed());
        videoEncoderConfiguration->setIpv6Address(
            result->getValue("//tt:Multicast/tt:Address/tt:IPv6Address")
                .trimmed());
        videoEncoderConfiguration->setPort(
            result->getValue("//tt:Multicast/tt:Port").trimmed().toInt());
        videoEncoderConfiguration->setTtl(
            result->getValue("//tt:Multicast/tt:TTL").trimmed().toInt());
        videoEncoderConfiguration->setAutoStart(
            result->getValue("//tt:Multicast/tt:AutoStart") == "true" ? true
                                                                      : false);
        videoEncoderConfiguration->setSessionTimeout(
            result->getValue("//tt:SessionTimeout").trimmed());
        delete result;
    }

    delete msg;
    return videoEncoderConfiguration;
}

AudioEncoderConfiguration*
MediaManagement::getAudioEncoderConfiguration() {
    AudioEncoderConfiguration* audioEncoderConfiguration = NULL;
    Message*                   msg                       = newMessage();
    QDomElement                token =
        newElement("wsdl:ConfigurationToken", "profile_audio_stream_1");
    QDomElement body = newElement("wsdl:GetAudioEncoderConfiguration");
    body.appendChild(token);
    msg->appendToBody(body);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        audioEncoderConfiguration = new AudioEncoderConfiguration();
        QXmlQuery*   query        = result->query();
        QString      value, xml;
        QDomDocument doc;
        QDomNodeList itemNodeList;
        QDomNode     node;
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//trt:Configuration");
        query->evaluateTo(&xml);
        doc.setContent(xml);
        itemNodeList = doc.elementsByTagName("trt:Configuration");
        for (int i = 0; i < itemNodeList.size(); i++) {
            node  = itemNodeList.at(i);
            value = node.toElement().attribute("token");
            audioEncoderConfiguration->setToken(value.trimmed());
        }
        audioEncoderConfiguration->setName(
            result->getValue("//tt:Name").trimmed());
        audioEncoderConfiguration->setUseCount(
            result->getValue("//tt:UseCount").trimmed().toInt());
        audioEncoderConfiguration->setEncoding(
            result->getValue("//tt:Encoding").trimmed());
        audioEncoderConfiguration->setBitrate(
            result->getValue("//tt:Bitrate").trimmed().toInt());
        audioEncoderConfiguration->setSampleRate(
            result->getValue("//tt:SampleRate").trimmed().toInt());
        audioEncoderConfiguration->setType(
            result->getValue("//tt:Multicast/tt:Address/tt:Type").trimmed());
        audioEncoderConfiguration->setIpv4Address(
            result->getValue("//tt:Multicast/tt:Address/tt:IPv4Address")
                .trimmed());
        audioEncoderConfiguration->setIpv6Address(
            result->getValue("//tt:Multicast/tt:Address/tt:IPv6Address")
                .trimmed());
        audioEncoderConfiguration->setPort(
            result->getValue("//tt:Multicast/tt:Port").trimmed().toInt());
        audioEncoderConfiguration->setTtl(
            result->getValue("//tt:Multicast/tt:TTL").trimmed().toInt());
        audioEncoderConfiguration->setAutoStart(
            result->getValue("//tt:Multicast/tt:AutoStart").trimmed() == "true"
                ? true
                : false);
        audioEncoderConfiguration->setSessionTimeout(
            result->getValue("//tt:SessionTimeout").trimmed());
        delete result;
    }
    delete msg;
    return audioEncoderConfiguration;
}

AudioEncoderConfigurationOptions*
MediaManagement::getAudioEncoderConfigurationOptions() {
    AudioEncoderConfigurationOptions* audioEncoderConfigurationOptions = NULL;
    Message*                          msg = newMessage();
    QDomElement                       configurationToken =
        newElement("wsdl:ConfigurationToken", "profile_audio_stream_1");
    QDomElement profileToken = newElement("wsdl:ProfileToken", "profile_CIF");
    QDomElement body = newElement("wsdl:GetAudioEncoderConfigurationOptions");
    body.appendChild(configurationToken);
    body.appendChild(profileToken);
    msg->appendToBody(body);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        audioEncoderConfigurationOptions =
            new AudioEncoderConfigurationOptions();
        QXmlQuery*      query = result->query();
        QXmlResultItems items, items1;
        QXmlItem        item, item1;
        QList<int>      bitrateList, sampleRateList;
        QString         value;
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//tt:Options");
        query->evaluateTo(&items);
        item = items.next();
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Encoding/string()");
            query->evaluateTo(&value);
            audioEncoderConfigurationOptions->setEncoding(
                audioEncoderConfigurationOptions->stringToEnum(
                    value.trimmed()));

            query->setQuery(result->nameSpace() + "./tt:BitrateList/tt:Items");
            query->evaluateTo(&items1);
            item1 = items1.next();
            while (!item1.isNull()) {
                query->setFocus(item1);
                query->setQuery(result->nameSpace() + "./string()");
                query->evaluateTo(&value);
                bitrateList.push_back(value.trimmed().toInt());
                item1 = items1.next();
            }
            audioEncoderConfigurationOptions->setBitrateList(bitrateList);
            query->setQuery(
                result->nameSpace() + "../../tt:SampleRateList/tt:Items");
            query->evaluateTo(&items1);
            item1 = items1.next();
            while (!item1.isNull()) {
                query->setFocus(item1);
                query->setQuery(result->nameSpace() + "./string()");
                query->evaluateTo(&value);
                sampleRateList.push_back(value.trimmed().toInt());
                item1 = items1.next();
            }
            audioEncoderConfigurationOptions->setSampleRateList(sampleRateList);
            item = items.next();
        }
        delete result;
    }

    delete msg;
    return audioEncoderConfigurationOptions;
}

VideoEncoderConfigurationOptions*
MediaManagement::getVideoEncoderConfigurationOptions(
    QString _configToken, QString _profileToken) {
    VideoEncoderConfigurationOptions* videoEncoderConfigurationOptions = NULL;
    Message*                          msg = newMessage();
    //    QDomElement configurationToken =
    //    newElement("wsdl:ConfigurationToken","profile_VideoSource_1");
    //    QDomElement profileTokekn =
    //    newElement("wsdl:ProfileToken","profile_CIF");
    //    QDomElement body =
    //    newElement("wsdl:GetVideoEncoderConfigurationOptions");
    QDomElement configurationToken =
        newElement("wsdl:ConfigurationToken", _configToken);
    QDomElement profileTokekn = newElement("wsdl:ProfileToken", _profileToken);
    QDomElement body = newElement("wsdl:GetVideoEncoderConfigurationOptions");
    body.appendChild(configurationToken);
    body.appendChild(profileTokekn);
    msg->appendToBody(body);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        videoEncoderConfigurationOptions =
            new VideoEncoderConfigurationOptions();
        QXmlQuery*      query = result->query();
        QXmlResultItems items;
        QXmlItem        item;
        QString         value;
        videoEncoderConfigurationOptions->setQualityRangeMin(
            result->getValue("//tt:QualityRange/tt:Min").trimmed().toInt());
        videoEncoderConfigurationOptions->setQulityRangeMax(
            result->getValue("//tt:QualityRange/tt:Max").trimmed().toInt());

        query->setQuery(
            result->nameSpace() +
            "doc($inputDocument)//tt:H264/tt:ResolutionsAvailable");
        query->evaluateTo(&items);
        item = items.next();
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Width/string()");
            query->evaluateTo(&value);
            videoEncoderConfigurationOptions->setResAvailableWidthH264(
                value.trimmed().toInt());
            query->setQuery(result->nameSpace() + "./tt:Height/string()");
            query->evaluateTo(&value);
            videoEncoderConfigurationOptions->setResAvailableHeightH264(
                value.trimmed().toInt());
            item = items.next();
        }

        query->setQuery(
            result->nameSpace() +
            "doc($inputDocument)//tt:JPEG/tt:ResolutionsAvailable");
        query->evaluateTo(&items);
        item = items.next();
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Width/string()");
            query->evaluateTo(&value);
            videoEncoderConfigurationOptions->setResAvailableWidthJpeg(
                value.trimmed().toInt());
            query->setQuery(result->nameSpace() + "./tt:Height/string()");
            query->evaluateTo(&value);
            videoEncoderConfigurationOptions->setResAvailableHeightJpeg(
                value.trimmed().toInt());
            item = items.next();
        }

        videoEncoderConfigurationOptions->setGovLengthRangeMin(
            result->getValue("//trt:Options/tt:H264/tt:GovLengthRange/tt:Min")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setGovLengthRangeMax(
            result->getValue("//trt:Options/tt:H264/tt:GovLengthRange/tt:Max")
                .trimmed()
                .toInt());

        videoEncoderConfigurationOptions->setFrameRateRangeMinJpeg(
            result->getValue("//trt:Options/tt:JPEG/tt:FrameRateRange/tt:Min")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setFrameRateRangeMaxJpeg(
            result->getValue("//trt:Options/tt:JPEG/tt:FrameRateRange/tt:Max")
                .trimmed()
                .toInt());

        videoEncoderConfigurationOptions->setFrameRateRangeMinH264(
            result->getValue("//trt:Options/tt:H264/tt:FrameRateRange/tt:Min")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setFrameRateRangeMaxH264(
            result->getValue("//trt:Options/tt:H264/tt:FrameRateRange/tt:Max")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setBitRateRangeMin(
            result->getValue("//tt:H264/tt:BitrateRange/tt:Min")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setBitRateRangeMax(
            result->getValue("//tt:H264/tt:BitrateRange/tt:Max")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setEncodingIntervalRangeMinJpeg(
            result
                ->getValue(
                    "//trt:Options/tt:JPEG/tt:EncodingIntervalRange/tt:Min")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setEncodingIntervalRangeMaxJpeg(
            result
                ->getValue(
                    "//trt:Options/tt:JPEG/tt:EncodingIntervalRange/tt:Max")
                .trimmed()
                .toInt());

        videoEncoderConfigurationOptions->setEncodingIntervalRangeMinH264(
            result
                ->getValue(
                    "//trt:Options/tt:H264/tt:EncodingIntervalRange/tt:Min")
                .trimmed()
                .toInt());
        videoEncoderConfigurationOptions->setEncodingIntervalRangeMaxH264(
            result
                ->getValue(
                    "//trt:Options/tt:H264/tt:EncodingIntervalRange/tt:Max")
                .trimmed()
                .toInt());

        query->setQuery(result->nameSpace() + "../tt:H264ProfilesSupported");
        query->evaluateTo(&items);
        item = items.next();
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./string()");
            query->evaluateTo(&value);
            videoEncoderConfigurationOptions->setH264ProfilesSupported(
                videoEncoderConfigurationOptions->stringToEnum(
                    value.trimmed()));
            item = items.next();
        }
        delete result;
    }

    delete msg;
    return videoEncoderConfigurationOptions;
}

void
MediaManagement::setVideoEncoderConfiguration(
    VideoEncoderConfiguration* videoConfigurations) {
    Message* msg = newMessage();
    msg->appendToBody(videoConfigurations->toxml());
    MessageParser* result = sendMessage(msg);
    videoConfigurations->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetVideoEncoderConfigurationResponse") ||
            result->find(
                "//*[local-name() = 'SetVideoEncoderConfigurationResponse']"))
            videoConfigurations->setResult(true);
        else
            videoConfigurations->setResult(false);
        delete result;
    }
    delete msg;
}

StreamUri*
MediaManagement::getStreamUri(const QString& token) {
    StreamUri*  streamUri    = NULL;
    Message*    msg          = newMessage();
    QDomElement stream       = newElement("sch:Stream", "RTP-Unicast");
    QDomElement transport    = newElement("sch:Transport");
    QDomElement protocol     = newElement("sch:Protocol", "RTSP");
    QDomElement tunnel       = newElement("sch:Tunnel");
    QDomElement streamSetup  = newElement("wsdl:StreamSetup");
    QDomElement getStreamUri = newElement("wsdl:GetStreamUri");
    QDomElement profileToken = newElement("wsdl:ProfileToken", token);
    getStreamUri.setAttribute("xmlns", "http://www.onvif.org/ver10/media/wsdl");
    transport.setAttribute("xmlns", "http://www.onvif.org/ver10/media/wsdl");
    stream.setAttribute("xmlns", "http://www.onvif.org/ver10/media/wsdl");


    getStreamUri.appendChild(streamSetup);
    getStreamUri.appendChild(profileToken);
    streamSetup.appendChild(stream);
    streamSetup.appendChild(transport);
    transport.appendChild(protocol);
    // transport.appendChild(tunnel);
    msg->appendToBody(getStreamUri);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        streamUri = new StreamUri();
        streamUri->setUri(result->getValue("//tt:Uri").trimmed());
        streamUri->setInvalidAfterConnect(
            result->getValue("//tt:InvalidAfterConnect").trimmed() == "true"
                ? true
                : false);
        streamUri->setInvalidAfterReboot(
            result->getValue("//tt:InvalidAfterReboot").trimmed() == "true"
                ? true
                : false);
        streamUri->setTimeout(result->getValue("//tt:Timeout").trimmed());
        delete result;
    }
    delete msg;
    return streamUri;
}

ImageSetting*
MediaManagement::getImageSetting(const QString& token) {
    ImageSetting* imageSetting       = NULL;
    Message*      msg                = newMessage();
    QDomElement   getImagingSettings = newElement("GetImagingSettings");
    getImagingSettings.setAttribute(
        "xmlns", "http://www.onvif.org/ver20/imaging/wsdl");
    QDomElement videoSourceToken = newElement("VideoSourceToken", token);
    getImagingSettings.appendChild(videoSourceToken);

    msg->appendToBody(getImagingSettings);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        imageSetting = new ImageSetting();

        imageSetting->setBrightness(
            result->getValue("//timg:ImagingSettings/tt:Brightness").toInt());

        imageSetting->setColorSaturation(
            result->getValue("//timg:ImagingSettings/tt:ColorSaturation")
                .toInt());

        imageSetting->setContrast(
            result->getValue("//timg:ImagingSettings/tt:Contrast").toInt());

        imageSetting->setSharpness(
            result->getValue("//timg:ImagingSettings/tt:Sharpness").toInt());

        imageSetting->setExposureManual(
            result->getValue("//timg:ImagingSettings/tt:Exposure/tt:Mode") ==
                    "MANUAL"
                ? true
                : false);

        imageSetting->setExposureIris(
            result->getValue("//timg:ImagingSettings/tt:Exposure/tt:Iris")
                .toInt());
        imageSetting->setAutofocusManual(
            result->getValue(
                "//timg:ImagingSettings/tt:Focus/tt:AutoFocusMode") == "MANUAL"
                ? true
                : false);
        imageSetting->setDefaultSpeed(
            result->getValue("//timg:ImagingSettings/tt:Focus/tt:DefaultSpeed")
                .toInt());
        delete result;
    }
    delete msg;
    return imageSetting;
}

ImageSettingOptions*
MediaManagement::getImageSettingOptions(const QString& token) {
    ImageSettingOptions* imageSettingOptions       = NULL;
    Message*             msg                       = newMessage();
    QDomElement          getImagingSettingsOptions = newElement("GetOptions");
    getImagingSettingsOptions.setAttribute(
        "xmlns", "http://www.onvif.org/ver20/imaging/wsdl");
    QDomElement videoSourceToken = newElement("VideoSourceToken", token);
    getImagingSettingsOptions.appendChild(videoSourceToken);

    msg->appendToBody(getImagingSettingsOptions);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        imageSettingOptions = new ImageSettingOptions();
        imageSettingOptions->setBrightnessMin(
            result->getValue("//timg:ImagingOptions/tt:Brightness/tt:Min")
                .toDouble());
        imageSettingOptions->setBrightnessMax(
            result->getValue("//timg:ImagingOptions/tt:Brightness/tt:Max")
                .toDouble());

        imageSettingOptions->setColorSaturationMin(
            result->getValue("//timg:ImagingOptions/tt:ColorSaturation/tt:Min")
                .toDouble());
        imageSettingOptions->setColorSaturationMax(
            result->getValue("//timg:ImagingOptions/tt:ColorSaturation/tt:Max")
                .toDouble());
        imageSettingOptions->setContrastMin(
            result->getValue("//timg:ImagingOptions/tt:Contrast/tt:Min")
                .toDouble());

        imageSettingOptions->setContrastMax(
            result->getValue("//timg:ImagingOptions/tt:Contrast/tt:Max")
                .toDouble());

        imageSettingOptions->setSharpnessMin(
            result->getValue("//timg:ImagingOptions/tt:Sharpness/tt:Min")
                .toDouble());

        imageSettingOptions->setSharpnessMax(
            result->getValue("//timg:ImagingOptions/tt:Sharpness/tt:Max")
                .toDouble());

        imageSettingOptions->setExposureIrisMin(
            result->getValue("//timg:ImagingOptions/tt:Exposure/tt:Iris/tt:Min")
                .toDouble());

        imageSettingOptions->setExposureIrisMax(
            result->getValue("//timg:ImagingOptions/tt:Exposure/tt:Iris/tt:Max")
                .toDouble());

        imageSettingOptions->setDefaultSpeedMin(
            result
                ->getValue(
                    "//timg:ImagingOptions/tt:Focus/tt:DefaultSpeed/tt:Min")
                .toDouble());

        imageSettingOptions->setDefaultSpeedMax(
            result
                ->getValue(
                    "//timg:ImagingOptions/tt:Focus/tt:DefaultSpeed/tt:Max")
                .toDouble());
        delete result;
    }
    delete msg;
    return imageSettingOptions;
}

ImageStatus*
MediaManagement::getImageStatus(const QString& token) {
    ImageStatus* imageStatus = new ImageStatus();
    imageStatus->setToken(token);
    Message*     msg = newMessage();
    msg->appendToBody(imageStatus->toxml());
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        if (result->find("//timg:GetStatusResponse"))
            imageStatus->setResult(true);
        else
            imageStatus->setResult(false);

        imageStatus->setPosition(result->getValue("//tt:FocusStatus20/tt:Position").trimmed().toFloat());

        auto moveStatus = result->getValue("//tt:FocusStatus20/tt:MoveStatus").trimmed();
        if (moveStatus == "MOVING")
            imageStatus->setMoveStatus(ONVIF::MoveStatus::MOVING);
        if (moveStatus == "IDLE")
            imageStatus->setMoveStatus(ONVIF::MoveStatus::IDLE);
        if (moveStatus == "UNKNOWN")
            imageStatus->setMoveStatus(ONVIF::MoveStatus::UNKNOWN);

        imageStatus->setError(result->getValue("//tt:Error").trimmed());
        delete result;
    }
    delete msg;
    return imageStatus;
}

MoveOptions*
MediaManagement::getFocusMoveOptions(const QString& token) {
    MoveOptions* opt = new MoveOptions;
    opt->setToken(token);
    Message* msg = newMessage();
    msg->appendToBody(opt->toxml());
    MessageParser* result = sendMessage(msg);
    if (result != NULL)
    {
        if (result->find("//tt:Absolute/tt:Position/tt:Min"))
            opt->setAbsolutePositionMin(result->getValue("//tt:Absolute/tt:Position/tt:Min").trimmed().toFloat());
        if (result->find("//tt:Absolute/tt:Position/tt:Max"))
            opt->setAbsolutePositionMin(result->getValue("//tt:Absolute/tt:Position/tt:Max").trimmed().toFloat());
        if (result->find("//tt:Relative/tt:Distance/tt:Min"))
            opt->setAbsolutePositionMin(result->getValue("//tt:Relative/tt:Distance/tt:Min").trimmed().toFloat());
        if (result->find("//tt:Relative/tt:Distance/tt:Max"))
            opt->setAbsolutePositionMin(result->getValue("//tt:Relative/tt:Distance/tt:Max").trimmed().toFloat());
        if (result->find("//tt:Continuous/tt:Speed/tt:Min"))
            opt->setAbsolutePositionMin(result->getValue("//tt:Continuous/tt:Speed/tt:Min").trimmed().toFloat());
        if (result->find("//tt:Continuous/tt:Speed/tt:Max"))
            opt->setAbsolutePositionMin(result->getValue("//tt:Continuous/tt:Speed/tt:Max").trimmed().toFloat());
        delete result;
    }
    delete msg;
    return opt;
}

void
MediaManagement::setImageSettings(ImageSetting* imageSettings) {
    Message* msg = newMessage();
    msg->appendToBody(imageSettings->toxml());
    MessageParser* result = sendMessage(msg);
    imageSettings->setResult(false);
    if (result != NULL) {
        if (result->find("//timg:SetImagingSettingsResponse") ||
            result->find(
                "//*[local-name() = 'SetImagingSettingsResponse']"))
            imageSettings->setResult(true);
        else
            imageSettings->setResult(false);
        delete result;
    }
    delete msg;
}

void
MediaManagement::focusMove(FocusMove* focusMove) {
    Message *msg = newMessage();
    msg->appendToBody(focusMove->toxml());
    MessageParser *result = sendMessage(msg);
    if(result != NULL) {
        if(result->find("//timg:MoveResponse") ||
           result->find("//timg:StopResponse"))
            focusMove->setResult(true);
        else
            focusMove->setResult(false);
        delete result;
    }
    delete msg;
}
