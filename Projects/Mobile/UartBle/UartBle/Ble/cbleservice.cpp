#include "cbleservice.h"

// ----------------------------------------------------------------------------
cBleService::cBleService(QObject *parent, QLowEnergyService *service) :
    QObject(parent),
    m_service(service)
{
    connect(m_service, &QLowEnergyService::stateChanged, this, &cBleService::slotStateChanged);
    connect(m_service, &QLowEnergyService::characteristicWritten, this, &cBleService::slotCharacteristicWritten);
    connect(m_service, &QLowEnergyService::descriptorRead, this, &cBleService::slotDescriptorRead);
    connect(m_service, &QLowEnergyService::characteristicChanged, this, &cBleService::slotCharacteristicChanged);

    m_service->discoverDetails();
}
cBleService::~cBleService()
{
    if(m_service)
    {
        delete m_service;
    }
}
// ----------------------------------------------------------------------------
void cBleService::slotStateChanged(QLowEnergyService::ServiceState newState)
{
    QList<QLowEnergyCharacteristic> hrChar;
    QString tmp_string = "";

    tmp_string.append("StateChanged(),"); // PrintLog("StateChanged()");
    switch (newState)
    {
        case QLowEnergyService::InvalidService:
            tmp_string.append("InvalidService"); // PrintLog("InvalidService");
            break;
        case QLowEnergyService::DiscoveryRequired:  // we know start/end handle but nothing more
            tmp_string.append("DiscoveryRequired"); // PrintLog("DiscoveryRequired");
            break;
        //TODO Rename DiscoveringServices -> DiscoveringDetails or DiscoveringService
        case QLowEnergyService::DiscoveringServices:// discoverDetails() called and running
            tmp_string.append("DiscoveringServices"); // PrintLog("DiscoveringServices");
            break;
        case QLowEnergyService::ServiceDiscovered:  // all details have been synchronized
            hrChar = m_service->characteristics();
            tmp_string.append("ServiceDiscovered: characteristic cnt: " + QString::number(hrChar.count()) + ","); // PrintLog("ServiceDiscovered: characteristic cnt: " + QString::number(hrChar.count()));

            for(auto it = hrChar.begin(); it != hrChar.end(); ++it)
            {
    //            if(it->isValid())
                QLowEnergyDescriptor m_NotificationDesc = it->descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

                tmp_string.append("\nService name: " + it->name() + ", uuid: " + it->uuid().toString() + ":"); // PrintLog(" Service name: " + it->name() + "; uuid: " + it->uuid().toString());

                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Unknown)
                    tmp_string.append("Property Unknown,"); // PrintLog("Property Unknown");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Broadcasting)
                    tmp_string.append("Property Broadcasting,"); // PrintLog("Property Broadcasting");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::WriteNoResponse)
                    tmp_string.append("Property WriteNoResponse,"); // PrintLog("Property WriteNoResponse");

                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Indicate)
                    tmp_string.append("Property Indicate,"); // PrintLog("Property Indicate");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::WriteSigned)
                    tmp_string.append("Property WriteSigned,"); // PrintLog("Property WriteSigned");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::ExtendedProperty)
                    tmp_string.append("Property ExtendedProperty,"); // PrintLog("Property ExtendedProperty");

                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Read)
                {
                    m_ReadCharacteristic = *it;
                    tmp_string.append("Property Read,");
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
    //                    tmp_string = "";
                    tmp_string.append("Property Write,");

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

    //                    tmp_string = "";
                    tmp_string.append("Property Notify,");
                }
    //                m_service->writeDescriptor()
            }

            break;
        case QLowEnergyService::LocalService:
            tmp_string.append("LocalService"); // PrintLog("LocalService");
            break;
        default:
            tmp_string.append("unknown state"); // PrintLog("unknown state.");
            break;
    }
    PrintLog(tmp_string + ".");
}
// ----------------------------------------------------------------------------
// индикация отправки данных
void cBleService::slotCharacteristicWritten(const QLowEnergyCharacteristic &info,
                                            const QByteArray &value)
{
    QString s = "";

    s.append(info.uuid().toString());
    s.append("; TX data -> ");
    s.append(QString(value));

    PrintLog(s);
}
// ----------------------------------------------------------------------------
void cBleService::slotDescriptorRead(const QLowEnergyDescriptor &info,
                                     const QByteArray &value)
{
    QString s = "slotDescriptorRead:";

    s.append(info.uuid().toString());
    s.append(QString(value));

    PrintLog(s);
}
// ----------------------------------------------------------------------------
// приём данных
void cBleService::slotCharacteristicChanged(const QLowEnergyCharacteristic &info,
                                            const QByteArray &value)
{
    QString s = "";

    s.append(info.name());
    s.append(info.uuid().toString());
    s.append("; RX data <- ");
    s.append(QString(value));

    emit signalReceiveData(info.uuid().toString(), value);
//    PrintLog(s);
}
// ----------------------------------------------------------------------------
void cBleService::SendMessage(const QString &text)
{
    QByteArray array = text.toLocal8Bit();

//    m_service->writeCharacteristic(m_WriteCharacteristic, array, QLowEnergyService::WriteWithoutResponse);
    m_service->writeCharacteristic(m_WriteCharacteristic, array, QLowEnergyService::WriteWithResponse);
}
void cBleService::TransmitBleData(const QByteArray &data)
{
    if(m_service)
    {
        m_service->writeCharacteristic(m_WriteCharacteristic, data, QLowEnergyService::WriteWithResponse);
    }
    else
    {
        PrintLog("TransmitBleData: there are no transmit services");
    }
}
// ----------------------------------------------------------------------------
// TODO тестовый метод
void cBleService::PrintLog(const QString &text)
{
    QString s = "- cBleService::";
    s += "uuid: " + m_service->serviceUuid().toString() + ": ";
    s += text;

    emit signalLog(s);
}
// ----------------------------------------------------------------------------
