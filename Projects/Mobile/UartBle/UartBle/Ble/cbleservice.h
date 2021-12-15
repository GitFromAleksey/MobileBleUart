#ifndef CBLESERVICE_H
#define CBLESERVICE_H

#include <QObject>
#include <QtBluetooth/QLowEnergyService>
#include <QtBluetooth/QLowEnergyDescriptor>

class cBleService : public QObject
{
    Q_OBJECT
public:
    explicit cBleService(QObject *parent = nullptr, QLowEnergyService *service = nullptr);

signals:
    void signalLog(const QString &text);

public slots:
//    connect(m_service, &QLowEnergyService::stateChanged, this, &cBleService::ServiceStateChanged);
//    connect(m_service, &QLowEnergyService::characteristicWritten, this, &cBleService::CharacteristicWritten);
//    connect(m_service, &QLowEnergyService::descriptorRead, this, &cBleService::DescriptorRead);
//    connect(m_service, &QLowEnergyService::characteristicChanged, this, &cBleService::CharacteristicChanged);
    void StateChanged(QLowEnergyService::ServiceState newState);
    void CharacteristicChanged(const QLowEnergyCharacteristic &info,
                               const QByteArray &value);
//    void characteristicRead(const QLowEnergyCharacteristic &info,
//                            const QByteArray &value);
    void CharacteristicWritten(const QLowEnergyCharacteristic &info,
                               const QByteArray &value);
    void DescriptorRead(const QLowEnergyDescriptor &info,
                        const QByteArray &value);
//    void descriptorWritten(const QLowEnergyDescriptor &info,
//                           const QByteArray &value);
//    void error(QLowEnergyService::ServiceError error);
private:
    QLowEnergyService *m_service;
    QLowEnergyCharacteristic m_ReadCharacteristic;
    QLowEnergyCharacteristic m_WriteCharacteristic;

    void printNameOfDevice(const QString &text);
};

#endif // CBLESERVICE_H
