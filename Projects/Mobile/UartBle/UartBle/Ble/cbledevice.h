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
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>


class cBleDevice : public QObject
{
public:
    cBleDevice();  

    void StartDiscovereDevices();

    void SetScanResultsCallback(QTextBrowser *qtb);

private:
    QBluetoothDeviceDiscoveryAgent *p_bda;
    QTextBrowser *p_qtb;

    void SetupBluetoothDeviceDiscoveryAgent();

    void DeviceDiscoveredCallback(const QBluetoothDeviceInfo &device);
    void DeviceScanFinishedCallback();
};

#endif // CBLEDEVICE_H
