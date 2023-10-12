#include "moveoptions.h"
#include "message.h"
using namespace ONVIF;

MoveOptions::MoveOptions(QObject* parent) : QObject(parent) {}

QDomElement
MoveOptions::toxml() {
    QDomElement imageStatus = newElement("timg:GetMoveOptions");
    QDomElement videoSourceToken = newElement("timg:VideoSourceToken", this->token());
    imageStatus.appendChild(videoSourceToken);
    return imageStatus;
}




