#ifndef ONVIF_DEVICESEARCHER_H
#define ONVIF_DEVICESEARCHER_H

#include "message.h"
#include <set>
#include <queue>
#include <memory>
#include <QObject>
#include <QUdpSocket>
#include <QTimer>

namespace ONVIF {
    class DeviceSearcher : public QObject {
        Q_OBJECT
    public:
        static DeviceSearcher* searcher;
        explicit DeviceSearcher(/*QHostAddress &addr, */QObject *parent = 0);
        static DeviceSearcher* instance(QHostAddress &addr);
        static QList<QHostAddress> getHostAddress();
        ~DeviceSearcher();
        
        void startSearch();
    signals:
        void receiveData(const QHash<QString, QString> &data);
        void deviceSearchingEnded();
    public slots:
    private slots:
        void sendSearchMsg();
        void readPendingDatagrams();
        void processDatagramQueue();
    private:
        std::set<std::unique_ptr<QUdpSocket>> m_sockets;
        std::queue<QByteArray> m_recievedPackets;
        QTimer m_timer;
        uint m_recall;
        std::unique_ptr<Message> msg;

        const static QHash<QString, QString> namespaces;
    };
}
#endif // ONVIF_DEVICESEARCHER_H

