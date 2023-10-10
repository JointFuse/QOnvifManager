#ifndef STATUS_H
#define STATUS_H
#include <QObject>
#include <QDomElement>
#include <QDateTime>
namespace ONVIF {
    class Status : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString profileToken READ profileToken WRITE setProfileToken)
        Q_PROPERTY(float position READ position WRITE setPosition)
        Q_PROPERTY(QString moveStatus READ moveStatus WRITE setMoveStatus)
        Q_PROPERTY(QString error READ error WRITE setError)

        Q_PROPERTY(bool result READ result WRITE setResult)
    public:
        explicit Status(QObject *parent = NULL);
        virtual ~Status();
        QDomElement toxml();
        QString profileToken() const
        {
            return m_profileToken;
        }

        float position() const
        {
            return m_position;
        }

        bool result() const
        {
            return m_result;
        }

        QString error() const {
            return m_error;
        }

        QString moveStatus() const {
            return m_moveStatus;
        }

    public slots:
        void setProfileToken(QString arg)
        {
            m_profileToken = arg;
        }

        void setPosition(float arg)
        {
            m_position = arg;
        }

        void setResult(bool arg)
        {
            m_result = arg;
        }

        void setError(QString arg) {
            m_error = arg;
        }

        void setMoveStatus(QString arg) {
            m_moveStatus = arg;
        }

    private:
        QString m_profileToken;
        float m_position;
        QString m_moveStatus;
        QString m_error;
        bool m_result;
    };
}
#endif // STATUS_H
