#include "focusmove.h"
#include "message.h"
using namespace ONVIF;

FocusMove::FocusMove(QObject *parent):QObject(parent)
{

}

FocusMove::~FocusMove()
{

}

QDomElement FocusMove::toxml()
{
    QDomElement move = newElement("timg:Move");
    QDomElement videoSourceToken = newElement("timg:VideoSourceToken",this->videoSourceToken());
    QDomElement focus = newElement("timg:Focus");
    QDomElement moveType;

    switch(m_moveType) {
    case MoveType::Absolute:
    {
        moveType = newElement("timg:Absolute");
        QDomElement position = newElement("timg:Position");
        position.setAttribute("x", this->position());
        moveType.appendChild(position);
        break;
    }
    case MoveType::Continuous:
    {
        moveType = newElement("timg:Continuous");
        QDomElement distance = newElement("timg:Distance");
        distance.setAttribute("x", this->position());
        moveType.appendChild(distance);
        break;
    }
    case MoveType::Relative:
        moveType = newElement("timg:Relative");
        break;
    default:
        break;
    }

    QDomElement speed = newElement("timg:Speed");
    speed.setAttribute("x", this->speed());
    moveType.appendChild(speed);
    focus.appendChild(moveType);
    move.appendChild(videoSourceToken);
    move.appendChild(focus);
    return move;
}
