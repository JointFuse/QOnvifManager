#include "devicemanagement.h"
#include <QDebug>

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
    {"wsdl", "http://www.onvif.org/ver10/device/wsdl"},
    {"sch", "http://www.onvif.org/ver10/schema"},
    {"wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"},
    {"wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"},
};

DeviceManagement::DeviceManagement(
    const QString& wsdlUrl, const QString& username, const QString& password)
    : Service(wsdlUrl, username, password) {}

const QHash<QString, QString>&
DeviceManagement::namespaces(const QString& key) {
    Q_UNUSED(key);
    return NAMESPACES;
}

Message*
DeviceManagement::newMessage() {
    return createMessage(NAMESPACES_MESSAGE);
}

QHash<QString, QString>
DeviceManagement::getDeviceInformation() {
    QHash<QString, QString> device_info;
    Message* msg = newMessage();
    msg->appendToBody(newElement("wsdl:GetDeviceInformation"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        device_info.insert("mf", result->getValue("//tds:Manufacturer"));
        device_info.insert("model", result->getValue("//tds:Model"));
        device_info.insert(
            "firmware_version", result->getValue("//tds:FirmwareVersion"));
        device_info.insert(
            "serial_number", result->getValue("//tds:SerialNumber"));
        device_info.insert("hardware_id", result->getValue("//tds:HardwareId"));
        delete result;
    }
    delete msg;
    return device_info;
}

QHash<QString, QString>
DeviceManagement::getDeviceScopes() {
    QHash<QString, QString> device_scopes;
    Message* msg = newMessage();
    msg->appendToBody(newElement(
        "wsdl:GetScopes xmlns=\"http://www.onvif.org/ver10/device/wsdl\""));
    MessageParser* result = sendMessage(msg);

    if (result != NULL) {
        device_scopes.insert(
            "name",
            result->getValue(
                "//tds:GetScopesResponse/tds:Scopes[./"
                "tt:ScopeDef[./text()='Configurable']]/"
                "/tt:ScopeItem[starts-with(text(),'odm:name:') or "
                "starts-with(text(),'onvif://www.onvif.org/name')]"));
        device_scopes.insert(
            "location",
            result->getValue(
                "//tds:GetScopesResponse/tds:Scopes[./"
                "tt:ScopeDef[./text()='Configurable']]/"
                "/tt:ScopeItem[starts-with(text(),'odm:location:') or "
                "starts-with(text(),'onvif://www.onvif.org/location/')]"));

        device_scopes.insert(
            "hardware",
            result
                ->getValue(
                    "//tds:GetScopesResponse/tds:Scopes[./"
                    "tt:ScopeDef[./text()='Fixed']]/"
                    "/tt:ScopeItem[starts-with(text(),'odm:hardware:') or "
                    "starts-with(text(),'onvif://www.onvif.org/hardware/')]")
                .remove(0, 31));
        delete result;
    }

    delete msg;
    return device_scopes;
}

SystemDateAndTime*
DeviceManagement::getSystemDateAndTime() {
    SystemDateAndTime* systemDateAndTime = NULL;
    Message*           msg               = newMessage();
    msg->appendToBody(newElement("wsdl:GetSystemDateAndTime"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        systemDateAndTime = new SystemDateAndTime();
        systemDateAndTime->setProperty(
            "dateTimeType", result->getValue("//tt:DateTimeType"));
        systemDateAndTime->setDaylightSavings(
            result->getValue("//tt:DaylightSavings") == "true");
        systemDateAndTime->setTz(result->getValue("//tt:TimeZone/tt:TZ"));
        systemDateAndTime->setUtcTime(
            result->getValue("//tt:UTCDateTime/tt:Date/tt:Year").toInt(),
            result->getValue("//tt:UTCDateTime/tt:Date/tt:Month").toInt(),
            result->getValue("//tt:UTCDateTime/tt:Date/tt:Day").toInt(),
            result->getValue("//tt:UTCDateTime/tt:Time/tt:Hour").toInt(),
            result->getValue("//tt:UTCDateTime/tt:Time/tt:Minute").toInt(),
            result->getValue("//tt:UTCDateTime/tt:Time/tt:Second").toInt());
        systemDateAndTime->setLocalTime(
            result->getValue("//tt:LocalDateTime/tt:Date/tt:Year").toInt(),
            result->getValue("//tt:LocalDateTime/tt:Date/tt:Month").toInt(),
            result->getValue("//tt:LocalDateTime/tt:Date/tt:Day").toInt(),
            result->getValue("//tt:LocalDateTime/tt:Time/tt:Hour").toInt(),
            result->getValue("//tt:LocalDateTime/tt:Time/tt:Minute").toInt(),
            result->getValue("//tt:LocalDateTime/tt:Time/tt:Second").toInt());
        delete result;
    }

    delete msg;

    return systemDateAndTime;
}

void
DeviceManagement::setSystemDateAndTime(SystemDateAndTime* systemDateAndTime) {
    Message* msg = newMessage();
    msg->appendToBody(systemDateAndTime->toxml());
    MessageParser* result = sendMessage(msg);
    systemDateAndTime->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetSystemDateAndTimeResponse"))
            systemDateAndTime->setResult(true);
        else
            systemDateAndTime->setResult(false);

        delete result;
    }
    delete msg;
}

void
DeviceManagement::setDeviceScopes(SystemScopes* systemScopes) {
    Message* msg = newMessage();
    msg->appendToBody(systemScopes->toxml());
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        if (result->find("//tds:SetScopesResponse"))
            systemScopes->setResult(true);
        else
            systemScopes->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::setSystemFactoryDefault(
    SystemFactoryDefault* systemFactoryDefault) {
    Message* msg = newMessage();
    msg->appendToBody(systemFactoryDefault->toxml());
    MessageParser* result = sendMessage(msg);
    systemFactoryDefault->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetSystemFactoryDefaultResponse"))
            systemFactoryDefault->setResult(true);
        else
            systemFactoryDefault->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::systemReboot(SystemReboot* systemReboot) {
    Message* msg = newMessage();
    msg->appendToBody(systemReboot->toxml());
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        if (result->find("//tds:SystemRebootResponse"))
            systemReboot->setResult(true);
        else
            systemReboot->setResult(false);
        delete result;
    }
    delete msg;
}

Users*
DeviceManagement::getUsers() {
    Users*   user = NULL;
    Message* msg  = newMessage();
    msg->appendToBody(newElement("wsdl:GetUsers"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        user = new Users();

        //        user->setProperty("userName",
        //        result->getValue("//tt:Username"));
        //        user->setProperty("passWord",
        //        result->getValue("//tt:Password"));
        //        user->setProperty("userLevel",
        //        result->getValue("//tt:UserLevel"));
        QXmlQuery* query = result->query();
        query->setQuery(result->nameSpace() + "doc($inputDocument)//tds:User");
        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem item = items.next();
        QString  username, userLevel;
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Username/string()");
            query->evaluateTo(&username);
            user->setUserNames(username.trimmed());

            query->setQuery(result->nameSpace() + "./tt:UserLevel/string()");
            query->evaluateTo(&userLevel);
            QString levelStr = userLevel.trimmed();

            user->setUserLevel(userLevel.trimmed());
            item = items.next();
        }
        delete result;
    }
    delete msg;
    return user;
}

Capabilities*
DeviceManagement::getCapabilitiesPtz() {
    Capabilities* capabilities = NULL;
    Message*      msg          = newMessage();
    QDomElement   cap          = newElement("wsdl:GetCapabilities");
    cap.appendChild(newElement("wsdl:Category", "PTZ"));
    msg->appendToBody(cap);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        capabilities = new Capabilities();
        capabilities->setProperty(
            "ptzXAddr", result->getValue("//tt:PTZ/tt:XAddr"));
        delete result;
    }
    delete msg;
    return capabilities;
}

Capabilities*
DeviceManagement::getCapabilitiesImaging() {
    Capabilities* capabilities = NULL;
    Message*      msg          = newMessage();
    QDomElement   cap          = newElement("wsdl:GetCapabilities");
    cap.appendChild(newElement("wsdl:Category", "Imaging"));
    msg->appendToBody(cap);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        capabilities = new Capabilities();
        capabilities->setProperty(
            "imagingXAddr", result->getValue("//tt:Imaging/tt:XAddr"));
        delete result;
    }
    delete msg;
    return capabilities;
}

