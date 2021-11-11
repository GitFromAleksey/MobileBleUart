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


class cBleDevice : public QObject
{
public:
    cBleDevice();  

    void StartDiscovereDevices();

    QString GetDevInfoByName(const QString &name);

    void SetScanResultsCallback(void (*printNameOfDevice)(const QString &text));
    void SetClearDevListCallback(void (*clearDevList)());

private:
    QBluetoothDeviceDiscoveryAgent *p_bda;
    QList<QBluetoothDeviceInfo> m_DevsList;

    void (*printNameOfDevice)(const QString &text);
    void (*clearDevList)();

    void SetupBluetoothDeviceDiscoveryAgent();

    void DeviceDiscoveredCallback(const QBluetoothDeviceInfo &device);
    void DeviceScanFinishedCallback();
};

#endif // CBLEDEVICE_H
