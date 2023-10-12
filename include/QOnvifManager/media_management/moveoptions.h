#ifndef MOVEOPTIONS_H
#define MOVEOPTIONS_H
#include <QDomElement>
#include <QObject>

namespace ONVIF {
class MoveOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float absolutePositionMin READ absolutePositionMin WRITE setAbsolutePositionMin)
    Q_PROPERTY(float relativeDistanceMin READ relativeDistanceMin WRITE setRelativeDistanceMin)
    Q_PROPERTY(float absoluteSpeedMin READ absoluteSpeedMin WRITE setAbsoluteSpeedMin)
    Q_PROPERTY(float relativeSpeedMin READ relativeSpeedMin WRITE setRelativeSpeedMin)
    Q_PROPERTY(float continuousSpeedMin READ continuousSpeedMin WRITE setContinuousSpeedMin)
    Q_PROPERTY(float absolutePositionMax READ absolutePositionMax WRITE setAbsolutePositionMax)
    Q_PROPERTY(float relativeDistanceMax READ relativeDistanceMax WRITE setRelativeDistanceMax)
    Q_PROPERTY(float absoluteSpeedMax READ absoluteSpeedMax WRITE setAbsoluteSpeedMax)
    Q_PROPERTY(float relativeSpeedMax READ relativeSpeedMax WRITE setRelativeSpeedMax)
    Q_PROPERTY(float continuousSpeedMax READ continuousSpeedMax WRITE setContinuousSpeedMax)
public:
    MoveOptions(QObject* parent = NULL);
    QDomElement toxml();

    float absolutePositionMin() const {
        return m_absolutePositionMin;
    }

    float relativeDistanceMin() const {
        return m_relativeDistanceMin;
    }

    float absoluteSpeedMin() const {
        return m_absoluteSpeedMin;
    }

    float relativeSpeedMin() const {
        return m_relativeSpeedMin;
    }

    float continuousSpeedMin() const {
        return m_continuousSpeedMin;
    }

    float absolutePositionMax() const {
        return m_absolutePositionMax;
    }

    float relativeDistanceMax() const {
        return m_relativeDistanceMax;
    }

    float absoluteSpeedMax() const {
        return m_absoluteSpeedMax;
    }

    float relativeSpeedMax() const {
        return m_relativeSpeedMax;
    }

    float continuousSpeedMax() const {
        return m_continuousSpeedMax;
    }

    void setAbsolutePositionMin(float arg) {
        m_absolutePositionMin = arg;
    }

    void setRelativeDistanceMin(float arg) {
        m_relativeDistanceMin = arg;
    }

    void setAbsoluteSpeedMin(float arg) {
        m_absoluteSpeedMin = arg;
    }

    void setRelativeSpeedMin(float arg) {
        m_relativeSpeedMin = arg;
    }

    void setContinuousSpeedMin(float arg) {
        m_continuousSpeedMin = arg;
    }

    void setAbsolutePositionMax(float arg) {
        m_absolutePositionMax = arg;
    }

    void setRelativeDistanceMax(float arg) {
        m_relativeDistanceMax = arg;
    }

    void setAbsoluteSpeedMax(float arg) {
        m_absoluteSpeedMax = arg;
    }

    void setRelativeSpeedMax(float arg) {
        m_relativeSpeedMax = arg;
    }

    void setContinuousSpeedMax(float arg) {
        m_continuousSpeedMax = arg;
    }

    bool result() const {
        return m_result;
    }

    void setResult(bool result) {
        m_result = result;
    }

    QString token() const {
        return m_token;
    }

    void setToken(const QString &token) {
        m_token = token;
    }

private:
    float m_absolutePositionMin{ 0 };
    float m_relativeDistanceMin{ 0 };
    float m_absoluteSpeedMin{ 0 };
    float m_relativeSpeedMin{ 0 };
    float m_continuousSpeedMin{ 0 };
    float m_absolutePositionMax{ 0 };
    float m_relativeDistanceMax{ 0 };
    float m_absoluteSpeedMax{ 0 };
    float m_relativeSpeedMax{ 0 };
    float m_continuousSpeedMax{ 0 };
    QString m_token;
    bool m_result;
};
}
#endif // MOVEOPTIONS_H
