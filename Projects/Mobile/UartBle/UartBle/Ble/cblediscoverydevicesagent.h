#ifndef CBLEDISCOVERYDEVICESAGENT_H
#define CBLEDISCOVERYDEVICESAGENT_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>

class cBleDiscoveryDevicesAgent : public QObject
{
    Q_OBJECT
public:

    enum Events
    {
        e_DiscoveredNewDevice,
        e_deviceUpdated,
        e_DiscoverFinished,
        e_DiscoverError,
        e_DiscoverCanceled
    };

    explicit cBleDiscoveryDevicesAgent(QObject *parent = nullptr);

    void StartDiscovery();

    QMap<QString, QString> GetFindingDevices();

private slots:
    void DeviceDiscovered(const QBluetoothDeviceInfo &info);
    void DeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    void Finished();
    void Error(QBluetoothDeviceDiscoveryAgent::Error error);
    void Canceled();
signals:
    void eventSignal(const Events &e);
private:
    QBluetoothDeviceDiscoveryAgent *m_DevsDicoveryAgent;
    QList<QBluetoothDeviceInfo> m_DevicesList;

};

#endif // CBLEDISCOVERYDEVICESAGENT_H
