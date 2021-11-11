#include "mabaseprotocol.h"
#include "crc_calculate.h"
#include <QDebug>

void MABaseProtocol::readData(uint8_t * p_data, uint16_t length)
{
    if(p_data && length) //if data is available
    {
        length--; //Subtract the length of the first byte. Only the TYPE is read from it.
        switch(*p_data++ >> 6) // 1.Dereferencing of a pointer 2.Bit shift 3. Address increment
        {
        case BLE_PACKET_TYPE_FIRST:
            if(length <= BLE_RX_BUFFER_SIZE)
            {
                memcpy(rxBuffer, p_data, length);
                pRxBuffer = rxBuffer + length;
            }

            break;

        case BLE_PACKET_TYPE_CURRENT:
            if(length <= BLE_RX_BUFFER_SIZE - (pRxBuffer - rxBuffer)) //If there is place left after the first TYPE packets
            {
                memcpy(pRxBuffer, p_data, length);
                pRxBuffer += length;
            }

            break;

        case BLE_PACKET_TYPE_FEND:
            dataRecieveCallback(p_data, length); //?
            break;

        case BLE_PACKET_TYPE_END:
            if(length <= BLE_RX_BUFFER_SIZE - (pRxBuffer - rxBuffer) && pRxBuffer != rxBuffer) //If there is place left after the first TYPE packets and current TY packets
            {
                memcpy(pRxBuffer, p_data, length);
                pRxBuffer += length;
                dataRecieveCallback(rxBuffer, pRxBuffer - rxBuffer);
            }
            pRxBuffer = rxBuffer;
            break;
        }
    }
}


void MABaseProtocol::dataRecieveCallback(void *data, uint32_t size)
{
    not_wrapped_ma_ble_frame_t* rx_frame = (not_wrapped_ma_ble_frame_t*)data;
    ble_frame_t wrapped_frame; // = {0};
    memset(&wrapped_frame, 0 , sizeof wrapped_frame);

    if(rx_frame->magic_number == BLE_MAGIC_NUMBER &&
            rx_frame->size   == size &&
            CRC_CCITT(0xFFFF, rx_frame, size, 1) == 0)
    {
        wrapped_frame.data = rx_frame->data;
        wrapped_frame.data_size = rx_frame->size;
        wrapped_frame.request_id = rx_frame->request_id;
        wrapped_frame.type = rx_frame->type;

        frameRecieveCallback(wrapped_frame);
    }

}

int32_t MABaseProtocol::sendFrame (ble_frame_t frame)
{
    not_wrapped_ma_ble_frame_t* tx_frame;
    uint32_t frame_size;
    int32_t ret_val = 0;

    frame_size = BLE_FRAME_FULL_SIZE(frame.data_size);

    if((tx_frame = (not_wrapped_ma_ble_frame_t*)malloc(frame_size)) != NULL)
    {
        tx_frame->size			= frame_size;
        tx_frame->type			= frame.type;
        tx_frame->request_id	= frame.request_id;
        tx_frame->magic_number	= BLE_MAGIC_NUMBER;
        tx_frame->random_number	= 0xAD;
        memcpy(tx_frame->data, frame.data, frame.data_size);

        *(ble_crc_t*)&tx_frame->data[frame.data_size] =
            CRC_CCITT(0xFFFF, tx_frame, frame_size - 2, 1);

        sendData(tx_frame, frame_size);
        free(tx_frame);

    }
    else
        ret_val = 0;

    return ret_val;
}

int32_t MABaseProtocol::sendData(const void *data, uint32_t size)
{
    uint32_t data_packet_size = CURRENT_MTU_SIZE - 1;
    const uint8_t *data8 = (uint8_t*)data;

    data_packet_size = (size > data_packet_size)?(data_packet_size):(size);
    size -= data_packet_size;
    txBuffer[0] = ((size)?(BLE_PACKET_TYPE_FIRST):(BLE_PACKET_TYPE_FEND)) << 6;
    memcpy(&txBuffer[1], data8, data_packet_size);
    data8 += data_packet_size;

    RawData rw;
    rw.p =  (uint8_t*)malloc(data_packet_size + 1);
    memcpy(rw.p, txBuffer, data_packet_size + 1);
    rw.ln = data_packet_size + 1;
    dataForSend.push_back(rw);
    //core_ble_send_data(buffer, data_packet_size + 1);

    while(size)
    {
        data_packet_size = (size > data_packet_size)?(data_packet_size):(size);
        size -= data_packet_size;
        txBuffer[0] = ((size)?(BLE_PACKET_TYPE_CURRENT):(BLE_PACKET_TYPE_END)) << 6;
        memcpy(&txBuffer[1], data8, data_packet_size);
        data8 += data_packet_size;

        rw.p =  (uint8_t*)malloc(data_packet_size + 1);
        memcpy(rw.p, txBuffer, data_packet_size + 1);
        rw.ln = data_packet_size + 1;
        dataForSend.push_back(rw);
        //core_ble_send_data(buffer, data_packet_size + 1);
    }

    return 0;
}

void MABaseProtocol::resetDataForSend()
{
    foreach (RawData rw, dataForSend)
    {
        if(rw.p != 0)
        {
            free(rw.p);
            rw.p = 0;
        }
        rw.ln = 0;
    }
    dataForSend.clear();
}

void MABaseProtocol::mainDataCallback(MABaseProtocol::RawData data)
{
    resetDataForSend(); //workaround for MaUpdater
    readData(data.p, data.ln);
}
