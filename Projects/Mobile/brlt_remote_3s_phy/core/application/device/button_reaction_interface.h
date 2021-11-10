/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BUTTON_REACTION_INTERFACE_H
#define __BUTTON_REACTION_INTERFACE_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
enum
{
	BUTTON_INT_EVT_POWER_ID = 0,
	BUTTON_INT_EVT_STATE_ID,
	BUTTON_INT_EVT_SET_ID,
	BUTTON_INT_EVT_GATE_ID,
	BUTTON_INT_EVT_FAN_UP_ID,
	BUTTON_INT_EVT_FAN_DOWN_ID, 
	BUTTON_INT_EVT_TEMP_UP_ID,
	BUTTON_INT_EVT_TEMP_DOWN_ID,
	BUTTON_INT_EVT_LONG_FAN_UP_ID,
	BUTTON_INT_EVT_LONG_FAN_DOWN_ID,
	BUTTON_INT_EVT_LONG_TEMP_UP_ID,
	BUTTON_INT_EVT_LONG_TEMP_DOWN_ID,
	BUTTON_INT_EVT_LONG_STATE_SET_GATE_ID,
	BUTTON_INT_EVT_LONG_STATE_SET_ID,
	BUTTON_INT_EVT_LONG_STATE_SET_FAN_UP_FAN_DOWN_ID,
	BUTTON_INT_EVT_LONG_SET_TEMP_UP_TEMP_DOWN_ID,
	BUTTON_INT_EVT_LONG_TEMP_DOWN_POWER_ID,
	BUTTON_INT_EVT_LONG_STATE_POWER_ID,
	BUTTON_INT_EVT_LONG_GATE_SETTEMP_DOWN_FAN_UP_ID,
	BUTTON_INT_EVT_NUM
};

typedef uint32_t (*init_button_reaction_instance_t)(void * p_arg);
typedef uint32_t (*deinit_button_reaction_instance_t)(void * p_arg);

typedef uint32_t (*button_event_t)(void * p_arg, uint32_t length);

typedef struct _tag_button_reaction_m_list_t
{
	init_button_reaction_instance_t 		init;
	deinit_button_reaction_instance_t		deinit;
	
	button_event_t			power;
	button_event_t			state;
	button_event_t			set;
	button_event_t			gate;
	button_event_t			fan_up;
	button_event_t			fan_down;
	button_event_t			temp_up;
	button_event_t			temp_down;
	
	button_event_t			long_fan_up;
	button_event_t			long_fan_down;
	button_event_t			long_temp_up;
	button_event_t			long_temp_down;
	
	button_event_t			long_state_set_gate;
	button_event_t			long_state_set;
	button_event_t			long_state_set_fan_up_fan_down;
	button_event_t			long_set_temp_up_temp_down;
	button_event_t			long_temp_up_temp_down;
	button_event_t			long_temp_down_power;
	button_event_t			long_state_power;
	button_event_t			long_gate_set_temp_down_fan_up;
}button_reaction_m_list_t;

typedef struct _tag_button_reaction_t
{
	const button_reaction_m_list_t * vptr;
}button_reaction_t;

/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__BUTTON_REACTION_INTERFACE_H*/
/******************************* END_OF_FILE **********************************/