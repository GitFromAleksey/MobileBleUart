#ifndef CBLESERVICE_H
#define CBLESERVICE_H

#include <QObject>
#include <QtBluetooth/QLowEnergyService>
#include <QtBluetooth/QLowEnergyDescriptor>
#include "Ble/BleDataStructures.h"


class cBleService : public QObject
{
    Q_OBJECT
public:
    explicit cBleService(QObject *parent = nullptr, QLowEnergyService *service = nullptr);
    ~cBleService();

    void DiscoverDetails();

    void SendMessage(const QString &text);

    void TransmitBleData(const QByteArray &data);

    void TransmitBleDataByUuid(const QString &uuid, const QByteArray &data);

    ServiceDescription GetDescription() const;
    QString GetServiceUuid() const;
    QList<CharacteristicDescription> GetAllServiceCharacteristics() const;
signals:
    void signalReceiveData(const QString &uuid, const QByteArray &data);
    void signalLog(const QString &text);

public slots:
//    connect(m_service, &QLowEnergyService::stateChanged, this, &cBleService::ServiceStateChanged);
//    connect(m_service, &QLowEnergyService::characteristicWritten, this, &cBleService::CharacteristicWritten);
//    connect(m_service, &QLowEnergyService::descriptorRead, this, &cBleService::DescriptorRead);
//    connect(m_service, &QLowEnergyService::characteristicChanged, this, &cBleService::CharacteristicChanged);
    void slotStateChanged(QLowEnergyService::ServiceState newState);
    void slotCharacteristicChanged(const QLowEnergyCharacteristic &info,
                               const QByteArray &value);
//    void characteristicRead(const QLowEnergyCharacteristic &info,
//                            const QByteArray &value);
    void slotCharacteristicWritten(const QLowEnergyCharacteristic &info,
                               const QByteArray &value);
    void slotDescriptorRead(const QLowEnergyDescriptor &info,
                        const QByteArray &value);
//    void descriptorWritten(const QLowEnergyDescriptor &info,
//                           const QByteArray &value);
//    void error(QLowEnergyService::ServiceError error);


private:
    QLowEnergyService *m_service;
    QLowEnergyCharacteristic m_ReadCharacteristic;
    QLowEnergyCharacteristic m_WriteCharacteristic;
    QList<QLowEnergyCharacteristic> m_Characteristics;

    void PrintLog(const QString &text);
};

#endif // CBLESERVICE_H
