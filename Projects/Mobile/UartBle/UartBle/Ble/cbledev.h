#ifndef CBLEDEV_H
#define CBLEDEV_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QtBluetooth/QBluetoothLocalDevice>
#include "Ble/BleDataStructures.h"
#include "Ble/cblediscoverydevicesagent.h"
#include "Ble/cblecontroller.h"

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
    void Connect(const QString &address);
    void Disconnect();

    void TransmitBleData(const QByteArray &data);

    QList<ServiceDescription> GetConnectedDeviceDescribtion();

private slots:

    void slotDiscoveryAgentEventHandler(const cBleDiscoveryDevicesAgent::Events &e);

    // слоты для QBluetoothLocalDevice m_CurrentDevice;
    void slotDeviceConnected(const QBluetoothAddress &address);
    void slotDeviceDisconnected(const QBluetoothAddress &address);
    void slotPairingFinished(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing);

//    void slotLog(const QString &text);
signals:

    void signalReceiveBleData(const QString &uuid, const QByteArray &data);
    void signalSendNewDeviceInfo(const QString name, const QString address);
    void signalLog(const QString &text);

private:
    cBleDiscoveryDevicesAgent *m_BleDiscoveryAgent;
    QBluetoothDeviceInfo m_CurrentDevInfo;
    QBluetoothLocalDevice m_CurrentDevice;
    cBleController m_BleController;
};

#endif // CBLEDEV_H
