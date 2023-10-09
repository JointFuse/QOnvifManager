#include "status.h"
#include "message.h"
using namespace ONVIF;

Status::Status(QObject *parent):QObject(parent)
{

}

Status::~Status()
{

}

QDomElement Status::toxml()
{
    QDomElement status = newElement("wsdl:GetStatus");
    QDomElement profileToken = newElement("wsdl:ProfileToken",this->profileToken());
    status.appendChild(profileToken);
    return status;
}
