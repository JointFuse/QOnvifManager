#ifndef QONVIFDEVICE_HPP
#define QONVIFDEVICE_HPP

#include "datastruct.hpp"

#include <QDateTime>
#include <QObject>
#include <QScopedPointer>

namespace ONVIF {
class DeviceManagement;
class MediaManagement;
}
///////////////////////////////////////////////////////////////////////////////
namespace device {
///////////////////////////////////////////////////////////////////////////////
class QOnvifDevicePrivate;
class QOnvifDevice : public QObject
{
public:
    QOnvifDevice(
        QString  _serviceAddress,
        QString  _userName,
        QString  _password,
        QObject* _parent);
    ~QOnvifDevice();
    Data& data();
    //date time
    Data::DateTime deviceDateAndTime();
    // device management
    void setDeviceProbeData(Data::ProbeData _probeData);

    bool setDateAndTime(QDateTime _dateAndTime);
    bool setScopes(QString _name, QString _location);

    bool refreshDeviceCapabilities();
    bool refreshDeviceInformation();
    bool refreshDeviceScopes();

    bool resetFactoryDevice();
    bool rebootDevice();

    // media management
    bool refreshVideoConfigs();
    bool refreshAudioConfigs();


    bool refreshProfiles();
    bool refreshInterfaces();

    bool refreshUsers();

private:
    Q_DECLARE_PRIVATE(QOnvifDevice)
    QScopedPointer<QOnvifDevicePrivate> d_ptr;
};
///////////////////////////////////////////////////////////////////////////////
} // namespace device
///////////////////////////////////////////////////////////////////////////////

#endif // QONVIFDEVICE_HPP
