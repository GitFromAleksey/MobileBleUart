/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __REMOTE_MENU_H
#define __REMOTE_MENU_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "menu_interface.h"
#include "main_common.h"
/*Exported defines*/
// clang-format off
#define REMOTE_PAIR_STR					"PAIr "
#define REMOTE_NO_PAIR_STR				"NPAIr"

#define REMOTE_CONN_STR					"CONN "
#define REMOTE_NO_CONN_STR				"NOCON"
#define REMOTE_ERASE_BONDS_STR			"ErASE"

#define REMOTE_FOUND_BRLT_STR			"br Lt"
#define REMOTE_FOUND_BR3S_STR			"br 3S"
#define REMOTE_FOUND_BR4S_STR			"br 4S"

#define DEFAULT_REMOTE_BLINK_PATTERN 	0x3333
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
enum
{
	REMOTE_MENU_INIT_ID = 0,
	REMOTE_MENU_BLE_ID,
	REMOTE_MENU_OFF_ID,
	REMOTE_MENU_SIZE,
};

enum
{
	REMOTE_BLE_CLEAR_M_ID = 0,
	REMOTE_BLE_CONN_ATT_M_ID,
	REMOTE_BLE_CONN_FAIL_M_ID,
	REMOTE_BLE_PAIR_ATT_M_ID,
	REMOTE_BLE_PAIR_FAIL_M_ID,
	REMOTE_BLE_ERASE_BONDS_ID,
	REMOTE_BLE_FOUND_BRLT_ID,
	REMOTE_BLE_FOUND_BR3S_ID,
	REMOTE_BLE_FOUND_BR4S_ID,
	
	REMOTE_BLE_M_NUM,
};

enum
{
	REMOTE_INIT_CLEAR_M_ID = 0,
	REMOTE_INIT_REPORT_BRC_FW_ID,
	REMOTE_INIT_SCREEN_TEST_ID,

	REMOTE_INIT_M_NUM,
};

enum
{
	REMOTE_OFF_CLEAR_M_ID = 0,

	REMOTE_OFF_M_NUM,
};

//typedef uint32_t (*set_indication_t)(void * p_arg, uint32_t length);

typedef struct _tag_remote_menu_off_state_t
{
	uint32_t size;
	set_indication_t * p_menu_sub_m_list;
}remote_menu_m_list_t;


typedef struct _tag_remote_menu_state_t
{
	remote_menu_m_list_t		menu[REMOTE_MENU_SIZE];
	uint32_t 					state;
	uint32_t 					sub_state;
	brc_main_t					* p_brc_main;
	device_instance_callback_t	p_remote_callback;
}remote_menu_state_t;
/*Exported constants*/
extern const menu_m_list_t	remote_menu_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
remote_menu_state_t * remote_menu_get_mode(void);
#endif /*__REMOTE_MENU_H*/
/******************************* END_OF_FILE **********************************/