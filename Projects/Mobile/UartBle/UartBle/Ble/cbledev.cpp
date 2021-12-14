#include "cbledev.h"

cBleDev::cBleDev(QObject *parent) : QObject(parent)
{
    m_BleDiscoveryAgent = new cBleDiscoveryDevicesAgent(this);

    connect(m_BleDiscoveryAgent,  &cBleDiscoveryDevicesAgent::eventSignal,
            this,           &cBleDev::slotDiscoveryAgentEventHandler);

    // настройка слотов BLE устройства
    connect(&m_CurrentDevice,  &QBluetoothLocalDevice::pairingFinished ,
            this,           &cBleDev::slotPairingFinished);
    connect(&m_CurrentDevice,  &QBluetoothLocalDevice::deviceConnected,
            this,           &cBleDev::slotDeviceConnected);
    connect(&m_CurrentDevice,  &QBluetoothLocalDevice::deviceConnected,
            this,           &cBleDev::slotDeviceDisconnected);
//    connect(m_LocalDevice,
//            SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing))
//            ,this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
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
    m_BleDiscoveryAgent->StopDiscovery();
    m_CurrentDevInfo = m_BleDiscoveryAgent->GetDeviceInfoByAddress(address);
    m_CurrentDevice.requestPairing(m_CurrentDevInfo.address(), QBluetoothLocalDevice::Paired);
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
void cBleDev::slotPairingFinished(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing)
{
    QString text;

    text.append("m_LocalDevice PairingFinished:");
    switch (pairing)
    {
        case QBluetoothLocalDevice::Pairing::AuthorizedPaired:
            text.append(" AuthorizedPaired; ");
            break;
        case QBluetoothLocalDevice::Pairing::Paired:
            text.append(" Paired; ");
            break;
        case QBluetoothLocalDevice::Pairing::Unpaired:
            text.append(" Unpaired; ");
            break;
        default:
            break;
    }
    text.append(address.toString());
//    printNameOfDevice(text);
}
void cBleDev::slotDeviceConnected(const QBluetoothAddress &address)
{
//    printNameOfDevice("m_LocalDevice DeviceConnected: " + address.toString());

    m_BleController.SetBleDevice(m_CurrentDevInfo);

//    m_control->connectToDevice();
}
void cBleDev::slotDeviceDisconnected(const QBluetoothAddress &address)
{
//    printNameOfDevice("m_LocalDevice DeviceDisconnected: " + address.toString());
}
