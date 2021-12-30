#include "cblecontroller.h"

cBleController::cBleController(QObject *parent) :
    QObject(parent),
    m_BleController(nullptr),
    m_IsBleControllerSetup(false)
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
        emit signalLog("cBleController::ConnectBleDevice: device is connected: " + info.address().toString());
        return;
    }

    m_BleController = QLowEnergyController::createCentral(info);
    ControllerSetup();
    m_BleController->connectToDevice();
    emit signalLog("cBleController::ConnectBleDevice: " + info.address().toString());
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
        emit signalLog("cBleController::DisconnectBleDevice");
    }
}
// ----------------------------------------------------------------------------
void cBleController::slotServiceDiscovered(const QBluetoothUuid &newService)
{
    printNameOfDevice("cBleController::ServiceDiscovered:" + newService.toString() + ":");

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
//        m_Service = new cBleService(nullptr, m_BleController->createServiceObject(newService));

//        connect(m_Service, &cBleService::signalLog,
//                this, &cBleController::signalLog);
//        connect(m_Service, &cBleService::signalReceiveData,
//                this, &cBleController::signalReceiveData);
    }

    m_Service = new cBleService(this, m_BleController->createServiceObject(newService));

    connect(m_Service, &cBleService::signalLog,
            this, &cBleController::signalLog);
    connect(m_Service, &cBleService::signalReceiveData,
            this, &cBleController::signalReceiveData);

}
// ----------------------------------------------------------------------------
void cBleController::TransmitBleData(const QByteArray &data)
{
    m_Service->TransmitBleData(data);
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
    emit signalLog("cBleController::slotServiceDiscoveryFinished");
}
// ----------------------------------------------------------------------------
// TODO тестовая функция
void cBleController::printNameOfDevice(const QString &text)
{
    emit signalLog(text);
}
// ----------------------------------------------------------------------------
//void cBleController::slotReceiveData(const QByteArray &data)
//{
//    emit signalReceiveData(data);
//}
// ----------------------------------------------------------------------------
