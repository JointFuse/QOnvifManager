#include "absolutemove.h"
#include "message.h"
using namespace ONVIF;

AbsoluteMove::AbsoluteMove(QObject *parent):QObject(parent)
{

}

AbsoluteMove::~AbsoluteMove()
{

}

QDomElement AbsoluteMove::toxml()
{
    QDomElement absoluteMove = newElement("wsdl:AbsoluteMove");
    QDomElement profileToken = newElement("wsdl:ProfileToken",this->profileToken());
    QDomElement position = newElement("wsdl:Position");
    QDomElement positionPanTilt = newElement("sch:PanTilt");
    if (m_x || m_y)
    {
        positionPanTilt.setAttribute("x",this->positionPanTiltX());
        positionPanTilt.setAttribute("y",this->positionPanTiltY());
    }
//    positionPanTilt.setAttribute("space",this->positionPanTiltSpace());
    QDomElement positionZoom = newElement("sch:Zoom");
    if (m_z) positionZoom.setAttribute("x",this->positionZoomX());
//    positionZoom.setAttribute("space",this->positionZoomSpace());
    QDomElement speed = newElement("wsdl:Speed");
//    speedPanTilt.setAttribute("space",this->speedPanTiltSpace());
//    speedZoom.setAttribute("space",this->speedZoomSpace());
    if (m_x || m_y)
    {
        position.appendChild(positionPanTilt);
        QDomElement speedPanTilt = newElement("sch:PanTilt");
        if (m_x) speedPanTilt.setAttribute("x",this->speedPanTiltX());
        if (m_y) speedPanTilt.setAttribute("y",this->speedPanTiltY());
        speed.appendChild(speedPanTilt);
    }
    if (m_z)
    {
        position.appendChild(positionZoom);
        QDomElement speedZoom = newElement("sch:Zoom");
        speedZoom.setAttribute("x",this->speedZoomX());
        speed.appendChild(speedZoom);
    }
    absoluteMove.appendChild(profileToken);
    absoluteMove.appendChild(position);
    absoluteMove.appendChild(speed);
    return absoluteMove;
}
