#include "cbledevice_old.h"

cBleDevice::cBleDevice() :
    m_DevsDicoveriAgent(nullptr),
    m_LocalDevice(new QBluetoothLocalDevice(this)),
    printNameOfDevice(nullptr)
{
//    connect(m_LocalDevice,  &QBluetoothLocalDevice::pairingFinished ,
//            this,           &cBleDevice::PairingFinished);

//    connect(m_LocalDevice,  &QBluetoothLocalDevice::deviceConnected,
//            this,           &cBleDevice::DeviceConnected);

//    connect(m_LocalDevice,  &QBluetoothLocalDevice::deviceConnected,
//            this,           &cBleDevice::DeviceDisconnected);

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
                printNameOfDevice("Cannot connect to remote device.");
            });
            connect(m_control, &QLowEnergyController::connected, this, [this]()
            {
//                setInfo("Controller connected. Search services...");
                printNameOfDevice("Controller connected. Discover services...");
                m_control->discoverServices();
            });
            connect(m_control, &QLowEnergyController::disconnected, this, [this]()
            {
//                setError("LowEnergy controller disconnected");
                printNameOfDevice("LowEnergy controller disconnected");
            });
        }
    }
}

void cBleDevice::ServiceDiscovered(const QBluetoothUuid &newService)
{
    printNameOfDevice("- ServiceDiscovered:" + newService.toString() + ";");

//    if(newService == QBluetoothUuid(QBluetoothUuid::)
    if(newService == QBluetoothUuid(QBluetoothUuid::ServiceDiscoveryServer)){printNameOfDevice("ServiceDiscoveryServer");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::BrowseGroupDescriptor)){printNameOfDevice("BrowseGroupDescriptor");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup)){printNameOfDevice("PublicBrowseGroup");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::SerialPort)){printNameOfDevice("SerialPort");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::LANAccessUsingPPP)){printNameOfDevice("LANAccessUsingPPP");}//0x1102,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DialupNetworking)){printNameOfDevice("DialupNetworking");}//0x1103,
    else if(newService == QBluetoothUuid(QBluetoothUuid::IrMCSync)){printNameOfDevice("IrMCSync");}//0x1104,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ObexObjectPush)){printNameOfDevice("ObexObjectPush");}//0x1105,
    else if(newService == QBluetoothUuid(QBluetoothUuid::OBEXFileTransfer)){printNameOfDevice("OBEXFileTransfer");}//0x1106,
    else if(newService == QBluetoothUuid(QBluetoothUuid::IrMCSyncCommand)){printNameOfDevice("IrMCSyncCommand");}//0x1107,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Headset)){printNameOfDevice("Headset");}//0x1108,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AudioSource)){printNameOfDevice("AudioSource");}//0x110a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AudioSink)){printNameOfDevice("AudioSink");}//0x110b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AV_RemoteControlTarget)){printNameOfDevice("AV_RemoteControlTarget");}//0x110c,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AdvancedAudioDistribution)){printNameOfDevice("AdvancedAudioDistribution");}//0x110d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AV_RemoteControl)){printNameOfDevice("AV_RemoteControl");}//0x110e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AV_RemoteControlController)){printNameOfDevice("AV_RemoteControlController");}//0x110f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HeadsetAG)){printNameOfDevice("HeadsetAG");}//0x1112,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PANU)){printNameOfDevice("PANU");}//0x1115,
    else if(newService == QBluetoothUuid(QBluetoothUuid::NAP)){printNameOfDevice("NAP");}//0x1116,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GN)){printNameOfDevice("GN");}//0x1117,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DirectPrinting)){printNameOfDevice("DirectPrinting");}//0x1118,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ReferencePrinting)){printNameOfDevice("ReferencePrinting");}//0x1119,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BasicImage)){printNameOfDevice("BasicImage");}//0x111a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImagingResponder)){printNameOfDevice("ImagingResponder");}//0x111b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImagingAutomaticArchive)){printNameOfDevice("ImagingAutomaticArchive");}//0x111c,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImagingReferenceObjects)){printNameOfDevice("ImagingReferenceObjects");}//0x111d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Handsfree)){printNameOfDevice("Handsfree");}//0x111e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HandsfreeAudioGateway)){printNameOfDevice("HandsfreeAudioGateway");}//0x111f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DirectPrintingReferenceObjectsService)){printNameOfDevice("DirectPrintingReferenceObjectsService");}//0x1120,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ReflectedUI)){printNameOfDevice("ReflectedUI");}//0x1121,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BasicPrinting)){printNameOfDevice("BasicPrinting");}//0x1122,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PrintingStatus)){printNameOfDevice("PrintingStatus");}//0x1123,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HumanInterfaceDeviceService)){printNameOfDevice("HumanInterfaceDeviceService");}//0x1124,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HardcopyCableReplacement)){printNameOfDevice("HardcopyCableReplacement");}//0x1125,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HCRPrint)){printNameOfDevice("HCRPrint");}//0x1126,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HCRScan)){printNameOfDevice("HCRScan");}//0x1127,
    else if(newService == QBluetoothUuid(QBluetoothUuid::SIMAccess)){printNameOfDevice("SIMAccess");}//0x112d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhonebookAccessPCE)){printNameOfDevice("PhonebookAccessPCE");}//0x112e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhonebookAccessPSE)){printNameOfDevice("PhonebookAccessPSE");}//0x112f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhonebookAccess)){printNameOfDevice("PhonebookAccess");}//0x1130,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HeadsetHS)){printNameOfDevice("HeadsetHS");}//0x1131,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MessageAccessServer)){printNameOfDevice("MessageAccessServer");}//0x1132,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MessageNotificationServer)){printNameOfDevice("MessageNotificationServer");}//0x1133,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MessageAccessProfile)){printNameOfDevice("MessageAccessProfile");}//0x1134,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GNSS)){printNameOfDevice("GNSS");}//0x1135,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GNSSServer)){printNameOfDevice("GNSSServer");}//0x1136,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Display3D)){printNameOfDevice("Display3D");}//0x1137,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Glasses3D)){printNameOfDevice("Glasses3D");}//0x1138,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Synchronization3D)){printNameOfDevice("Synchronization3D");}//0x1139,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MPSProfile)){printNameOfDevice("MPSProfile");}//0x113a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MPSService)){printNameOfDevice("MPSService");}//0x113b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PnPInformation)){printNameOfDevice("PnPInformation");}//0x1200,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericNetworking)){printNameOfDevice("GenericNetworking");}//0x1201,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericFileTransfer)){printNameOfDevice("GenericFileTransfer");}//0x1202,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericAudio)){printNameOfDevice("GenericAudio");}//0x1203,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericTelephony)){printNameOfDevice("GenericTelephony");}//0x1204,
    else if(newService == QBluetoothUuid(QBluetoothUuid::VideoSource)){printNameOfDevice("VideoSource");}//0x1303,
    else if(newService == QBluetoothUuid(QBluetoothUuid::VideoSink)){printNameOfDevice("VideoSink");}//0x1304,
    else if(newService == QBluetoothUuid(QBluetoothUuid::VideoDistribution)){printNameOfDevice("VideoDistribution");}//0x1305,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HDP)){printNameOfDevice("HDP");}//0x1400,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HDPSource)){printNameOfDevice("HDPSource");}//0x1401,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HDPSink)){printNameOfDevice("HDPSink");}//0x1402,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericAccess)){printNameOfDevice("GenericAccess");}//0x1800,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericAttribute)){printNameOfDevice("GenericAttribute");}//0x1801,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImmediateAlert)){printNameOfDevice("ImmediateAlert");}//0x1802,
    else if(newService == QBluetoothUuid(QBluetoothUuid::LinkLoss)){printNameOfDevice("LinkLoss");}//0x1803,
    else if(newService == QBluetoothUuid(QBluetoothUuid::TxPower)){printNameOfDevice("TxPower");}//0x1804,
    else if(newService == QBluetoothUuid(QBluetoothUuid::CurrentTimeService)){printNameOfDevice("CurrentTimeService");}//0x1805,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ReferenceTimeUpdateService)){printNameOfDevice("ReferenceTimeUpdateService");}//0x1806,
    else if(newService == QBluetoothUuid(QBluetoothUuid::NextDSTChangeService)){printNameOfDevice("NextDSTChangeService");}//0x1807,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Glucose)){printNameOfDevice("Glucose");}//0x1808,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HealthThermometer)){printNameOfDevice("HealthThermometer");}//0x1809,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DeviceInformation)){printNameOfDevice("DeviceInformation");}//0x180a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HeartRate)){printNameOfDevice("HeartRate");}//0x180d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhoneAlertStatusService)){printNameOfDevice("PhoneAlertStatusService");}//0x180e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BatteryService)){printNameOfDevice("BatteryService");}//0x180f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BloodPressure)){printNameOfDevice("BloodPressure");}//0x1810,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AlertNotificationService)){printNameOfDevice("AlertNotificationService");}//0x1811,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HumanInterfaceDevice)){printNameOfDevice("HumanInterfaceDevice");}//0x1812,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ScanParameters)){printNameOfDevice("ScanParameters");}//0x1813,
    else if(newService == QBluetoothUuid(QBluetoothUuid::RunningSpeedAndCadence)){printNameOfDevice("RunningSpeedAndCadence");}//0x1814,
    else if(newService == QBluetoothUuid(QBluetoothUuid::CyclingSpeedAndCadence)){printNameOfDevice("CyclingSpeedAndCadence");}//0x1816,
    else if(newService == QBluetoothUuid(QBluetoothUuid::CyclingPower)){printNameOfDevice("CyclingPower");}//0x1818,
    else if(newService == QBluetoothUuid(QBluetoothUuid::LocationAndNavigation)){printNameOfDevice("LocationAndNavigation");}//0x1819,
    else if(newService == QBluetoothUuid(QBluetoothUuid::EnvironmentalSensing)){printNameOfDevice("EnvironmentalSensing");}//0x181a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BodyComposition)){printNameOfDevice("BodyComposition");}//0x181b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::UserData)){printNameOfDevice("UserData");}//0x181c,
    else if(newService == QBluetoothUuid(QBluetoothUuid::WeightScale)){printNameOfDevice("WeightScale");}//0x181d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BondManagement)){printNameOfDevice("BondManagement");}//0x181e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ContinuousGlucoseMonitoring)){printNameOfDevice("ContinuousGlucoseMonitoring");}//0x181f
    else
    {
        printNameOfDevice("Unknown service! DiscoverDetails...");
        m_service = m_control->createServiceObject(newService);

        connect(m_service, &QLowEnergyService::stateChanged, this, &cBleDevice::ServiceStateChanged);
        connect(m_service, &QLowEnergyService::characteristicWritten, this, &cBleDevice::CharacteristicWritten);
        connect(m_service, &QLowEnergyService::descriptorRead, this, &cBleDevice::DescriptorRead);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &cBleDevice::CharacteristicChanged);

