#include "cbledev.h"

cBleDev::cBleDev(QObject *parent) : QObject(parent)
{
    m_BleDiscoveryAgent = new cBleDiscoveryDevicesAgent(this);

    connect(m_BleDiscoveryAgent,  &cBleDiscoveryDevicesAgent::eventSignal,
            this,           &cBleDev::slotDiscoveryAgentEventHandler);
}
// ----------------------------------------------------------------------------
void cBleDev::StartFindDevices(void)
{
    if(m_BleDiscoveryAgent)
        m_BleDiscoveryAgent->StartDiscovery();
}
// ----------------------------------------------------------------------------
void cBleDev::SetDeviceByAddress(const QString &address)
{
    m_CurrentDevInfo = m_BleDiscoveryAgent->GetDeviceInfoByAddress(address);
}
// ----------------------------------------------------------------------------
void cBleDev::slotDiscoveryAgentEventHandler(const cBleDiscoveryDevicesAgent::Events &e)
{
    QString name;
    QString address;

    switch (e)
    {
        case cBleDiscoveryDevicesAgent::Events::e_deviceUpdated:

            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoveredNewDevice:
            m_BleDiscoveryAgent->GetLastFindingDevice(name, address);
            signalSendNewDeviceInfo(name, address);
            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoverFinished:

            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoverCanceled:

            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoverError:

            break;
        default:
            break;
    }
}
// ----------------------------------------------------------------------------
