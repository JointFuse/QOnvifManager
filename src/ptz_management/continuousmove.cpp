#include "continuousmove.h"
#include "message.h"
using namespace ONVIF;

ContinuousMove::ContinuousMove(QObject *parent):QObject(parent)
{
    m_panTiltX = 0;
    m_panTiltY = 0;
    m_zoomX = 0;
}

ContinuousMove::~ContinuousMove()
{

}

QDomElement ContinuousMove::toxml()
{
    QDomElement continuousMove = newElement("wsdl:ContinuousMove");
    QDomElement velocity = newElement("wsdl:Velocity");
    QDomElement profileToken = newElement("wsdl:ProfileToken",this->profileToken());
    continuousMove.appendChild(profileToken);
    if (m_panTiltX != 0 || m_panTiltY != 0)
    {
        QDomElement panTilt = newElement("sch:PanTilt");
        if (m_panTiltX != 0) panTilt.setAttribute("x",this->panTiltX());
        if (m_panTiltY != 0) panTilt.setAttribute("y",this->panTiltY());
        panTilt.setAttribute("space",this->panTiltSpace());
        velocity.appendChild(panTilt);
    }
    if (m_zoomX != 0)
    {
        QDomElement zoom = newElement("sch:Zoom");
        zoom.setAttribute("x",this->zoomX());
        zoom.setAttribute("space",this->zoomSpace());
        velocity.appendChild(zoom);
    }
//    QDomElement timeout = newElement("wsdl:Timeout",this->timeout());
//    continuousMove.appendChild(timeout);
    continuousMove.appendChild(velocity);
    return continuousMove;
}


