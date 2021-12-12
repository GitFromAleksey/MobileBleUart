#ifndef CBLEDEV_H
#define CBLEDEV_H

#include <qobject.h>
#include "Ble/cblediscoverydevicesagent.h"

class cBleDev : public QObject
{
    Q_OBJECT
public:
    enum events
    {
      e_FindNewDevice,
      e_FindDeviceFinish
    };

    explicit cBleDev(QObject *parent = nullptr);

    void StartFindDevices(void);

private slots:

    void DiscoveryAgentEventHandler(cBleDiscoveryDevicesAgent::Events &e);

signals:

private:
    cBleDiscoveryDevicesAgent *m_BleDiscoveryAgent;
};

#endif // CBLEDEV_H
