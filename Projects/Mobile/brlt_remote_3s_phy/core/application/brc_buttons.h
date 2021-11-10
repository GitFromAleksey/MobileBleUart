/*******************************************************************************
* @file    brc_buttons.h
* @author  Savenko S.A. Sorokin N.N
* @date    02-08-2018
* @brief
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRC_BUTTONS_H
#define __BRC_BUTTONS_H

/*Includes*/
#include <stdint.h>

#include "app_gpiote.h"
#include "app_button.h"
#include "app_timer.h"

#include "platform.h"
/*Exported defines*/
// clang-format off
#define REM_BUTTONS_NUMBER 8


#define REM_BUTTON_POWER  			REM_BUTTON_POWER_PIN 	//POWE
#define REM_BUTTON_STATUS 			REM_BUTTON_DISP_PIN 	//GATE
#define REM_BUTTON_SET				REM_BUTTON_MENU_PIN		//SET
#define REM_BUTTON_GATE				REM_BUTTON_FLOW_PIN 	//STATE
#define REM_BUTTON_TEMP_UP			REM_BUTTON_TUP_PIN 		//TEMP_UP
#define REM_BUTTON_TEMP_DOWN		REM_BUTTON_TDW_PIN 		//TEMP_DOWN
#define REM_BUTTON_SPEED_UP			REM_BUTTON_SPUP_PIN  	//SPEED_UP
#define REM_BUTTON_SPEED_DOWN		REM_BUTTON_SPDW_PIN 	//SPEED_DOWN

#define REM_BUTTONS_LIST														   \
	{	REM_BUTTON_POWER, REM_BUTTON_STATUS, REM_BUTTON_SET, REM_BUTTON_GATE,				   \
		REM_BUTTON_TEMP_UP, REM_BUTTON_TEMP_DOWN, REM_BUTTON_SPEED_UP, REM_BUTTON_SPEED_DOWN,  \
	}

#define BUTTON_PUSH APP_BUTTON_PUSH
#define BUTTON_RELEASE APP_BUTTON_RELEASE
#define BUTTON_LONG_PUSH 2

#define BUTTON_LONG_PUSH_FAN_TEMP_TIME 1000
#define BUTTON_SMLT_LONG_PUSH_STATE_SET_GATE_TIME 3000
#define BUTTON_SMLT_LONG_PUSH_STATE_SET_TIME 5000
#define BUTTON_SMLT_LONG_PUSH_STATE_SET_FAN_UP_FAN_DOWN_TIME 5000
#define BUTTON_SMLT_PUSH_TEMP_DOWN_TEMP_UP_TIME 2000
#define BUTTON_SMLT_LONG_PUSH_TEMP_DOWN_TEMP_UP_SET_TIME 3000
#define BUTTON_SMLT_PUSH_TEMP_DOWN_POWER_TIME 2000
#define BUTTON_SMLT_LONG_PUSH_STATUS_POWER_TIME 4000
#define BUTTON_SMLT_LONG_PUSH_GATE_SET_TEMP_DOWN_SPEED_UP_TIME 4000

enum
{
	BTN_PWR_ID = 0,
	BTN_DISP_ID,
	BTN_MENU_ID,
	BTN_FLOW_ID,
	BTN_TUP_ID,
	BTN_TDW_ID,
	BTN_SPUP_ID,
	BTN_SPDW_ID,
	BTN_QUANITY,
	BTN_FALSE_ID = -1,
};

enum
{
	BUTTON_EVENT_NOTHING = 0,
	POWER_BUTTON_RELEASE,
	STATE_BUTTON_RELEASE,
	SET_BUTTON_RELEASE,
	GATE_BUTTON_RELEASE,
	FAN_UP_BUTTON_RELEASE,
	FAN_DOWN_BUTTON_RELEASE,
	TEMP_UP_BUTTON_RELEASE,
	TEMP_DOWN_BUTTON_RELEASE,

	FAN_UP_BUTTON_LONG_PUSH,
	FAN_DOWN_BUTTON_LONG_PUSH,
	TEMP_UP_BUTTON_LONG_PUSH,
	TEMP_DOWN_BUTTON_LONG_PUSH,

	SMLT_LONG_PUSH_STATE_SET_GATE,
	SMLT_LONG_PUSH_STATE_SET,
	SMLT_LONG_PUSH_STATE_SET_FAN_UP_FAN_DOWN,
	SMLT_LONG_PUSH_TEMP_DOWN_TEMP_UP_SET,
	SMLT_PUSH_TEMP_DOWN_TEMP_UP,
	SMLT_LONG_PUSH_TEMP_DOWN_POWER,
	SMLT_LONG_PUSH_STATUS_POWER,
	SMLT_LONG_PUSH_GATE_SET_TEMP_DOWN_SPEED_UP,
};

typedef void (* my_button_event_callback_t)(uint32_t evt,
										uint8_t * p_data, uint32_t data_len);

void my_button_init (my_button_event_callback_t callback);

uint8_t check_erase_bond_button (void);
uint8_t check_all_buttons (void);
uint8_t check_pairing_buttons (void);

#endif /*__BRC_BUTTONS_H*/