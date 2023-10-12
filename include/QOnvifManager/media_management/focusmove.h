#ifndef FOCUSMOVE_H
#define FOCUSMOVE_H
#include <QObject>
#include <QDomElement>
namespace ONVIF {
    enum class MoveType {
        Absolute,
        Continuous,
        Relative,
        Stop,
    };

    class FocusMove : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString videoSourceToken READ videoSourceToken WRITE setVideoSourceToken)
        Q_PROPERTY(float position READ position WRITE setPosition)
        Q_PROPERTY(float distance READ distance WRITE setDistance)
        Q_PROPERTY(float speed READ speed WRITE setSpeed)
        Q_PROPERTY(int moveType READ moveType WRITE setMoveType)
        Q_PROPERTY(bool result READ result WRITE setResult)
    public:
        explicit FocusMove(QObject *parent = NULL);
        virtual ~FocusMove();
        QDomElement toxml();

        QString videoSourceToken() const
        {
            return m_videoSourceToken;
        }

        float position() const
        {
            return m_position;
        }

        float distance() const
        {
            return m_distance;
        }

        float speed() const
        {
            return m_speed;
        }

        int moveType() const
        {
            return m_moveType;
        }

        bool result() const
        {
            return m_result;
        }

    public slots:
        void setVideoSourceToken(QString arg)
        {
            m_videoSourceToken = arg;
        }

        void setPosition(float arg)
        {
            m_position = arg;
        }

        void setDistance(float arg)
        {
            m_distance = arg;
        }

        void setSpeed(float arg)
        {
            m_speed = arg;
        }

        void setMoveType(int arg)
        {
            m_moveType = arg;
        }

        void setResult(bool arg)
        {
            m_result = arg;
        }

    private:
        QString m_videoSourceToken;
        float m_position;
        float m_distance;
        float m_speed;
        int m_moveType;
        bool m_result;
    };
}
#endif // FOCUSMOVE_H
