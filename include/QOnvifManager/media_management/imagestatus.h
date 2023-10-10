#ifndef IMAGESTATUS_H
#define IMAGESTATUS_H
#include <QDomElement>
#include <QObject>

namespace ONVIF {
class ImageStatus : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float position READ position WRITE setPosition)
    Q_PROPERTY(int moveStatus READ moveStatus WRITE setMoveStatus)
    Q_PROPERTY(QString error READ error WRITE setError)
public:
    ImageStatus(QObject* parent = NULL);
    QDomElement toxml();

    float position() const {
        return m_position;
    }

    int moveStatus() const {
        return m_moveStatus;
    }

    QString error() const {
        return m_error;
    }

    bool result() const {
        return m_result;
    }

    void setPosition(float position) {
        m_position = position;
    }

    void setMoveStatus(int status) {
        m_moveStatus = status;
    }

    void setError(QString error) {
        m_error = error;
    }

    void setResult(bool result) {
        m_result = result;
    }

    QString token() const
    {
        return m_token;
    }

    void setToken(const QString &token)
    {
        m_token = token;
    }

private:
    float m_position;
    int m_moveStatus;
    QString m_error;
    QString m_token;
    bool m_result;
};
}
#endif // IMAGESTATUS_H
