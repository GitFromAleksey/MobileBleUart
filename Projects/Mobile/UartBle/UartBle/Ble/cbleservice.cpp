#include "cbleservice.h"

cBleService::cBleService(QObject *parent, QLowEnergyService *service) :
    QObject(parent),
    m_service(service)
{
    connect(m_service, &QLowEnergyService::stateChanged, this, &cBleService::StateChanged);
    connect(m_service, &QLowEnergyService::characteristicWritten, this, &cBleService::CharacteristicWritten);
    connect(m_service, &QLowEnergyService::descriptorRead, this, &cBleService::DescriptorRead);
    connect(m_service, &QLowEnergyService::characteristicChanged, this, &cBleService::CharacteristicChanged);

    m_service->discoverDetails();
}

void cBleService::StateChanged(QLowEnergyService::ServiceState newState)
{
    QList<QLowEnergyCharacteristic> hrChar;
    QString tmp_string = "";

    printNameOfDevice("cBleService::StateChanged:");
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
            QLowEnergyDescriptor m_NotificationDesc = it->descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

            printNameOfDevice(" Service name: " + it->name() + "; uuid: " + it->uuid().toString());

                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Unknown)
                    printNameOfDevice("  Property Unknown");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Broadcasting)
                    printNameOfDevice("  Property Broadcasting");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::WriteNoResponse)
                    printNameOfDevice("  Property WriteNoResponse");


                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Indicate)
                    printNameOfDevice("  Property Indicate");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::WriteSigned)
                    printNameOfDevice("  Property WriteSigned");
                if(it->properties() & QLowEnergyCharacteristic::PropertyType::ExtendedProperty)
                    printNameOfDevice("  Property ExtendedProperty");

                if(it->properties() & QLowEnergyCharacteristic::PropertyType::Read)
                {
                    m_ReadCharacteristic = *it;
                    tmp_string = "";
                    tmp_string.append("  Property Read uuid: ");
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
                    tmp_string = "";
                    tmp_string.append("  Property Write uuid: ");
                    tmp_string.append(m_WriteCharacteristic.uuid().toString());
                    printNameOfDevice(tmp_string);
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

                    tmp_string = "";
                    tmp_string.append("  Property Notify uuid: ");
                    tmp_string.append(m_ReadCharacteristic.uuid().toString());
                    printNameOfDevice(tmp_string);
                }
//                m_service->writeDescriptor()
        }

        break;
    case QLowEnergyService::LocalService:
        printNameOfDevice(" LocalService");
        break;
    default:
//        printNameOfDevice(";");
        break;
    }
}
void cBleService::CharacteristicWritten(const QLowEnergyCharacteristic &info,
                           const QByteArray &value)
{

}
void cBleService::DescriptorRead(const QLowEnergyDescriptor &info,
                    const QByteArray &value)
{

}
void cBleService::CharacteristicChanged(const QLowEnergyCharacteristic &info,
                           const QByteArray &value)
{

}
// TODO тестовый метод
void cBleService::printNameOfDevice(const QString &text)
{
    emit signalLog(text);
}

