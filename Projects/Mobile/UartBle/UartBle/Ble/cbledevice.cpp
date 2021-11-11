#include "cbledevice.h"

cBleDevice::cBleDevice() :
    p_bda(nullptr),
    printNameOfDevice(nullptr)
{
}

void cBleDevice::SetupBluetoothDeviceDiscoveryAgent()
{
    if(p_bda == nullptr)
    {
        p_bda = new QBluetoothDeviceDiscoveryAgent();
        p_bda->setLowEnergyDiscoveryTimeout(20000);

        connect(p_bda, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                this, &cBleDevice::DeviceDiscoveredCallback);
        connect(p_bda, &QBluetoothDeviceDiscoveryAgent::finished,
                this, &cBleDevice::DeviceScanFinishedCallback);

//        connect(ui->devicesList, SIGNAL(itemPressed(QListWidgetItem*)),
//                this, SLOT(deviceSelected(QListWidgetItem*)));
    }
}

void cBleDevice::StartDiscovereDevices()
{
    if(p_bda == nullptr)
        SetupBluetoothDeviceDiscoveryAgent();
    p_bda->stop();
    p_bda->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    m_DevsList.clear();
    clearDevList();
}

void cBleDevice::DeviceScanFinishedCallback()
{
    printNameOfDevice("Scan Finished");
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