//void stateChanged(QLowEnergyService::ServiceState newState);
//void characteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value);
//void characteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value);
//void characteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value);
//void descriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value);
//void descriptorWritten(const QLowEnergyDescriptor &info, const QByteArray &value);
//void error(QLowEnergyService::ServiceError error);

        m_service->discoverDetails();

    }
}

void cBleDevice::SendMessage(const QString &text)
{
    QByteArray array = text.toLocal8Bit();

//    m_service->writeCharacteristic(m_WriteCharacteristic, array, QLowEnergyService::WriteWithoutResponse);
    m_service->writeCharacteristic(m_WriteCharacteristic, array, QLowEnergyService::WriteWithResponse);
}
// подтверждение отправки SendMessage
void cBleDevice::CharacteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value)
{

}
// приём данных от BLE устройства
void cBleDevice::CharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    QString s = QString(value);
    printNameOfDevice(s);
}
// ХЗ
void cBleDevice::DescriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value)
{
}

void cBleDevice::ServiceStateChanged(QLowEnergyService::ServiceState newState)
{
    QList<QLowEnergyCharacteristic> hrChar;
    QString tmp_string = "";

    printNameOfDevice("ServiceStateChanged ServiceState:");
    switch (newState)
    {
    case QLowEnergyService::InvalidService:
        printNameOfDevice(" InvalidService");
        break;
    case QLowEnergyService::DiscoveryRequired:  // we know start/end handle but nothing more
        printNameOfDevice(" DiscoveryRequired");
        break;
    //TODO Rename DiscoveringServices -> DiscoveringDetails or DiscoveringService
    case QLowEnergyService::DiscoveringServices:// discoverDetails() called and running
        printNameOfDevice(" DiscoveringServices");
        break;
    case QLowEnergyService::ServiceDiscovered:  // all details have been synchronized
        printNameOfDevice(" ServiceDiscovered");

        hrChar = m_service->characteristics();
        for(auto it = hrChar.begin(); it != hrChar.end(); ++it)
        {
//            if(it->isValid())
            m_NotificationDesc = it->descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

            printNameOfDevice("service name: " + it->name() + "; uuid: " + it->uuid().toString());

                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Unknown)
                    printNameOfDevice("Property Unknown");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Broadcasting)
                    printNameOfDevice("Property Broadcasting");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::WriteNoResponse)
                    printNameOfDevice("Property WriteNoResponse");


                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Indicate)
                    printNameOfDevice("Property Indicate");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::WriteSigned)
                    printNameOfDevice("Property WriteSigned");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::ExtendedProperty)
                    printNameOfDevice("Property ExtendedProperty");

                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Read)
                {
                    m_ReadCharacteristic = *it;
                    tmp_string = "";
                    tmp_string.append("Property Read uuid: ");
                    tmp_string.append(m_ReadCharacteristic.uuid().toString());
                    printNameOfDevice(tmp_string);
                }
