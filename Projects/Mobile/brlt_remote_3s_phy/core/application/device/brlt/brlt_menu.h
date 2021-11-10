/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRLT_MENU_H
#define __BRLT_MENU_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "menu_interface.h"
#include "device_interface.h"
#include "main_common.h"
/*Exported defines*/
// clang-format off
#define BRLT_STBY_STR					"-- --"
#define BRLT_HEATER_OFF_STR				"H-OFF"
#define BRLT_DEV_REP_STR				"br Lt"
#define BRLT_ON_STR						" On"
#define BRLT_OFF_STR					"OFF"
#define BRLT_ERASE_BONDS_STR			"ErASE"
#define BRLT_DEFAULT_MODE_ID			0

#define BRLT_ON_DEFAULT_SINGLE_CONT_OFFSET	(uint32_t)(BRLT_ON_DEFAULT_TSET_M_ID - BRLT_ON_DEFAULT_CONT_TSET_M_ID)

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
enum
{
	BRLT_MENU_INIT_ID = 0,
	BRLT_MENU_ON_ID,
	BRLT_MENU_OFF_ID,

	BRLT_MENU_SIZE,
}; /* menu states */

enum
{
	BRLT_ON_CLEAR_M_ID = 0,
	BRLT_ON_DEFAULT_M_ID,
	BRLT_ON_SET_M_ID,
	BRLT_ON_ERROR_M_ID,
	BRLT_ON_KIV_M_ID,
	BRLT_ON_KIV_SET_M_ID,

	BRLT_ON_M_NUM,
}; /* menu on substates */

enum
{
	BRLT_ON_DEFAULT_HOFF_M_ID = 0,
	BRLT_ON_DEFAULT_TSET_M_ID,
	BRLT_ON_DEFAULT_H_INACTIVE_M_ID,
	BRLT_ON_DEFAULT_TOUT_M_ID,
	BRLT_ON_DEFAULT_DEV_REPORT_M_ID,
	BRLT_ON_DEFAULT_DEV_FW_REPORT_M_ID,
	BRLT_ON_DEFAULT_M_NUM,
	BRLT_ON_DEFAULT_INVALID_SPEED_M_ID,

	BRLT_ON_DEFAULT_CONT_TSET_M_ID,
}; /* menu on default substate modes */

enum
{
	BRLT_ON_SET_HEATER_HOFF_M_ID = 0,
	BRLT_ON_SET_HEATER_TSET_M_ID,
	BRLT_ON_SET_HEATER_H_INACTIVE_M_ID,
	BRLT_ON_SET_FILTER_TIME_M_ID,
	
	BRLT_ON_SET_M_NUM,
}; /* menu on set substate modes */

enum
{
	BRLT_ON_KIV_TOUT_M_ID = 0,
	BRLT_ON_KIV_DEV_REPORT_M_ID,
	BRLT_ON_KIV_DEV_FW_REPORT_M_ID,
	BRLT_ON_KIV_TSET_M_ID,
	BRLT_ON_KIV_VSET_M_ID,
	
	BRLT_ON_KIV_M_NUM,
}; /* menu on kiv substates modes */

enum
{
	BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID,
	BRLT_ON_KIV_SET_AUTOKIV_TEMP_M_ID,

	BRLT_ON_KIV_SET_AUTOKIV_M_NUM,
}; /* menu on set kiv substate modes*/

enum
{
	BRLT_INIT_CLEAR_M_ID = 0,
	BRLT_INIT_ERASE_BONDS_M_ID,
	BRLT_INIT_REPORT_DEV_M_ID,

	BRLT_INIT_M_NUM,
}; /* menu init substates */

enum
{
	BRLT_OFF_CLEAR_M_ID = 0,
	BRLT_OFF_DEFAULT_M_ID,
	BRLT_OFF_ERROR_M_ID,

	BRLT_OFF_M_NUM,
}; /* brlt off substates */

//typedef uint32_t (*set_indication_t)(void * p_arg, uint32_t length);

typedef struct _tag_brlt_menu_off_state_t
{
	uint32_t size;
	set_indication_t * p_menu_sub_m_list;
}brlt_menu_m_list_t;

typedef struct _tag_brlt_menu_state_t
{
	brlt_menu_m_list_t			menu[BRLT_MENU_SIZE];
	uint32_t 					state;
	uint32_t 					sub_state;
	uint32_t					sub_state_mode;
	brc_main_t					* p_brc_main;
	brlt_state_t				* p_brlt_state;
	device_instance_callback_t	p_brlt_callback;
	uint8_t 					* p_prev_menu_sub_state_mode;
}brlt_menu_state_t;
/*Exported constants*/
extern const menu_m_list_t	brlt_menu_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
brlt_menu_state_t * brlt_menu_get_mode(void);
#endif /*__BRLT_MENU_H*/
/******************************* END_OF_FILE **********************************/