#include "imagestatus.h"
#include "message.h"
using namespace ONVIF;

ImageStatus::ImageStatus(QObject* parent) : QObject(parent) {}

QDomElement
ImageStatus::toxml() {
    QDomElement imageStatus = newElement("timg:GetStatus");
    QDomElement videoSourceToken = newElement("timg:VideoSourceToken", this->token());
    imageStatus.appendChild(videoSourceToken);
    return imageStatus;
}




