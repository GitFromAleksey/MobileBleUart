#include "firmware.h"
#include "test_firm_array.h"

#include <QDebug>

Firmware::Firmware()
{
    data = nullptr;
}

ArrayFirmware::ArrayFirmware()
{
    data = (char*)BrLt_bin;
}

uint32_t ArrayFirmware::size() const
{
    return sizeof(BrLt_bin);
}

const char *ArrayFirmware::p() const
{
    return data;
}

uint16_t ArrayFirmware::version() const
{
    return *((uint16_t*)(data + 0x208));
}

bool ArrayFirmware::isValid(firm_id_t b_firm_id = NUM_OF_FIRMWARES) const
{
    return true;
}



FileFirmware::FileFirmware(QString name)
{
    QFile file(name);

    if (!file.open(QIODevice::ReadOnly))
    {
        isOpen = false;
        qDebug() << name << "firm open failed";
        return;
    }

    qDebug() << name << "firm is open" << "size " << file.size();

    isOpen = true;
    array = file.readAll();
}

uint32_t FileFirmware::size() const
{
    return array.size();
}

const char *FileFirmware::p() const
{
    return array.constData();
}

uint16_t FileFirmware::version() const
{
    return *((uint16_t*)(array.constData() + 0x208));
}

bool FileFirmware::isValid(firm_id_t b_firm_id) const
{
    if (b_firm_id >= NUM_OF_FIRMWARES)
        return false;

    return isOpen &&
            *((uint16_t*)(array.constData() + 0x204)) == firm_id[b_firm_id];
}
