#ifndef SYSTEMSCOPES_HPP
#define SYSTEMSCOPES_HPP

#include <QDomElement>
#include <QObject>
#include <QString>

namespace ONVIF {
class SystemScopes : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString location READ location WRITE setLocation)
    Q_PROPERTY(bool result READ result WRITE setResult)
public:
    QDomElement toxml();
    void setScopes(QString name, QString location);
    SystemScopes();

    QString name() const {
        return m_name;
    }

    QString location() const {
        return m_location;
    }

    bool result() const
    {
        return m_result;
    }


public slots:
    void setName(const QString& name) {
        m_name = name;
    }

    void setLocation(const QString& location) {
        m_location = location;
    }
    void setResult(bool result)
    {
        m_result = result;
    }
private:
    QString m_name;
    QString m_location;
    bool m_result;
};
}

#endif // SYSTEMSCOPES_HPP
