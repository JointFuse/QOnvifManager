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
        Q_PROPERTY(float positionPanTiltX READ positionPanTiltX WRITE setPositionPanTiltX)
        Q_PROPERTY(float positionPanTiltY READ positionPanTiltY WRITE setPositionPanTiltY)
        Q_PROPERTY(bool moveStatusPanTiltX READ moveStatusPanTiltX WRITE setMoveStatusPanTiltX)
        Q_PROPERTY(bool moveStatusPanTiltY READ moveStatusPanTiltY WRITE setMoveStatusPanTiltY)
        Q_PROPERTY(QString positionPanTiltSpace READ positionPanTiltSpace WRITE setPositionPanTiltSpace)
        Q_PROPERTY(float positionZoomX READ positionZoomX WRITE setPositionZoomX)
        Q_PROPERTY(bool moveStatusZoomX READ moveStatusZoomX WRITE setMoveStatusZoomX)
        Q_PROPERTY(QString positionZoomSpace READ positionZoomSpace WRITE setPositionZoomSpace)
        Q_PROPERTY(QString error READ error WRITE setError)
        Q_PROPERTY(QDateTime utcTime READ utcTime WRITE setutcTime)

        Q_PROPERTY(bool result READ result WRITE setResult)
    public:
        explicit Status(QObject *parent = NULL);
        virtual ~Status();
        QDomElement toxml();
        QString profileToken() const
        {
            return m_profileToken;
        }

        float positionPanTiltX() const
        {
            return m_positionPanTiltX;
        }

        float positionPanTiltY() const
        {
            return m_positionPanTiltY;
        }

        QString positionPanTiltSpace() const
        {
            return m_positionPanTiltSpace;
        }

        float positionZoomX() const
        {
            return m_positionZoomX;
        }

        QString positionZoomSpace() const
        {
            return m_positionZoomSpace;
        }

        bool result() const
        {
            return m_result;
        }

        QDateTime utcTime() const {
            return m_utcTime;
        }

        QString error() const {
            return m_error;
        }

        bool moveStatusPanTiltX() const {
            return m_moveStatusPanTiltX;
        }

        bool moveStatusPanTiltY() const {
            return m_moveStatusPanTiltY;
        }

        bool moveStatusZoomX() const {
            return m_moveStatusZoomX;
        }

    public slots:
        void setProfileToken(QString arg)
        {
            m_profileToken = arg;
        }

        void setPositionPanTiltX(float arg)
        {
            m_positionPanTiltX = arg;
        }

        void setPositionPanTiltY(float arg)
        {
            m_positionPanTiltY = arg;
        }

        void setPositionPanTiltSpace(QString arg)
        {
            m_positionPanTiltSpace = arg;
        }

        void setPositionZoomX(float arg)
        {
            m_positionZoomX = arg;
        }

        void setPositionZoomSpace(QString arg)
        {
            m_positionZoomSpace = arg;
        }

        void setResult(bool arg)
        {
            m_result = arg;
        }

        void setutcTime(const QDateTime& arg) {
            m_utcTime = arg;
        }

        void setError(QString arg) {
            m_error = arg;
        }

        void setMoveStatusPanTiltX(bool arg) {
            m_moveStatusPanTiltX = arg;
        }

        void setMoveStatusPanTiltY(bool arg) {
            m_moveStatusPanTiltY = arg;
        }

        void setMoveStatusZoomX(bool arg) {
            m_moveStatusZoomX = arg;
        }

    private:
        QString m_profileToken;
        float m_positionPanTiltX;
        float m_positionPanTiltY;
        QString m_positionPanTiltSpace;
        float m_positionZoomX;
        QString m_positionZoomSpace;
        bool m_moveStatusPanTiltX;
        bool m_moveStatusPanTiltY;
        bool m_moveStatusZoomX;
        QString m_error;
        QDateTime m_utcTime;
        bool m_result;
    };
}
#endif // STATUS_H
