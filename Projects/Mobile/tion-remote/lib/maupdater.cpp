#include "maupdater.h"
#include "crc_calculate.h"

MABaseProtocol::RawDataList MAUpdater::doUpdate(bool start)
{
    if (start)
    {
        percentage_of_firm = 0;
        set_state(MA_UPD_PREPARE);
    }

    return dataForSend;
}

int MAUpdater::progress()
{
    return percentage_of_firm;
}

void MAUpdater::setFirm(Firmware *fw)
{
    firmware = fw;
}

void MAUpdater::frameRecieveCallback(ble_frame_t frame)
{

    switch(frame.type)
    {
        case SLAVE_UPDATE_REQ:
            set_state(MA_UPD_START);
            break;
        case SLAVE_UPDATE_FIRM_START_OK:
            set_state(MA_UPD_CHUNK);
            break;
        case SLAVE_UPDATE_FIRM_CHUNK_OK:
            percentage_of_firm = (1000 * set_state(MA_UPD_CHUNK) /
                firmware->size())/10;

            percentage_of_firm %= 100;

            break;
        case SLAVE_UPDATE_FIRM_BURN_AND_VERIFY_OK:
            percentage_of_firm = UPDATE_DONE;
        break;
        default:
            break;
    }

}

int MAUpdater::set_state(
        MAUpdater::ma_updater_state_t state)
{
    if(firmware->p() == nullptr)
        return 0;

    resetDataForSend();

    static uint8_t reboot_data[7] = {0};
    static uint8_t start_data[132] = {0};
    static uint8_t chunck_data[CHUNK_SIZE + 4] = {0};
    static ma_updater_state_t prev_st = MA_UPD_IDLE;
    static uint32_t offset = 0;

    ble_frame_t frame;
    memset(&frame, 0 , sizeof(frame));
    uint32_t request_id = 0x01;
    uint16_t file_crc = 0;

    *(uint32_t*)start_data = static_cast<uint32_t>(firmware->size() + 128 + BLE_CRC_SIZE);//this is file len
    memset(start_data + 4, 0xDE, sizeof start_data - 4);//file may consist app + crc + softdev

    if(state != prev_st + 1 && state != MA_UPD_CHUNK)
        frame.type = 0;

    prev_st = state;
    frame.request_id = request_id;
    switch(state)
    {
    case MA_UPD_IDLE:
        offset = 0;
        break;

    case MA_UPD_PREPARE:
        offset = 0;
        frame.type = SLAVE_UPDATE_REBOOT_FOR_UPDATE;
        frame.data = reboot_data;
        frame.data_size = sizeof(reboot_data);
        sendFrame(frame);
        break;

    case MA_UPD_START:
        frame.type = SLAVE_UPDATE_START;
        frame.data = start_data;
        frame.data_size = sizeof(start_data);
        sendFrame(frame);
        break;

    case MA_UPD_CHUNK:

        if(offset < firmware->size())
        {
            *(uint32_t*)chunck_data = offset;
            memcpy(chunck_data + 4, firmware->p() + offset, CHUNK_SIZE);

            frame.type = SLAVE_UPDATE_FIRM_CHUNK;
            frame.data = chunck_data;

            if((offset + CHUNK_SIZE) > firmware->size())
            {
                frame.data_size = firmware->size() - offset + 4;
                offset = firmware->size();
            }
            else
            {
                frame.data_size = CHUNK_SIZE + 4;
                offset += CHUNK_SIZE;
            }
        }
        else if(offset != MA_UDP_END_OF_FIRMWARE)
        {
            *(uint32_t*)chunck_data = offset;
            file_crc = CRC_CCITT(0xFFFF,firmware->p(),firmware->size(),0);
            *(chunck_data + 4) = ((uint8_t*)&file_crc)[1];
            *(chunck_data + 5) = ((uint8_t*)&file_crc)[0];
            frame.type = SLAVE_UPDATE_FIRM_CHUNK;
            frame.data = chunck_data;
            frame.data_size = BLE_CRC_SIZE + 4;
            offset = MA_UDP_END_OF_FIRMWARE;
        }
        else
        {
            frame.type = SLAVE_UPDATE_UPDATE;
            frame.data = start_data;
            frame.data_size = sizeof(start_data);
            prev_st = MA_UPD_IDLE;
            if (!timeout_timer->isActive())
                timeout_timer->start(800);
        }
        sendFrame(frame);
        break;

    default:
        break;
    }

    return offset;
}

void MAUpdater::timeout_handler(void)
{
    percentage_of_firm = UPDATE_DONE_TIMEOUT;
    timeout_timer->stop();
}

void MAUpdater::setTimer(QTimer *timer)
{
    if (timer)
        timeout_timer = timer;
}