//                if(it->properties() & QLowEnergyCharacteristic::PropertyType::WriteNoResponse)
//                {
//                    m_WriteCharacteristic = *it;
//                    printNameOfDevice("Property WriteNoResponse");
//                    m_NotificationDesc = it->descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

//                    if(m_NotificationDesc.isValid())
//                    {
//                        m_service->writeDescriptor(m_NotificationDesc, QByteArray::fromHex("0100"));
//                    }
//                }

                // характеристика для отправки данных BLE устройству
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Write)
                {
                    m_WriteCharacteristic = *it;
                    printNameOfDevice("Property Write");

//                    m_NotificationDesc = it->descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

//                    if(m_NotificationDesc.isValid())
//                    {
//                        m_service->writeDescriptor(m_NotificationDesc, QByteArray::fromHex("0100"));
//                    }
                }

                // Notification - это типа уведомление от BLE устройства, т.е. приём данных
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Notify)
                {
                    m_ReadCharacteristic = *it;

                    //m_NotificationDesc = it->descriptor(m_ReadCharacteristic.uuid());
                    m_NotificationDesc = it->descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

                    // TODO что за 0100? я так и не разобрался
                    m_service->writeDescriptor(m_NotificationDesc, QByteArray::fromHex("0100"));

                    printNameOfDevice("Property Notify");
                }
//                m_service->writeDescriptor()
        }

        break;
    case QLowEnergyService::LocalService:
        printNameOfDevice(" LocalService");
        break;
    default:
        printNameOfDevice(";");
        break;
    }

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

// start pairing to device
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
        for(auto it = m_DevsList.begin(); it != m_DevsList.end(); ++it)
            if(it->address() == device.address())
                return;

        m_DevsList.append(device);
        printNameOfDevice(device.name());
    }
}
void cBleDevice::DeviceConnected(const QBluetoothAddress &address)
{
    printNameOfDevice("m_LocalDevice DeviceConnected: " + address.toString());

    SetupLowEnergyController(address);

    m_control->connectToDevice();
}
void cBleDevice::DeviceDisconnected(const QBluetoothAddress &address)
{
    printNameOfDevice("m_LocalDevice DeviceDisconnected: " + address.toString());
}
void cBleDevice::PairingFinished(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing)
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
    printNameOfDevice(text);
}
