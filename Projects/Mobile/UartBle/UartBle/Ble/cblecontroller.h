#ifndef CBLECONTROLLER_H
#define CBLECONTROLLER_H

#include <QObject>
#include <QtBluetooth/QLowEnergyController>
#include "Ble/cbleservice.h"

class cBleController : public QObject
{
    Q_OBJECT
public:
    explicit cBleController(QObject *parent = nullptr);

    void ConnectBleDevice(const QBluetoothDeviceInfo &info);
    void DisconnectBleDevice();

    void TransmitBleData(const QByteArray &data);


    bool IsConnected() const;

signals:

    void signalLog(const QString &text);
    void signalReceiveData(const QString &uuid, const QByteArray &data);

public slots:
    // QLowEnergyController slots
    void slotServiceDiscovered(const QBluetoothUuid &newService);
    void slotServiceDiscoveryFinished();
//    void slotReceiveData(const QByteArray &data);

private:
    QLowEnergyController *m_BleController;
    bool m_IsBleControllerSetup;
    cBleService *m_Service;

    void ControllerSetup();

    void printNameOfDevice(const QString &text);
};

#endif // CBLECONTROLLER_H