Capabilities*
DeviceManagement::getCapabilitiesMedia() {
    Capabilities* capabilities = NULL;
    Message*      msg          = newMessage();
    QDomElement   cap          = newElement("wsdl:GetCapabilities");
    cap.appendChild(newElement("wsdl:Category", "Media"));
    msg->appendToBody(cap);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        capabilities = new Capabilities();
        capabilities->setProperty(
            "mediaXAddr", result->getValue("//tt:Media/tt:XAddr"));
        capabilities->setProperty(
            "rtpMulticast",
            result->getValue("//tt:RTPMulticast") == "true" ? true : false);
        capabilities->setProperty(
            "rtpTcp",
            result->getValue("//tt:RTP_TCP") == "true" ? true : false);
        capabilities->setProperty(
            "rtpRtspTcp",
            result->getValue("//tt:RTP_RTSP_TCP") == "true" ? true : false);
        delete result;
    }
    delete msg;
    return capabilities;
}

Capabilities*
DeviceManagement::getCapabilitiesDevice() {
    Capabilities* capabilities = NULL;
    Message*      msg          = newMessage();
    QDomElement   cap          = newElement("wsdl:GetCapabilities");
    cap.appendChild(newElement("wsdl:Category", "Device"));
    msg->appendToBody(cap);
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        capabilities = new Capabilities();
        capabilities->setProperty(
            "deviceXAddr", result->getValue("//tt:Device/tt:XAddr"));
        capabilities->setProperty(
            "iPFilter",
            result->getValue("//tt:IPFilter") == "true" ? true : false);
        capabilities->setProperty(
            "zeroConfiguration",
            result->getValue("//tt:ZeroConfiguration") == "true" ? true
                                                                 : false);
        capabilities->setProperty(
            "iPVersion6",
            result->getValue("//tt:IPVersion6") == "true" ? true : false);
        capabilities->setProperty(
            "dynDNS", result->getValue("//tt:DynDNS") == "true" ? true : false);
        capabilities->setProperty(
            "discoveryResolve",
            result->getValue("//tt:DiscoveryResolve") == "true" ? true : false);
        capabilities->setProperty(
            "discoveryBye",
            result->getValue("//tt:DiscoveryBye") == "true" ? true : false);
        capabilities->setProperty(
            "remoteDiscovery",
            result->getValue("//tt:RemoteDiscovery") == "true" ? true : false);
        capabilities->setProperty(
            "systemBackup",
            result->getValue("//tt:SystemBackup") == "true" ? true : false);
        capabilities->setProperty(
            "systemLogging",
            result->getValue("//tt:SystemLogging") == "true" ? true : false);
        capabilities->setProperty(
            "firmwareUpgrade",
            result->getValue("//tt:FirmwareUpgrade") == "true" ? true : false);
        capabilities->setProperty(
            "major", result->getValue("//tt:Major").toInt());
        capabilities->setProperty(
            "minor", result->getValue("//tt:Minor").toInt());
        capabilities->setProperty(
            "httpFirmwareUpgrade",
            result->getValue("//tt:HttpFirmwareUpgrade") == "true" ? true
                                                                   : false);
        capabilities->setProperty(
            "httpSystemBackup",
            result->getValue("//tt:HttpSystemBackup") == "true" ? true : false);
        capabilities->setProperty(
            "httpSystemLogging",
            result->getValue("//tt:HttpSystemLogging") == "true" ? true
                                                                 : false);
        capabilities->setProperty(
            "httpSupportInformation",
            result->getValue("//tt:HttpSupportInformation") == "true" ? true
                                                                      : false);
        capabilities->setProperty(
            "inputConnectors",
            result->getValue("//tt:InputConnectors").toInt());
        capabilities->setProperty(
            "relayOutputs", result->getValue("//tt:RelayOutputs").toInt());
        capabilities->setProperty(
            "tls11", result->getValue("//tt:TLS1.1") == "true" ? true : false);
        capabilities->setProperty(
            "tls12", result->getValue("//tt:TLS1.2") == "true" ? true : false);
        capabilities->setProperty(
            "onboardKeyGeneration",
            result->getValue("//tt:OnboardKeyGeneration") == "true" ? true
                                                                    : false);
        capabilities->setProperty(
            "accessPolicyConfig",
            result->getValue("//tt:AccessPolicyConfig") == "true" ? true
                                                                  : false);
        capabilities->setProperty(
            "x509Token",
            result->getValue("//tt:X.509Token") == "true" ? true : false);
        capabilities->setProperty(
            "samlToken",
            result->getValue("//tt:SAMLToken") == "true" ? true : false);
        capabilities->setProperty(
            "kerberosToken",
            result->getValue("//tt:KerberosToken") == "true" ? true : false);
        capabilities->setProperty(
            "relToken",
            result->getValue("//tt:RELToken") == "true" ? true : false);
        capabilities->setProperty(
            "tls10", result->getValue("//tt:TLS1.0") == "true" ? true : false);
        capabilities->setProperty(
            "dot1x", result->getValue("//tt:Dot1x") == "true" ? true : false);
        capabilities->setProperty(
            "remoteUserHanding",
            result->getValue("//tt:RemoteUserHanding") == "true" ? true
                                                                 : false);
        delete result;
    }
    delete msg;
    return capabilities;
}


