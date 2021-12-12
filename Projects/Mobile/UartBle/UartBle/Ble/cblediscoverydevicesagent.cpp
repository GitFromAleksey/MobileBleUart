#include "cblediscoverydevicesagent.h"


cBleDiscoveryDevicesAgent::cBleDiscoveryDevicesAgent(QObject *parent) :
    QObject(parent),
    m_DevsDicoveryAgent(nullptr),
    eventCalback(nullptr)
{
    m_DevsDicoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    m_DevsDicoveryAgent->setLowEnergyDiscoveryTimeout(10000);

    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &cBleDiscoveryDevicesAgent::DeviceDiscovered);
    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
            this, &cBleDiscoveryDevicesAgent::DeviceUpdated);
    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &cBleDiscoveryDevicesAgent::Finished);
//    connect(m_DevsDicoveriAgent, &QBluetoothDeviceDiscoveryAgent::error,
//            this, &cBleDiscoveryDevicesAgent::Error);
    connect(m_DevsDicoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &cBleDiscoveryDevicesAgent::Canceled);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::SetEventsCallBack(void (*eventCalback)(const Events &e))
{
    this->eventCalback = eventCalback;
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::SendEvent(const Events &e)
{
    if(this->eventCalback == nullptr)
        return;

    this->eventCalback(e);

    eventSignal(e);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::StartDiscovery()
{
    m_DevicesList.clear();

    m_DevsDicoveryAgent->stop();
    m_DevsDicoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::DeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    // If device is LowEnergy-device, add it to the list
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        for(auto it = m_DevicesList.begin(); it != m_DevicesList.end(); ++it)
            if(it->address() == info.address())
                return;

        m_DevicesList.append(info);

        SendEvent(e_DiscoveredNewDevice);
    }
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::DeviceUpdated(const QBluetoothDeviceInfo &info,
                                              QBluetoothDeviceInfo::Fields updatedFields)
{
    SendEvent(e_deviceUpdated);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::Finished()
{
//printNameOfDevice("Scan Finished");
    SendEvent(e_DiscoverFinished);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::Error(QBluetoothDeviceDiscoveryAgent::Error error)
{
    SendEvent(e_DiscoverError);
}
// ----------------------------------------------------------------------------
void cBleDiscoveryDevicesAgent::Canceled()
{
    SendEvent(e_DiscoverCanceled);
}
// ----------------------------------------------------------------------------
