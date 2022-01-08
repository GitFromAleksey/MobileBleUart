#include "cblecontroller.h"

cBleController::cBleController(QObject *parent) :
    QObject(parent),
    m_BleController(nullptr),
    m_IsBleControllerSetup(false),
    m_Service(nullptr)
{

}
// ----------------------------------------------------------------------------
void cBleController::ControllerSetup()
{
    if(m_BleController == nullptr)
        return;

    if(m_IsBleControllerSetup)
        return;

    connect(m_BleController, &QLowEnergyController::serviceDiscovered,
            this, &cBleController::slotServiceDiscovered);
    connect(m_BleController, &QLowEnergyController::discoveryFinished,
            this, &cBleController::slotServiceDiscoveryFinished);

    connect(m_BleController, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error) > (&QLowEnergyController::error),
            this, [this](QLowEnergyController::Error error)
    {
        Q_UNUSED(error);
//                setError("Cannot connect to remote device.");
    });

    connect(m_BleController, &QLowEnergyController::connected, this, [this]()
    {
//                setInfo("Controller connected. Search services...");
        m_BleController->discoverServices();
    });
    connect(m_BleController, &QLowEnergyController::disconnected, this, [this]()
    {
//                setError("LowEnergy controller disconnected");
    });

    m_IsBleControllerSetup = true;
}
// ----------------------------------------------------------------------------
void cBleController::ConnectBleDevice(const QBluetoothDeviceInfo &info)
{
    if(IsConnected())
    {
        PrintLog("ConnectBleDevice(): device is connected: " + info.address().toString());
        return;
    }

    m_BleController = QLowEnergyController::createCentral(info);
    ControllerSetup();
    m_BleController->connectToDevice();
    PrintLog("ConnectBleDevice(): " + info.address().toString());
}
void cBleController::DisconnectBleDevice()
{
    if(m_BleController)
    {
        m_BleController->disconnectFromDevice();
        if(m_Service)
        {
            delete  m_Service;
            m_Service = nullptr;
        }
        delete m_BleController;
        m_BleController = nullptr;

        m_IsBleControllerSetup = false;
        PrintLog("DisconnectBleDevice()");
    }
}
// ----------------------------------------------------------------------------
void cBleController::slotServiceDiscovered(const QBluetoothUuid &newService)
{
    QString logText = "ServiceDiscovered(),";
//    PrintLog("cBleController::ServiceDiscovered:" + newService.toString() + ":");
    logText.append("\nuuid:" + newService.toString() + ",");

//    if(newService == QBluetoothUuid(QBluetoothUuid::)
    if(newService == QBluetoothUuid(QBluetoothUuid::ServiceDiscoveryServer)){logText.append("ServiceDiscoveryServer");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::BrowseGroupDescriptor)){logText.append("BrowseGroupDescriptor");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup)){logText.append("PublicBrowseGroup");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::SerialPort)){logText.append("SerialPort");}
    else if(newService == QBluetoothUuid(QBluetoothUuid::LANAccessUsingPPP)){logText.append("LANAccessUsingPPP");}//0x1102,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DialupNetworking)){logText.append("DialupNetworking");}//0x1103,
    else if(newService == QBluetoothUuid(QBluetoothUuid::IrMCSync)){logText.append("IrMCSync");}//0x1104,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ObexObjectPush)){logText.append("ObexObjectPush");}//0x1105,
    else if(newService == QBluetoothUuid(QBluetoothUuid::OBEXFileTransfer)){logText.append("OBEXFileTransfer");}//0x1106,
    else if(newService == QBluetoothUuid(QBluetoothUuid::IrMCSyncCommand)){logText.append("IrMCSyncCommand");}//0x1107,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Headset)){logText.append("Headset");}//0x1108,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AudioSource)){logText.append("AudioSource");}//0x110a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AudioSink)){logText.append("AudioSink");}//0x110b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AV_RemoteControlTarget)){logText.append("AV_RemoteControlTarget");}//0x110c,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AdvancedAudioDistribution)){logText.append("AdvancedAudioDistribution");}//0x110d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AV_RemoteControl)){logText.append("AV_RemoteControl");}//0x110e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AV_RemoteControlController)){logText.append("AV_RemoteControlController");}//0x110f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HeadsetAG)){logText.append("HeadsetAG");}//0x1112,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PANU)){logText.append("PANU");}//0x1115,
    else if(newService == QBluetoothUuid(QBluetoothUuid::NAP)){logText.append("NAP");}//0x1116,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GN)){logText.append("GN");}//0x1117,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DirectPrinting)){logText.append("DirectPrinting");}//0x1118,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ReferencePrinting)){logText.append("ReferencePrinting");}//0x1119,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BasicImage)){logText.append("BasicImage");}//0x111a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImagingResponder)){logText.append("ImagingResponder");}//0x111b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImagingAutomaticArchive)){logText.append("ImagingAutomaticArchive");}//0x111c,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImagingReferenceObjects)){logText.append("ImagingReferenceObjects");}//0x111d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Handsfree)){logText.append("Handsfree");}//0x111e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HandsfreeAudioGateway)){logText.append("HandsfreeAudioGateway");}//0x111f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DirectPrintingReferenceObjectsService)){logText.append("DirectPrintingReferenceObjectsService");}//0x1120,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ReflectedUI)){logText.append("ReflectedUI");}//0x1121,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BasicPrinting)){logText.append("BasicPrinting");}//0x1122,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PrintingStatus)){logText.append("PrintingStatus");}//0x1123,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HumanInterfaceDeviceService)){logText.append("HumanInterfaceDeviceService");}//0x1124,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HardcopyCableReplacement)){logText.append("HardcopyCableReplacement");}//0x1125,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HCRPrint)){logText.append("HCRPrint");}//0x1126,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HCRScan)){logText.append("HCRScan");}//0x1127,
    else if(newService == QBluetoothUuid(QBluetoothUuid::SIMAccess)){logText.append("SIMAccess");}//0x112d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhonebookAccessPCE)){logText.append("PhonebookAccessPCE");}//0x112e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhonebookAccessPSE)){logText.append("PhonebookAccessPSE");}//0x112f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhonebookAccess)){logText.append("PhonebookAccess");}//0x1130,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HeadsetHS)){logText.append("HeadsetHS");}//0x1131,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MessageAccessServer)){logText.append("MessageAccessServer");}//0x1132,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MessageNotificationServer)){logText.append("MessageNotificationServer");}//0x1133,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MessageAccessProfile)){logText.append("MessageAccessProfile");}//0x1134,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GNSS)){logText.append("GNSS");}//0x1135,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GNSSServer)){logText.append("GNSSServer");}//0x1136,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Display3D)){logText.append("Display3D");}//0x1137,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Glasses3D)){logText.append("Glasses3D");}//0x1138,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Synchronization3D)){logText.append("Synchronization3D");}//0x1139,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MPSProfile)){logText.append("MPSProfile");}//0x113a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::MPSService)){logText.append("MPSService");}//0x113b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PnPInformation)){logText.append("PnPInformation");}//0x1200,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericNetworking)){logText.append("GenericNetworking");}//0x1201,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericFileTransfer)){logText.append("GenericFileTransfer");}//0x1202,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericAudio)){logText.append("GenericAudio");}//0x1203,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericTelephony)){logText.append("GenericTelephony");}//0x1204,
    else if(newService == QBluetoothUuid(QBluetoothUuid::VideoSource)){logText.append("VideoSource");}//0x1303,
    else if(newService == QBluetoothUuid(QBluetoothUuid::VideoSink)){logText.append("VideoSink");}//0x1304,
    else if(newService == QBluetoothUuid(QBluetoothUuid::VideoDistribution)){logText.append("VideoDistribution");}//0x1305,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HDP)){logText.append("HDP");}//0x1400,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HDPSource)){logText.append("HDPSource");}//0x1401,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HDPSink)){logText.append("HDPSink");}//0x1402,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericAccess)){logText.append("GenericAccess");}//0x1800,
    else if(newService == QBluetoothUuid(QBluetoothUuid::GenericAttribute)){logText.append("GenericAttribute");}//0x1801,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ImmediateAlert)){logText.append("ImmediateAlert");}//0x1802,
    else if(newService == QBluetoothUuid(QBluetoothUuid::LinkLoss)){logText.append("LinkLoss");}//0x1803,
    else if(newService == QBluetoothUuid(QBluetoothUuid::TxPower)){logText.append("TxPower");}//0x1804,
    else if(newService == QBluetoothUuid(QBluetoothUuid::CurrentTimeService)){logText.append("CurrentTimeService");}//0x1805,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ReferenceTimeUpdateService)){logText.append("ReferenceTimeUpdateService");}//0x1806,
    else if(newService == QBluetoothUuid(QBluetoothUuid::NextDSTChangeService)){logText.append("NextDSTChangeService");}//0x1807,
    else if(newService == QBluetoothUuid(QBluetoothUuid::Glucose)){logText.append("Glucose");}//0x1808,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HealthThermometer)){logText.append("HealthThermometer");}//0x1809,
    else if(newService == QBluetoothUuid(QBluetoothUuid::DeviceInformation)){logText.append("DeviceInformation");}//0x180a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HeartRate)){logText.append("HeartRate");}//0x180d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::PhoneAlertStatusService)){logText.append("PhoneAlertStatusService");}//0x180e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BatteryService)){logText.append("BatteryService");}//0x180f,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BloodPressure)){logText.append("BloodPressure");}//0x1810,
    else if(newService == QBluetoothUuid(QBluetoothUuid::AlertNotificationService)){logText.append("AlertNotificationService");}//0x1811,
    else if(newService == QBluetoothUuid(QBluetoothUuid::HumanInterfaceDevice)){logText.append("HumanInterfaceDevice");}//0x1812,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ScanParameters)){logText.append("ScanParameters");}//0x1813,
    else if(newService == QBluetoothUuid(QBluetoothUuid::RunningSpeedAndCadence)){logText.append("RunningSpeedAndCadence");}//0x1814,
    else if(newService == QBluetoothUuid(QBluetoothUuid::CyclingSpeedAndCadence)){logText.append("CyclingSpeedAndCadence");}//0x1816,
    else if(newService == QBluetoothUuid(QBluetoothUuid::CyclingPower)){logText.append("CyclingPower");}//0x1818,
    else if(newService == QBluetoothUuid(QBluetoothUuid::LocationAndNavigation)){logText.append("LocationAndNavigation");}//0x1819,
    else if(newService == QBluetoothUuid(QBluetoothUuid::EnvironmentalSensing)){logText.append("EnvironmentalSensing");}//0x181a,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BodyComposition)){logText.append("BodyComposition");}//0x181b,
    else if(newService == QBluetoothUuid(QBluetoothUuid::UserData)){logText.append("UserData");}//0x181c,
    else if(newService == QBluetoothUuid(QBluetoothUuid::WeightScale)){logText.append("WeightScale");}//0x181d,
    else if(newService == QBluetoothUuid(QBluetoothUuid::BondManagement)){logText.append("BondManagement");}//0x181e,
    else if(newService == QBluetoothUuid(QBluetoothUuid::ContinuousGlucoseMonitoring)){logText.append("ContinuousGlucoseMonitoring");}//0x181f
    else
    {
        logText.append("Unknown service");
//        m_Service = new cBleService(nullptr, m_BleController->createServiceObject(newService));

//        connect(m_Service, &cBleService::signalLog,
//                this, &cBleController::signalLog);
//        connect(m_Service, &cBleService::signalReceiveData,
//                this, &cBleController::signalReceiveData);
    }

    m_Service = new cBleService(this, m_BleController->createServiceObject(newService));

    m_ServiceList.append(m_Service);

    connect(m_Service, &cBleService::signalLog,
            this, &cBleController::signalLog);
    connect(m_Service, &cBleService::signalReceiveData,
            this, &cBleController::signalReceiveData);

    m_Service->DiscoverDetails();

    logText.append(".");
    PrintLog(logText);
}
// ----------------------------------------------------------------------------
void cBleController::TransmitBleData(const QByteArray &data)
{
    m_Service->TransmitBleData(data);
}
// ----------------------------------------------------------------------------
QList<ServiceDescription> cBleController::GetServicesDescription()
{
    QList<ServiceDescription> services;
    for(auto it = m_ServiceList.begin(); it != m_ServiceList.end(); ++it)
    {
        services.append( (*it)->GetDescription() );
    }
    return services;
}
// ----------------------------------------------------------------------------
bool cBleController::IsConnected() const
{
    QLowEnergyController::ControllerState state;

    if(!m_BleController)
    {
        return false;
    }

    state = m_BleController->state();
    switch (state)
    {
        case QLowEnergyController::ControllerState::ConnectedState :
            return true;
            break;
        case QLowEnergyController::ControllerState::ConnectingState :
            return false;
            break;
        case QLowEnergyController::ControllerState::UnconnectedState :
            return false;
            break;
        case QLowEnergyController::ControllerState::DiscoveringState :
            return false;
            break;
        case QLowEnergyController::ControllerState::DiscoveredState :
            return true;
            break;
        case QLowEnergyController::ControllerState::ClosingState :
            return false;
            break;
        case QLowEnergyController::ControllerState::AdvertisingState :
            return false;
            break;
        default:
            break;
    }
}
// ----------------------------------------------------------------------------
void cBleController::slotServiceDiscoveryFinished()
{
    PrintLog("slotServiceDiscoveryFinished");

//    for(auto it = m_ServiceList.begin(); it != m_ServiceList.end(); ++it)
//    {
//        (*it)->DiscoverDetails();
//    }
}
// ----------------------------------------------------------------------------
// TODO тестовая функция
void cBleController::PrintLog(const QString &text)
{
    QString s = "- cBleController::";

    s.append(text);

    emit signalLog(s);
}
// ----------------------------------------------------------------------------
//void cBleController::slotReceiveData(const QByteArray &data)
//{
//    emit signalReceiveData(data);
//}
// ----------------------------------------------------------------------------
