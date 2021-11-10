/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __MAIN_COMMON_H
#define __MAIN_COMMON_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "brc_buttons.h"
#include "core_ble.h"
#include "brc_lcd_indication.h"
#include "device_interface.h"
#include "user_data.h" 
#ifndef DEBUG_BUILD
#include "version.h"
#else
#include "debug_version.h"
#endif
#ifndef SW_VERSION
	#define SW_VERSION 1 
#endif



/*Exported defines*/
// clang-format off
enum
{
	DEFAULT_DEV = 0, 
	BRLT_DEV,
	BR3S_DEV,
	BR4S_DEV,

	SUP_DEV_COUNT,
	NO_VALUE,
};

enum
{
	DEFAULT_EVT = 0,
	BLE_EVT,
	USER_TIMEOUT_OVF_EVT,
	USER_TIMEOUT_RELOAD_EVT,
	USER_MENU_TIMEOUT_OVF_EVT,
	USER_MENU_TIMEOUT_RELOAD_EVT,
	BUTTON_EVT,
	SLEEP_EVT,
	FOUND_BRLT,
	FOUND_BR3S,
	DEV_SWITCH,
	INVALID_SPEED,
	BLE_DISCONNECT,
	BLE_ERASE_BONDS,
	BLE_START_PAIRING,
	BLE_BONDS_ERASED,
	BLE_SEND_DATA,
	BLE_FORCE_SEND_DATA,
	START_DEV_POLL,
	STOP_DEV_POLL,
	START_CONT_PARAM_CHANGE_TIMER,
	STOP_CONT_PARAM_CHANGE_TIMER,
	DEV_POLL_TIMER_RELOAD,
	INVALID_SPEED_TIMEOUT_OVF_EVT,
	START_SCREEN_TEST,
	REPORT_BRC_FW_TIMEOUT_RELOAD,
	REPORT_BRC_FW_TIMEOUT_OVF,
	SCREEN_TEST_TIMEOUT_RELOAD,
	SCREEN_TEST_TIMEOUT_OVF,
	BUTTON_EVT_BLOCK,
	BUTTON_EVT_BLOCK_OVF,
	FOUND_BR4S,
};
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_brc_dev_t
{
	device_t				* interface;
	ble_adv_data_parser_t   p_adv_data_parser;
}brc_dev_t;

typedef struct _tag_brc_main_t
{
	const ble_m_list_t			* p_core_ble;
	const indication_m_list_t	* p_lcd;
	brc_lcd_g_ram_t				* p_lcd_g_ram;
	brc_dev_t					dev_list[SUP_DEV_COUNT];
	brc_dev_t					* p_current_dev;
	uint32_t					expected_dev_type;
	uint32_t 					test_mode;
}brc_main_t;
/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/

#endif /*__MAIN_COMMON_H*/
/******************************* END_OF_FILE **********************************/