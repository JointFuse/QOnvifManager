﻿#include "focusmove.h"
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
    QDomElement move;
    QDomElement videoSourceToken = newElement("timg:VideoSourceToken",this->videoSourceToken());
    QDomElement moveType;

    switch((MoveType)m_moveType) {
    case MoveType::Absolute:
    {
        moveType = newElement("tt:Absolute");
        QDomElement position = newElement("tt:Position",
                                          QString::number(this->position()));
        moveType.appendChild(position);
        break;
    }
    case MoveType::Relative:
    {
        moveType = newElement("tt:Relative");
        QDomElement distance = newElement("tt:Distance",
                                          QString::number(this->distance()));
        moveType.appendChild(distance);
        break;
    }
    case MoveType::Continuous:
        moveType = newElement("tt:Continuous");
        break;
    case MoveType::Stop:
        move = newElement("timg:Stop");
        move.appendChild(videoSourceToken);
        /* this section shorter then whole method */
        return move;
    default:
        break;
    }

    move = newElement("timg:Move");
    QDomElement focus = newElement("timg:Focus");
    QDomElement speed = newElement("tt:Speed", QString::number(this->speed()));
    moveType.appendChild(speed);
    focus.appendChild(moveType);
    move.appendChild(videoSourceToken);
    move.appendChild(focus);
    return move;
}
