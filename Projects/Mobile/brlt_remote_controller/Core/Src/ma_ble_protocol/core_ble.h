#ifndef _CORE_BLE_H_
#define _CORE_BLE_H_

#include "ble.h"
#include "ble_gap.h"
#include <stdbool.h>
#include "nrf_log.h"

typedef void (*core_ble_event_callback_t)(ble_evt_t * p_ble_evt);
typedef void (*core_ble_data_callback_t) (uint8_t * p_data, uint16_t data_len);
typedef bool (*core_ble_adv_filter_t)
				(ble_gap_evt_adv_report_t const * p_adv_report);

uint32_t core_ble_send_data (uint8_t* data, uint32_t size);
uint16_t core_ble_get_current_data_len (void);
void core_ble_disconnect(void);
bool core_ble_scan_start(bool* is_whitelist);
void core_ble_set_adv_filter (core_ble_adv_filter_t filter);
void core_ble_init (core_ble_event_callback_t evt_callback, 
					core_ble_data_callback_t data_callback);
bool core_ble_is_connect(void);
void core_ble_erase_bond(void);
#endif /* _CORE_BLE_H_ */

