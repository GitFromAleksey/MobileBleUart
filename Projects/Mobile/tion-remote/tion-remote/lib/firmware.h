#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <QByteArray>
#include <QFile>

typedef enum
{
    FIRM_BrLt = 0,
    FIRM_Rb4S = 1,
    FIRM_IQ   = 2,

    NUM_OF_FIRMWARES,
}firm_id_t;

static const uint16_t firm_id[NUM_OF_FIRMWARES] = {0x8002, 0x8003, 0x8004};

class Firmware
{
public:
    Firmware();
    virtual ~Firmware(){}
    virtual uint32_t size() const = 0;
    virtual const char* p () const = 0;
    virtual uint16_t version(void) const = 0;
    virtual bool isValid(firm_id_t b_firm_id) const = 0;

protected:
    const char* data;
};

class ArrayFirmware : public Firmware
{
public:
    ArrayFirmware();
    virtual ~ArrayFirmware(){}

    uint32_t size() const;
    const char* p () const;
    uint16_t version(void) const;
    bool isValid(firm_id_t b_firm_id) const;
};

class FileFirmware : public Firmware
{
public:
    FileFirmware(QString name);
    virtual ~FileFirmware(){}

    uint32_t size() const;
    const char* p () const;
    uint16_t version(void) const;
    bool isValid(firm_id_t b_firm_id) const;
private:
    bool isOpen;
    QByteArray array;
};
#endif // FIRMWARE_H
