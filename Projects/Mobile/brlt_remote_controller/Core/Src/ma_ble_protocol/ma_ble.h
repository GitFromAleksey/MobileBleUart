#ifndef _MA_BLE_H_
#define _MA_BLE_H_

#include <stdint.h>
#include <stdbool.h>

#define SLAVE_UPDATE_REQ                        0x4004   // посылает ведомый при необходимости обновиться (асинхронные данные)
#define SLAVE_UPDATE_START                      0x4005   // перед началом обновления посылается ведомому, содержит заголовок файла с прошивкой и размер прошивки
#define SLAVE_UPDATE_FIRM_CHUNK                 0x4006   // кусочек прошивки (сожержит смещение, тело кусочка, и размер кусочка (косвенно))
#define SLAVE_UPDATE_UPDATE                     0x4007   // верифицирует прошивку - в случае успеха обновляется
#define SLAVE_UPDATE_ERR                        0x4008   // содержит код ошибки
#define SLAVE_UPDATE_FIRM_CHUNK_OK              0x400B   // подтверждение кусочка 
#define SLAVE_UPDATE_FIRM_START_OK              0x400C   // подтверждение готовности к приему прошивки
#define SLAVE_UPDATE_FIRM_BURN_AND_VERIFY_OK    0x400D   // ошибка целостности
#define SLAVE_UPDATE_REBOOT_FOR_UPDATE          0x400E 	 //делает метку что при перезагрузке надо ждать обновления

#define BLE_FRAME_TYPE_DEV_STATUS_REQ			0x4009
#define BLE_FRAME_TYPE_DEV_STATUS_RESP 			0x400A

#define	BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ		0x1230
#define	BLE_FRAME_TYPE_GET_MODE_RESP			0x1231
#define	BLE_FRAME_TYPE_GET_MODE_REQ				0x1232	
#define	BLE_FRAME_TYPE_SET_MODE_SAVE_REQ		0x1234

typedef enum
{
	MA_BLE_CONNECTION,
	MA_BLE_DISCONNECTION,
	MA_BLE_START_SEARCH,
	MA_BLE_FIND_DEVICE
}ma_ble_event_t;

enum ma_ble_device
{
	IRM,
	BRLT
};

typedef struct ma_ble_frame_t_
{
	uint16_t type;
	uint32_t request_id; 
	const void *data;
	uint16_t data_size;
}ma_ble_frame_t;

typedef void (*ma_ble_event_handler_t)(ma_ble_event_t evt);
typedef void (*ma_ble_frame_callback_t) (ma_ble_frame_t frame);


void ma_ble_start_search(void);
void ma_ble_disconnect(void);
uint32_t ma_ble_init (uint8_t device_type, bool erase_paired, 
					  ma_ble_event_handler_t evt_h, 
					  ma_ble_frame_callback_t callback);
int32_t ma_ble_put_frame_in_queue(ma_ble_frame_t frame);
void ma_ble_frames_flush(void);
bool ma_ble_is_connect(void);

#endif /* _MA_BLE_H_*/

