/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BR3S_MENU_H
#define __BR3S_MENU_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "menu_interface.h"
#include "device_interface.h"
#include "main_common.h"
/*Exported defines*/
// clang-format off
#define BR3S_STBY_STR					"-- --"
#define BR3S_HEATER_OFF_STR				"H-OFF"
#define BR3S_DEV_REP_STR				"br 3S"
#define BR3S_ON_STR						" On"
#define BR3S_OFF_STR					"OFF"
#define BR3S_ERASE_BONDS_STR			"ErASE"
#define BR3S_DEFAULT_MODE_ID			0
#define BR3S_ON_SET_SINGLE_CONT_OFFSET		(uint32_t)(BR3S_ON_SET_CONT_TIME_HOUR_M_ID - BR3S_ON_SET_TIME_HOUR_M_ID)
#define BR3S_ON_DEFAULT_SINGLE_CONT_OFFSET	(uint32_t)(BR3S_ON_DEFAULT_CONT_TSET_M_ID - BR3S_ON_DEFAULT_TSET_M_ID)
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
enum
{
	BR3S_MENU_INIT_ID = 0,
	BR3S_MENU_ON_ID,
	BR3S_MENU_OFF_ID,
	BR3S_MENU_SIZE,
};

enum
{
	BR3S_ON_CLEAR_M_ID = 0,
	BR3S_ON_DEFAULT_M_ID,
	BR3S_ON_SET_M_ID,
	BR3S_ON_ERROR_M_ID,
	BR3S_ON_M_NUM,
};

enum
{
	BR3S_ON_DEFAULT_TOUT_M_ID = 0,
	BR3S_ON_DEFAULT_TIN_M_ID,
	BR3S_ON_DEFAULT_TIME_M_ID,
	BR3S_ON_DEFAULT_DEV_REPORT_M_ID,
	BR3S_ON_DEFAULT_DEV_FW_REPORT_M_ID,
	BR3S_ON_DEFAULT_M_NUM,
	BR3S_ON_DEFAULT_TSET_M_ID,
	BR3S_ON_DEFAULT_HOFF_M_ID,
	BR3S_ON_DEFAULT_INVALID_SPEED_M_ID,

	BR3S_ON_DEFAULT_CONT_TSET_M_ID,
};

enum
{
	BR3S_ON_SET_HEATER_HOFF_M_ID = 0,
	BR3S_ON_SET_HEATER_TSET_M_ID,
	BR3S_ON_SET_TIME_HOUR_M_ID,
	BR3S_ON_SET_TIME_MIN_M_ID,
	BR3S_ON_SET_MA_TIME_M_ID,
	BR3S_ON_SET_ALARM_ON_M_ID,
	BR3S_ON_SET_ALARM_ON_HOUR_M_ID,
	BR3S_ON_SET_ALARM_ON_MIN_M_ID,
	BR3S_ON_SET_ALARM_OFF_HOUR_M_ID,
	BR3S_ON_SET_ALARM_OFF_MIN_M_ID,
	BR3S_ON_SET_ALARM_OFF_M_ID,
	BR3S_ON_SET_FILTER_TIME_M_ID,
	BR3S_ON_SET_M_NUM,

	BR3S_ON_SET_CONT_TIME_HOUR_M_ID,
	BR3S_ON_SET_CONT_TIME_MIN_M_ID,
	BR3S_ON_SET_DUMMY_M_ID,
	BR3S_ON_SET_DUMMY_MA_M_ID,
	BR3S_ON_SET_CONT_ALARM_ON_HOUR_M_ID,
	BR3S_ON_SET_CONT_ALARM_ON_MIN_M_ID,
	BR3S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID,
	BR3S_ON_SET_CONT_ALARM_OFF_MIN_M_ID,
};

enum
{
	BR3S_INIT_CLEAR_M_ID = 0,
	BR3S_INIT_ERASE_BONDS_M_ID,
	BR3S_INIT_REPORT_DEV_M_ID,
	BR3S_INIT_DUMMY_SCREEN_M_ID,

	BR3S_INIT_M_NUM,
};

enum
{
	BR3S_OFF_CLEAR_M_ID = 0,
	BR3S_OFF_DEFAULT_M_ID,
	BR3S_OFF_ERROR_M_ID,
	BR3S_OFF_M_NUM,
};



typedef struct _tag_br3s_menu_off_state_t
{
	uint32_t size;
	set_indication_t * p_menu_sub_m_list;
}br3s_menu_m_list_t;

typedef struct _tag_br3s_menu_state_t
{
	br3s_menu_m_list_t			menu[BR3S_MENU_SIZE];
	uint32_t 					state;
	uint32_t 					sub_state;
	uint32_t					sub_state_mode;
	brc_main_t					* p_brc_main;
	br3s_state_t				* p_br3s_state;
	device_instance_callback_t	p_br3s_callback;
	uint8_t 					* p_prev_menu_sub_state_mode;
}	br3s_menu_state_t;
/*Exported constants*/
extern const menu_m_list_t	br3s_menu_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
br3s_menu_state_t * br3s_menu_get_mode(void);
#endif /*__br3s_MENU_H*/
/******************************* END_OF_FILE **********************************/