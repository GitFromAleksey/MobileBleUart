#include "cbledevice.h"

cBleDevice::cBleDevice() :
    p_bda(nullptr),
    p_qtb(nullptr)
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
                this, &cBleDevice::StartDiscovereDevices);

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
}

void cBleDevice::DeviceScanFinishedCallback()
{

}

void cBleDevice::SetScanResultsCallback(QTextBrowser *qtb)
{
   this->p_qtb = qtb;
}

void cBleDevice::DeviceDiscoveredCallback(const QBluetoothDeviceInfo &device)
{
    QString name;
    // If device is LowEnergy-device, add it to the list
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        name = device.name();
        p_qtb->setText(name);
//        qDebug() << "rssi" << device.rssi() << device.isCached() << name
//                 << device.address();

//        QMap<QString, TionDeviceBaseLogic*>::iterator iter = Devices.find(name);
//        if(iter == Devices.end())
//        {
//            return;
//        }

//        Q*String label = QString("%1 %2 rssi %3").arg(device.address().toString()).
//                                                arg(name).arg(device.rssi());
//        Q*List<QListWidgetItem *> items = ui->devicesList->findItems(label,
//                                                                    Qt::MatchExactly);

//        if (items.empty())
//        {
//            QListWidgetItem *item = new QListWidgetItem(label);

//            QBluetoothLocalDevice::Pairing pairingStatus =
//                                        localDevice->pairingStatus(device.address());
//            if (pairingStatus == QBluetoothLocalDevice::Paired ||
//                    pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
//                //item->setTextColor(QColor(Qt::green));
//                item->setForeground(QColor(Qt::green));
//            else
//                //item->setTextColor(QColor(Qt::black));
//                item->setForeground(QColor(Qt::black));
//            ui->devicesList->addItem(item);
//        }
    }
}

//void DeviceSelector::addDevice(const QBluetoothDeviceInfo &device)
//{
//    // If device is LowEnergy-device, add it to the list
//    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
//    {
//        QString name = device.name();
//        qDebug() << "rssi" << device.rssi() << device.isCached() << name
//                 << device.address();

//        QMap<QString, TionDeviceBaseLogic*>::iterator iter = Devices.find(name);
//        if(iter == Devices.end())
//        {
//            return;
//        }

//        QString label = QString("%1 %2 rssi %3").arg(device.address().toString()).
//                                                arg(name).arg(device.rssi());
//        QList<QListWidgetItem *> items = ui->devicesList->findItems(label,
//                                                                    Qt::MatchExactly);

//        if (items.empty())
//        {
//            QListWidgetItem *item = new QListWidgetItem(label);

//            QBluetoothLocalDevice::Pairing pairingStatus =
//                                        localDevice->pairingStatus(device.address());
//            if (pairingStatus == QBluetoothLocalDevice::Paired ||
//                    pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
//                //item->setTextColor(QColor(Qt::green));
//                item->setForeground(QColor(Qt::green));
//            else
//                //item->setTextColor(QColor(Qt::black));
//                item->setForeground(QColor(Qt::black));
//            ui->devicesList->addItem(item);
//        }
//    }
//}