NetworkInterfaces*
DeviceManagement::getNetworkInterfaces() {
    NetworkInterfaces* networkInterfaces = NULL;
    Message*           msg               = newMessage();
    msg->appendToBody(newElement("wsdl:GetNetworkInterfaces"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        networkInterfaces = new NetworkInterfaces();
        networkInterfaces->setProperty(
            "interfaceToken",
            result->getValue("//tds:NetworkInterfaces/@token"));
        networkInterfaces->setProperty(
            "networkInfacesEnabled",
            result->getValue("//tds:NetworkInterfaces/tt:Enabled"));
        networkInterfaces->setProperty(
            "networkInfacesName", result->getValue("//tt:Name"));
        networkInterfaces->setProperty(
            "hwAaddress", result->getValue("//tt:HwAddress"));
        networkInterfaces->setProperty(
            "mtu", result->getValue("//tt:MTU").toInt());
        networkInterfaces->setProperty(
            "ipv4esult != NULL)Enabled",
            result->getValue("//tt:IPv4/tt:Enabled"));
        networkInterfaces->setProperty(
            "ipv4ManualAddress", result->getValue("//tt:Manual/tt:Address"));
        networkInterfaces->setProperty(
            "ipv4ManualPrefixLength",
            result->getValue("//tt:Manual/tt:PrefixLength").toInt());
        networkInterfaces->setProperty(
            "ipv4LinkLocalAddress",
            result->getValue("//tt:LinkLocal/tt:Address"));
        networkInterfaces->setProperty(
            "ipvLinkLocalPrefixLength",
            result->getValue("//tt:LinkLocal/tt:PrefixLength").toInt());
        networkInterfaces->setProperty(
            "ipv4FromDHCPAddress",
            result->getValue("//tt:FromDHCP/tt:Address"));
        networkInterfaces->setProperty(
            "ipv4FromDHCPPrefixLength",
            result->getValue("//tt:FromDHCP/tt:PrefixLength").toInt());
        networkInterfaces->setProperty(
            "ipv4DHCP", result->getValue("//tt:DHCP"));
        delete result;
    }
    delete msg;
    return networkInterfaces;
}

void
DeviceManagement::setNetworkInterfaces(NetworkInterfaces* networkInterfaces) {
    Message* msg = newMessage();
    msg->appendToBody(networkInterfaces->toxml());
    MessageParser* result = sendMessage(msg);
    networkInterfaces->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetNetworkInterfacesResponse"))
            networkInterfaces->setResult(true);
        else
            networkInterfaces->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::setNetworkProtocols(NetworkProtocols* networkProtocols) {
    Message* msg = newMessage();
    msg->appendToBody(networkProtocols->toxml());
    MessageParser* result = sendMessage(msg);
    networkProtocols->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetNetworkProtocolsResponse"))
            networkProtocols->setResult(true);
        else
            networkProtocols->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::setDefaultGateway(
    NetworkDefaultGateway* networkDefaultGateway) {
    Message* msg = newMessage();
    msg->appendToBody(networkDefaultGateway->toxml());
    MessageParser* result = sendMessage(msg);
    networkDefaultGateway->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetNetworkDefaultGatewayResponse"))
            networkDefaultGateway->setResult(true);
        else
            networkDefaultGateway->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::setDiscoveryMode(NetworkDiscoveryMode* networkDiscoveryMode) {
    Message* msg = newMessage();
    msg->appendToBody(networkDiscoveryMode->toxml());
    MessageParser* result = sendMessage(msg);
    networkDiscoveryMode->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetDiscoveryModeResponse"))
            networkDiscoveryMode->setResult(true);
        else
            networkDiscoveryMode->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::setDNS(NetworkDNS* networkDns) {
    Message* msg = newMessage();
    msg->appendToBody(networkDns->toxml());
    MessageParser* result = sendMessage(msg);
    networkDns->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetDNSResponse"))
            networkDns->setResult(true);
        else
            networkDns->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::setHostname(NetworkHostname* networkHostname) {
    Message* msg = newMessage();
    msg->appendToBody(networkHostname->toxml());
    MessageParser* result = sendMessage(msg);
    networkHostname->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetHostnameResponse"))
            networkHostname->setResult(true);
        else
            networkHostname->setResult(false);
        delete result;
    }
    delete msg;
}

void
DeviceManagement::setNTP(NetworkNTP* networkNtp) {
    Message* msg = newMessage();
    msg->appendToBody(networkNtp->toxml());
    MessageParser* result = sendMessage(msg);
    networkNtp->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:SetNTPResponse"))
            networkNtp->setResult(true);
        else
            networkNtp->setResult(false);
        delete result;
    }
    delete msg;
}

void DeviceManagement::setUsers(Users* users)
{
    Message* msg = newMessage();
    msg->appendToBody(users->toxml());
    MessageParser* result = sendMessage(msg);
    users->setResult(false);
    if (result != NULL) {
        if (result->find("//tds:CreateUsersResponse")||
                result->find("//tds:SetUserResponse"))
            users->setResult(true);
        else
            users->setResult(false);
        delete result;
    }
    delete msg;
}

NetworkProtocols*
DeviceManagement::getNetworkProtocols() {
    NetworkProtocols* networkProtocols = NULL;
    Message*          msg              = newMessage();
    msg->appendToBody(newElement("wsdl:GetNetworkProtocols"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        networkProtocols = new NetworkProtocols();

        QXmlQuery* query = result->query();
        query->setQuery(
            result->nameSpace() + "doc($inputDocument)//tds:NetworkProtocols");
        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem item = items.next();
        QString  protocolsName, protocolsEnabled, protocolsPort;
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Name/string()");
            query->evaluateTo(&protocolsName);
            networkProtocols->setNetworkProtocolsName(protocolsName.trimmed());

            query->setQuery(result->nameSpace() + "./tt:Enabled/string()");
            query->evaluateTo(&protocolsEnabled);
            networkProtocols->setNetworkProtocolsEnabled(
                protocolsEnabled.trimmed() == "true" ? true : false);

            query->setQuery(result->nameSpace() + "./tt:Port/string()");
            query->evaluateTo(&protocolsPort);
            networkProtocols->setNetworkProtocolsPort(
                protocolsPort.trimmed().toInt());
            item = items.next();
        }
        delete result;
    }
    delete msg;
    return networkProtocols;
}
NetworkDefaultGateway*
DeviceManagement::getNetworkDefaultGateway() {
    NetworkDefaultGateway* networkDefaultGateway = NULL;
    Message*               msg                   = newMessage();
    msg->appendToBody(newElement("wsdl:GetNetworkDefaultGateway"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        networkDefaultGateway = new NetworkDefaultGateway();
        networkDefaultGateway->setProperty(
            "ipv4Address",
            result->getValue("//tds:NetworkGateway/tt:IPv4Address"));
        networkDefaultGateway->setProperty(
            "ipv6Address",
            result->getValue("//tds:NetworkGateway/tt:IPv6Address"));
        delete result;
    }
    delete msg;
    return networkDefaultGateway;
}

NetworkDiscoveryMode*
DeviceManagement::getNetworkDiscoverMode() {

    NetworkDiscoveryMode* networkDiscoveryMode = NULL;
    Message*              msg                  = newMessage();
    msg->appendToBody(newElement("wsdl:GetDiscoveryMode"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        networkDiscoveryMode = new NetworkDiscoveryMode();
        networkDiscoveryMode->setProperty(
            "discoveryMode", result->getValue("//tds:DiscoveryMode"));
        delete result;
    }
    delete msg;
    return networkDiscoveryMode;
}

NetworkDNS*
DeviceManagement::getNetworkDNS() {

    NetworkDNS* networkDNS = NULL;
    Message*    msg        = newMessage();
    msg->appendToBody(newElement("wsdl:GetDNS"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        networkDNS = new NetworkDNS();
        networkDNS->setProperty(
            "dhcp", result->getValue("//tds:DNSInformation/tt:FromDHCP"));
        networkDNS->setProperty(
            "searchDomain",
            result->getValue("//tds:DNSInformation/tt:SearchDomain"));

        QXmlQuery* query = result->query();
        query->setQuery(
            result->nameSpace() +
            "doc($inputDocument)//tds:DNSInformation/tt:DNSManual");

        QXmlResultItems items;
        query->evaluateTo(&items);
        QXmlItem item = items.next();
        QString  dnsType, dnsIPv4Address;
        while (!item.isNull()) {
            query->setFocus(item);
            query->setQuery(result->nameSpace() + "./tt:Type/string()");
            query->evaluateTo(&dnsType);
            networkDNS->setManualType(dnsType.trimmed());

            query->setQuery(result->nameSpace() + "./tt:IPv4Address/string()");
            query->evaluateTo(&dnsIPv4Address);
            networkDNS->setIpv4Address(dnsIPv4Address.trimmed());
            item = items.next();
        }
        delete result;
    }
    delete msg;
    return networkDNS;
}

NetworkHostname*
DeviceManagement::getNetworkHostname() {

    NetworkHostname* networkHostname = NULL;
    Message*         msg             = newMessage();
    msg->appendToBody(newElement("wsdl:GetHostname"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        networkHostname = new NetworkHostname();
        networkHostname->setProperty(
            "dhcp", result->getValue("//tds:HostnameInformation/tt:FromDHCP"));
        networkHostname->setProperty(
            "name", result->getValue("//tds:HostnameInformation/tt:Name"));
        delete result;
    }
    delete msg;
    return networkHostname;
}

NetworkNTP*
DeviceManagement::getNetworkNTP() {

    NetworkNTP* networkNTP = NULL;
    Message*    msg        = newMessage();
    msg->appendToBody(newElement("wsdl:GetNTP"));
    MessageParser* result = sendMessage(msg);
    if (result != NULL) {
        networkNTP = new NetworkNTP();
        networkNTP->setProperty(
            "dhcp", result->getValue("//tds:NTPInformation/tt:fromDHCP"));
        networkNTP->setProperty(
            "manualType",
            result->getValue("//tds:NTPInformation/tt:NTPManual/tt:Type"));
        networkNTP->setProperty(
            "name",
            result->getValue("//tds:NTPInformation/tt:NTPManual/tt:DNSname"));
        networkNTP->setProperty(
            "ipv4Address",
            result->getValue(
                "//tds:NTPInformation/tt:NTPManual/tt:IPv4Address"));
        networkNTP->setProperty(
            "ipv6Address",
            result->getValue(
                "//tds:NTPInformation/tt:NTPManual/tt:IPv6Address"));
        delete result;
    }
    delete msg;
    return networkNTP;
}
