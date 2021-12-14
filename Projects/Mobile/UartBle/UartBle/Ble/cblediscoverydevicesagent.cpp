#include "cblediscoverydevicesagent.h"


cBleDiscoveryDevicesAgent::cBleDiscoveryDevicesAgent(QObject *parent) :
    QObject(parent),
    m_DevsDicoveryAgent(nullptr)
{
    m_DevsDicoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    m_DevsDicoveryAgent->setLowEnergyDiscoveryTimeout(10000);

    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &cBleDiscoveryDevicesAgent::DeviceDiscovered);
    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
            this, &cBleDiscoveryDevicesAgent::DeviceUpdated);
    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &cBleDiscoveryDevicesAgent::Finished);
//    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::error, // TODO что-то не так нужно разобраться
//            this, &cBleDiscoveryDevicesAgent::Error);
    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &cBleDiscoveryDevicesAgent::Canceled);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::StartDiscovery()
{
    m_DevicesList.clear();

    m_DevsDicoveryAgent->stop();
    m_DevsDicoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}
// ----------------------------------------------------------------------------
// Обнаружено устройство
void cBleDiscoveryDevicesAgent::DeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    // If device is LowEnergy-device, add it to the list
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        for(auto it = m_DevicesList.begin(); it != m_DevicesList.end(); ++it)
            if(it->address() == info.address())
                return;

        m_DevicesList.append(info);

        emit eventSignal(e_DiscoveredNewDevice);
    }
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::DeviceUpdated(const QBluetoothDeviceInfo &info,
                                              QBluetoothDeviceInfo::Fields updatedFields)
{
    emit eventSignal(e_deviceUpdated);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::Finished()
{
    emit eventSignal(e_DiscoverFinished);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::Error(QBluetoothDeviceDiscoveryAgent::Error error)
{
    emit eventSignal(e_DiscoverError);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::Canceled()
{
    emit eventSignal(e_DiscoverCanceled);
}
// ----------------------------------------------------------------------------
