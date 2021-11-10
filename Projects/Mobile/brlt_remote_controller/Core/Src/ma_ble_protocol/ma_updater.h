#ifndef _MA_UPDATER_H_
#define _MA_UPDATER_H_

#include "ma_ble.h"

#define MA_UDP_END_OF_FIRMWARE 0xFFFFFFFF

typedef enum ma_boot_state_t_
{
	MA_UPD_IDLE,
	MA_UPD_PREPARE,
	MA_UPD_START,
	MA_UPD_CHUNK,
}
ma_updater_state_t;

uint32_t ma_updater_set_state(ma_updater_state_t state);
uint32_t ma_updater_firmw_size(void);
uint16_t ma_updater_firmw_version(void);

#endif /*_MA_UPDATER_H_*/