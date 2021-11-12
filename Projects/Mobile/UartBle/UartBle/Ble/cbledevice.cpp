#include "cbledevice.h"

cBleDevice::cBleDevice() :
    m_DevsDicoveriAgent(nullptr),
    m_LocalDevice(new QBluetoothLocalDevice(this)),
    printNameOfDevice(nullptr)
{
    connect(m_LocalDevice,  &QBluetoothLocalDevice::pairingFinished ,
            this,           &cBleDevice::PairingFinished);

    connect(m_LocalDevice,  &QBluetoothLocalDevice::deviceConnected,
            this,           &cBleDevice::DeviceConnected);

    connect(m_LocalDevice,  &QBluetoothLocalDevice::deviceConnected,
            this,           &cBleDevice::DeviceDisconnected);

//    connect(m_LocalDevice,
//            SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing))
//            ,this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));


}

void cBleDevice::SetupBluetoothDeviceDiscoveryAgent()
{
    if(m_DevsDicoveriAgent == nullptr)
    {
        m_DevsDicoveriAgent = new QBluetoothDeviceDiscoveryAgent();
        m_DevsDicoveriAgent->setLowEnergyDiscoveryTimeout(10000);

        connect(m_DevsDicoveriAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                this, &cBleDevice::DeviceDiscoveredCallback);
        connect(m_DevsDicoveriAgent, &QBluetoothDeviceDiscoveryAgent::finished,
                this, &cBleDevice::DeviceScanFinishedCallback);

//        connect(ui->devicesList, SIGNAL(itemPressed(QListWidgetItem*)),
//                this, SLOT(deviceSelected(QListWidgetItem*)));

    }
}

void cBleDevice::SetupLowEnergyController(const QBluetoothAddress &address)
{
    for(auto it = m_DevsList.begin(); it != m_DevsList.end(); ++it)
    {
        if(it->address() == address)
        {
            m_control = QLowEnergyController::createCentral(*it);

            connect(m_control, &QLowEnergyController::serviceDiscovered,
                    this, &cBleDevice::ServiceDiscovered);
            connect(m_control, &QLowEnergyController::discoveryFinished,
                    this, &cBleDevice::ServiceDiscoveryFinished);

            connect(m_control, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error) > (&QLowEnergyController::error),
                    this, [this](QLowEnergyController::Error error)
            {
                Q_UNUSED(error);
//                setError("Cannot connect to remote device.");
            });
            connect(m_control, &QLowEnergyController::connected, this, [this]()
            {
//                setInfo("Controller connected. Search services...");
                m_control->discoverServices();
            });
            connect(m_control, &QLowEnergyController::disconnected, this, [this]()
            {
//                setError("LowEnergy controller disconnected");
            });
        }
    }
}

void cBleDevice::ServiceDiscovered(const QBluetoothUuid &newService)
{
    printNameOfDevice(newService.toString());
}
void cBleDevice::ServiceDiscoveryFinished()
{
    printNameOfDevice("Service DiscoveryFinished");
}

void cBleDevice::StartDiscovereDevices()
{
    if(m_DevsDicoveriAgent == nullptr)
        SetupBluetoothDeviceDiscoveryAgent();
    m_DevsDicoveriAgent->stop();
    m_DevsDicoveriAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    m_DevsList.clear();
    clearDevList();
}

void cBleDevice::DeviceScanFinishedCallback()
{
    printNameOfDevice("Scan Finished");
}

bool cBleDevice::SetCurrentDeviceByName(const QString &name)
{
    m_DevsDicoveriAgent->stop();
    for(auto it = m_DevsList.begin(); it != m_DevsList.end(); ++it)
    {
        if(name == it->name())
        {
            QBluetoothDeviceInfo di = static_cast<QBluetoothDeviceInfo>(*it);

            m_LocalDevice->requestPairing(di.address(), QBluetoothLocalDevice::Paired);

            return true;
        }
    }
    return false;
}

QString cBleDevice::GetDevInfoByName(const QString &name)
{
    QString dev_info;

    for(auto it = m_DevsList.begin(); it != m_DevsList.end(); ++it)
    {
        if(name == it->name())
        {
            QBluetoothDeviceInfo di = static_cast<QBluetoothDeviceInfo>(*it);

            dev_info.append(di.name());
            dev_info.append("; ");
            dev_info.append(it->address().toString());
//        //        name.append(it->deviceUuid())
            dev_info.append(QString(it->rssi()));
        }
    }
    return dev_info;
}

void cBleDevice::SetScanResultsCallback(void (*printNameOfDevice)(const QString &text))
{
   this->printNameOfDevice = printNameOfDevice;
}

void cBleDevice::SetClearDevListCallback(void (*clearDevList)())
{
    this->clearDevList = clearDevList;
}

void cBleDevice::DeviceDiscoveredCallback(const QBluetoothDeviceInfo &device)
{
    // If device is LowEnergy-device, add it to the list
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        m_DevsList.append(device);
        printNameOfDevice(device.name());
    }
}
void cBleDevice::DeviceConnected(const QBluetoothAddress &address)
{
    printNameOfDevice("DeviceConnected: " + address.toString());

    SetupLowEnergyController(address);

    m_control->connectToDevice();
}
void cBleDevice::DeviceDisconnected(const QBluetoothAddress &address)
{
    printNameOfDevice("DeviceDisconnected: " + address.toString());
}
void cBleDevice::PairingFinished(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing)
{
    QString text;

    text.append("device is pair:");
    text.append(address.toString());
    printNameOfDevice(text);
}
