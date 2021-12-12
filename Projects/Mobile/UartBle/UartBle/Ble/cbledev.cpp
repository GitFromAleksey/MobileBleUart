#include "cbledev.h"

cBleDev::cBleDev(QObject *parent) : QObject(parent)
{
    m_BleDiscoveryAgent = new cBleDiscoveryDevicesAgent(this);

//    connect(m_BleDiscoveryAgent,  &m_BleDiscoveryAgent->SendEvent,
//            this,           &cBleDevice::DeviceDisconnected);
}
// ----------------------------------------------------------------------------
void cBleDev::StartFindDevices(void)
{

}
// ----------------------------------------------------------------------------
void cBleDev::DiscoveryAgentEventHandler(cBleDiscoveryDevicesAgent::Events &e)
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
