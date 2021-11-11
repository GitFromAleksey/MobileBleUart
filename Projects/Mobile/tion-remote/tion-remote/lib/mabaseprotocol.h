#ifndef MABASEPROTOCOL_H
#define MABASEPROTOCOL_H

#include <QByteArray>
#include <QList>
#include <QTimer>
#include <lib/firmware.h>
#include <QDateTime>

class MABaseProtocol
{
public:
    MABaseProtocol() : pRxBuffer(rxBuffer) {}

    struct RawData
    {
        RawData() {}
        uint8_t* p;
        uint16_t ln;
    };

    typedef QList<RawData> RawDataList;

    void mainDataCallback(RawData data);

protected:

    //***************************************************
    // ma ble level
    //***************************************************
    enum
    {
        BLE_RX_BUFFER_SIZE = 1024 + 64,
        BLE_MAGIC_NUMBER                    =   0x3A,
        BLE_CRC_SIZE = 2,
        NRF_BLE_GATT_MAX_MTU_SIZE = 254,
        CURRENT_MTU_SIZE = 20
    };

    typedef enum{
        BLE_PACKET_TYPE_FIRST   = 0,
        BLE_PACKET_TYPE_CURRENT = 1,
        BLE_PACKET_TYPE_FEND    = 2,
        BLE_PACKET_TYPE_END     = 3, //Bytes of MA packets
    }ble_packet_type_t;

    typedef struct ma_ble_frame_t_
    {
        uint16_t type;
        uint32_t request_id;
        const void *data;
        uint16_t data_size;
    }ble_frame_t;

#pragma pack(push, 1)
    typedef struct ble_frame_type_t_
    {
        uint16_t size;
        uint8_t  magic_number;
        uint8_t  random_number;
        uint16_t type;
        uint32_t request_id;
        uint8_t  data[BLE_CRC_SIZE];
    }not_wrapped_ma_ble_frame_t;
#pragma pack(pop)

    void    dataRecieveCallback(void *data, uint32_t size);
    int32_t sendFrame(ble_frame_t frame);
    int32_t sendData(const void *data, uint32_t size);
    void    readData(uint8_t * p_data, uint16_t length);

    int firmVersion = 0;

    uint32_t BLE_FRAME_FULL_SIZE(uint32_t data_size)
    {
       return (sizeof(not_wrapped_ma_ble_frame_t) + data_size);
    }
    typedef uint16_t ble_crc_t;

    RawDataList dataForSend;
    void resetDataForSend();

    virtual void frameRecieveCallback(ble_frame_t frame) = 0;

private:
    uint8_t rxBuffer[BLE_RX_BUFFER_SIZE]; //big MA packet - 1024 bytes
    uint8_t *pRxBuffer; //pointer in big MA packet

    uint8_t txBuffer[NRF_BLE_GATT_MAX_MTU_SIZE - 3];
};

#endif // MABASEPROTOCOL_H
