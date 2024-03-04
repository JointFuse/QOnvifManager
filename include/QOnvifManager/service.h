#ifndef ONVIF_SERVICE_H
#define ONVIF_SERVICE_H

#include <chrono>
#include <QObject>
#include "message.h"
#include "client.h"
#include "messageparser.h"

namespace ONVIF {
    class Service : public QObject {
        Q_OBJECT
    public:
        explicit Service(const QString & wsdlUrl, const QString &username, const QString &password);
        
        ~Service();
        MessageParser *sendMessage(Message &message, const QString &namespaceKey = "");
        MessageParser *sendMessage(Message *message, const QString &namespaceKey = "", int* sendDuration = nullptr);

        void setUsername(QString arg) noexcept { mUsername = arg; }
        void setPassword(QString arg) noexcept { mPassword = arg; }
        
    protected:
        virtual const QHash<QString, QString>& namespaces(const QString &key) = 0;
        Message *createMessage(const QHash<QString, QString> &namespaces);
#ifndef QT_DEBUG
    private:
#endif
        Client *mClient;
    private:
        std::chrono::system_clock::time_point mLastDeviceTimeUpdate;
        QDateTime mDeviceDateTime;
        QString mUsername, mPassword;
    };
}


#endif // ONVIF_SERVICE_H
