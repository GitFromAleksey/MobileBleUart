#ifndef CBLECONTROLLER_H
#define CBLECONTROLLER_H

#include <QObject>
#include <QtBluetooth/QLowEnergyController>

class cBleController : public QObject
{
    Q_OBJECT
public:
    explicit cBleController(QObject *parent = nullptr);

    void SetBleDevice(const QBluetoothDeviceInfo &info);

signals:

public slots:
    // QLowEnergyController slots
    void slotServiceDiscovered(const QBluetoothUuid &newService);
    void slotServiceDiscoveryFinished();

private:
    QLowEnergyController *m_BleController;
    bool m_IsBleControllerSetup;

    void ControllerSetup();

    void printNameOfDevice(const QString &text);
};

#endif // CBLECONTROLLER_H
