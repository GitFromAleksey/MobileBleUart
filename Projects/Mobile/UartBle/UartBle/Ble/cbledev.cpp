#include "cbledev.h"

cBleDev::cBleDev(QObject *parent) : QObject(parent)
{
    m_BleDiscoveryAgent = new cBleDiscoveryDevicesAgent(this);

    connect(m_BleDiscoveryAgent,  &cBleDiscoveryDevicesAgent::eventSignal,
            this,           &cBleDev::DiscoveryAgentEventHandler);
}
// ----------------------------------------------------------------------------
void cBleDev::StartFindDevices(void)
{
    if(m_BleDiscoveryAgent)
        m_BleDiscoveryAgent->StartDiscovery();
}
// ----------------------------------------------------------------------------
void cBleDev::DiscoveryAgentEventHandler(const cBleDiscoveryDevicesAgent::Events &e)
{
    switch (e)
    {
        case cBleDiscoveryDevicesAgent::Events::e_deviceUpdated:

            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoveredNewDevice:

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
