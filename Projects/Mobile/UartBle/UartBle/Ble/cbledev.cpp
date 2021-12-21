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
    connect(&m_CurrentDevice,  &QBluetoothLocalDevice::deviceDisconnected,
            this,           &cBleDev::slotDeviceDisconnected);
//    connect(m_LocalDevice,
//            SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing))
//            ,this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));

    connect(&m_BleController, &cBleController::signalLog,
            this, &cBleDev::slotLog );
}
// ----------------------------------------------------------------------------
void cBleDev::StartFindDevices(void)
{
    if(m_BleDiscoveryAgent)
    {
        m_BleDiscoveryAgent->StartDiscovery();
        emit signalLog("cBleDev::StartFindDevices");
    }
}
// ----------------------------------------------------------------------------
void cBleDev::Connect(const QString &address)
{
//    m_BleDiscoveryAgent->StopDiscovery();
//    m_CurrentDevInfo = m_BleDiscoveryAgent->GetDeviceInfoByAddress(address);
//    m_CurrentDevice.requestPairing(m_CurrentDevInfo.address(), QBluetoothLocalDevice::Paired);

    m_BleDiscoveryAgent->StopDiscovery();
    m_CurrentDevInfo = m_BleDiscoveryAgent->GetDeviceInfoByAddress(address);
    m_BleController.ConnectBleDevice(m_CurrentDevInfo);
}
// ----------------------------------------------------------------------------
void cBleDev::Disconnect()
{
    m_CurrentDevice.requestPairing(m_CurrentDevInfo.address(), QBluetoothLocalDevice::Unpaired);
    m_BleController.DisconnectBleDevice();
}
// ----------------------------------------------------------------------------
void cBleDev::slotDiscoveryAgentEventHandler(const cBleDiscoveryDevicesAgent::Events &e)
{
    QString name;
    QString address;

    switch (e)
    {
        case cBleDiscoveryDevicesAgent::Events::e_deviceUpdated:
//            emit signalLog("cBleDev::slotDiscoveryAgentEventHandler: e_deviceUpdated");
            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoveredNewDevice:
            m_BleDiscoveryAgent->GetLastFindingDevice(name, address);
            signalSendNewDeviceInfo(name, address);
            emit signalLog("cBleDev::slotDiscoveryAgentEventHandler: e_DiscoveredNewDevice");
            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoverFinished:
            emit signalLog("cBleDev::slotDiscoveryAgentEventHandler: e_DiscoverFinished");
            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoverCanceled:
            emit signalLog("cBleDev::slotDiscoveryAgentEventHandler: e_DiscoverCanceled");
            break;
        case cBleDiscoveryDevicesAgent::Events::e_DiscoverError:
            emit signalLog("cBleDev::slotDiscoveryAgentEventHandler: e_DiscoverError");
            break;
        default:
            break;
    }
}
// ----------------------------------------------------------------------------
void cBleDev::slotPairingFinished(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing)
{
    switch (pairing)
    {
        case QBluetoothLocalDevice::Pairing::AuthorizedPaired:
            emit signalLog("cBleDev::slotPairingFinished: AuthorizedPaired: " + address.toString());
            break;
        case QBluetoothLocalDevice::Pairing::Paired:
//            m_BleController.SetBleDevice(m_CurrentDevInfo);
            emit signalLog("cBleDev::slotPairingFinished: Paired: " + address.toString());
            break;
        case QBluetoothLocalDevice::Pairing::Unpaired:
            emit signalLog("cBleDev::slotPairingFinished: Unpaired: " + address.toString());
            break;
        default:
            break;
    }
}
// ----------------------------------------------------------------------------
void cBleDev::slotDeviceConnected(const QBluetoothAddress &address)
{
//    if(m_CurrentDevInfo.address() == address)
//    {
//        emit signalLog("cBleDev::slotDeviceConnected: " + address.toString());
//        m_BleController.SetBleDevice(m_CurrentDevInfo);
//    }
}
// ----------------------------------------------------------------------------
void cBleDev::slotDeviceDisconnected(const QBluetoothAddress &address)
{
    emit signalLog("cBleDev::slotDeviceDisconnected: " + address.toString());
}
// ----------------------------------------------------------------------------
void cBleDev::TransmitBleData(const QByteArray &data)
{
    m_BleController.TransmitBleData(data);
}
// ----------------------------------------------------------------------------
void cBleDev::slotLog(const QString &text)
{
    emit signalLog(text);
}
// ----------------------------------------------------------------------------
