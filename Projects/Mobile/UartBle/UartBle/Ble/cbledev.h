#ifndef CBLEDEV_H
#define CBLEDEV_H

#include <QObject>
#include <QMap>
#include <QString>
#include "Ble/cblediscoverydevicesagent.h"
#include <QtBluetooth/QBluetoothLocalDevice>

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
    void SetDeviceByAddress(const QString &address);

private slots:

    void slotDiscoveryAgentEventHandler(const cBleDiscoveryDevicesAgent::Events &e);

    // слоты для QBluetoothLocalDevice m_CurrentDevice;
    void slotDeviceConnected(const QBluetoothAddress &address);
    void slotDeviceDisconnected(const QBluetoothAddress &address);
    void slotPairingFinished(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing);

signals:

    void signalSendNewDeviceInfo(const QString name, const QString address);

private:
    cBleDiscoveryDevicesAgent *m_BleDiscoveryAgent;
    QBluetoothDeviceInfo m_CurrentDevInfo;
    QBluetoothLocalDevice m_CurrentDevice;
};

#endif // CBLEDEV_H
