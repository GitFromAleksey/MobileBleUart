#ifndef CBLEDEVICE_H
#define CBLEDEVICE_H

//#include "brlt/remotecontroller.h"
//#include "br4s/br4s_mainwindow.h"
//#include "IQ/iq_mainwindow.h"
//#include <QList>
//#include <QString>
//#include <QSettings>
//#include <QWidget>
//#include <QListWidgetItem>

#include <qobject.h>
#include <qtextbrowser.h>
#include <qlist.h>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QLowEnergyController>


class cBleDevice : public QObject
{
public:
    cBleDevice();  

    void StartDiscovereDevices();

    bool SetCurrentDeviceByName(const QString &name);
    QString GetDevInfoByName(const QString &name);

    void SetScanResultsCallback(void (*printNameOfDevice)(const QString &text));
    void SetClearDevListCallback(void (*clearDevList)());

private slots:

    void DeviceConnected(const QBluetoothAddress &address);
    void DeviceDisconnected(const QBluetoothAddress &address);
    void PairingFinished(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing);

    // QLowEnergyController slots
    void ServiceDiscovered(const QBluetoothUuid &newService);
    void ServiceDiscoveryFinished();

private:
    QBluetoothDeviceDiscoveryAgent *m_DevsDicoveriAgent;
    QList<QBluetoothDeviceInfo> m_DevsList;
    QLowEnergyController* m_control;
    QBluetoothLocalDevice *m_LocalDevice;

    void (*printNameOfDevice)(const QString &text);
    void (*clearDevList)();

    void SetupBluetoothDeviceDiscoveryAgent();
    void SetupLowEnergyController(const QBluetoothAddress &address);

    void DeviceDiscoveredCallback(const QBluetoothDeviceInfo &device);
    void DeviceScanFinishedCallback();


};

#endif // CBLEDEVICE_H
