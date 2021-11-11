#ifndef MAUPDATER_H
#define MAUPDATER_H

#include "mabaseprotocol.h"
#include "firmware.h"

class MAUpdater : public MABaseProtocol
{
public:
    MAUpdater() : percentage_of_firm(0), firmware(nullptr)
    {}

    enum {UPDATE_DONE = 0xFF,
          UPDATE_DONE_TIMEOUT = 0xFE,};

    RawDataList doUpdate(bool start = false);
    int progress();
    void setFirm(Firmware* fw);
    void timeout_handler(void);
    void setTimer(QTimer *timeout_timer);
private:

    enum
    {
        SLAVE_UPDATE_REQ  =                      0x4004,   // посылает ведомый при необходимости обновиться (асинхронные данные)
        SLAVE_UPDATE_START =                     0x4005,   // перед началом обновления посылается ведомому, содержит заголовок файла с прошивкой и размер прошивки
        SLAVE_UPDATE_FIRM_CHUNK =                 0x4006,   // кусочек прошивки (сожержит смещение, тело кусочка, и размер кусочка (косвенно))
        SLAVE_UPDATE_UPDATE =                    0x4007,   // верифицирует прошивку - в случае успеха обновляется
        SLAVE_UPDATE_ERR =                       0x4008,   // содержит код ошибки
        SLAVE_UPDATE_FIRM_CHUNK_OK =              0x400B,   // подтверждение кусочка
        SLAVE_UPDATE_FIRM_START_OK =              0x400C,   // подтверждение готовности к приему прошивки
        SLAVE_UPDATE_FIRM_BURN_AND_VERIFY_OK =   0x400D,   // ошибка целостности
        SLAVE_UPDATE_REBOOT_FOR_UPDATE =         0x400E 	 //делает метку что при перезагрузке надо ждать обновления
    };

    void frameRecieveCallback(ble_frame_t frame);

    enum { MA_UDP_END_OF_FIRMWARE = 0xFFFFFFFF,
           CHUNK_SIZE = 768,  //if 1024 may be falls
         };

    typedef enum ma_boot_state_t_
    {
        MA_UPD_IDLE,
        MA_UPD_PREPARE,
        MA_UPD_START,
        MA_UPD_CHUNK,
    }
    ma_updater_state_t;

    int set_state(ma_updater_state_t state);

    uint8_t percentage_of_firm;
    Firmware* firmware;
    QTimer *timeout_timer;
};

#endif // MAUPDATER_H
