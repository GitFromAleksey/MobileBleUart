/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __TIMER_DRV_H
#define __TIMER_DRV_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "app_timer.h"
#include "app_error.h"

/*Exported defines*/
// clang-format off
#define TIMER_CONFIG_SIZE				sizeof(timer_config_t)
// clang-format on
/*Exported macro*/
// clang-format off
#define USER_TIMEOUT_TIMER_RELOAD_VALUE			10000
#define USER_MENU_TIMEOUT_TIMER_RELOAD_VALUE	20000
#define GET_STATUS_TIMER_RELOAD_VALUE			750
#define TIMESLOT_TIMER_RELOAD_VALUE				10
#define BLINK_TIMER_RELOAD_VALUE				500
#define BUTTON_BLOCK_TIMER_RELOAD_VALUE			10
#define AUTO_SLEEP_TIMER_RELOAD_VALUE			30000
#define SWITCH_DEV_TIMER_RELOAD_VALUE			1000
#define CONT_PARAM_CHANGE_TIMER_RELOAD_VALUE	100
#define INVALID_SPEED_TIMER_RELOAD_VALUE		3000
#define SCREEN_TEST_TIMER_RELOAD_VALUE			100
#define BRC_REPORT_FW_TIMER_RELOAD_VALUE		1000

#define SET_GET_STATUS_TIMER_INSTANCE(TARGET, CONTEXT)			\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = GET_STATUS_TIMER_ID,                            	\
		.timeout = GET_STATUS_TIMER_RELOAD_VALUE,				\
		.p_context	= CONTEXT								  	\
	}

#define SET_TIMESLOT_TIMER_INSTANCE(TARGET, CONTEXT)						\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = TIMESLOT_TIMER_ID,                            	\
		.timeout = TIMESLOT_TIMER_RELOAD_VALUE,					\
		.p_context	= CONTEXT								  	\
	}

#define SET_BLINK_TIMER_INSTANCE(TARGET, CONTEXT)						\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = BLINK_TIMER_ID,                     		       	\
		.timeout = BLINK_TIMER_RELOAD_VALUE,					\
		.p_context	= CONTEXT								  	\
	}

#define SET_BUTTON_TIMER_INSTANCE(TARGET, CONTEXT)						\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = BUTTON_TIMER_ID,                     		       	\
		.timeout = BUTTON_BLOCK_TIMER_RELOAD_VALUE,					\
		.p_context	= CONTEXT					  				\
	}

#define SET_AUTO_SLEEP_TIMER_INSTANCE(TARGET, CONTEXT)					\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = AUTO_SLEEP_TIMER_ID,                     		   	\
		.timeout = AUTO_SLEEP_TIMER_RELOAD_VALUE,				\
		.p_context	= CONTEXT									\
	}
#define SET_SLEEP_TIMER_INSTANCE(TARGET, TIMEOUT, CONTEXT)						\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = AUTO_SLEEP_TIMER_ID,                     		   	\
		.timeout = TIMEOUT,				\
		.p_context	= CONTEXT									\
	}
#define SET_USER_TIMEOUT_TIMER_INSTANCE(TARGET, CONTEXT)					\
	timer_config_t (TARGET) = 									\
	{                                                       	\
		.id = USER_TIMEOUT_TIMER_ID,                     	   	\
		.timeout = USER_TIMEOUT_TIMER_RELOAD_VALUE,				\
		.p_context	= CONTEXT									\
	}

#define SET_USER_MENU_TIMEOUT_TIMER_INSTANCE(TARGET, CONTEXT)					\
	timer_config_t (TARGET) = 									\
	{                                                       	\
		.id = USER_TIMEOUT_TIMER_ID,                     	   	\
		.timeout = USER_MENU_TIMEOUT_TIMER_RELOAD_VALUE,		\
		.p_context	= CONTEXT									\
	}

#define SET_MISC_TIMER_INSTANCE(TARGET, TIMEOUT, CONTEXT)						\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = MISC_TIMER_ID,                            		\
		.timeout = TIMEOUT,										\
		.p_context	= CONTEXT								  	\
	}	

#define SET_CONT_PARAM_CHANGE_TIMER_INSTANCE(TARGET, CONTEXT)	\
	timer_config_t (TARGET) = 									\
	{                                                         	\
		.id = CONT_PARAM_CHANGE_TIMER_ID,                       \
		.timeout = CONT_PARAM_CHANGE_TIMER_RELOAD_VALUE,		\
		.p_context	= CONTEXT								  	\
	}	

#define SET_INVALID_SPEED_TIMER_INSTANCE(TARGET, CONTEXT)		\
	timer_config_t (TARGET) = 									\
	{															\
		.id = MISC_TIMER_ID,									\
		.timeout = INVALID_SPEED_TIMER_RELOAD_VALUE,			\
		.p_context	= CONTEXT								  	\
	}	

#define SET_SCREEN_TEST_TIMER_INSTANCE(TARGET, CONTEXT)			\
	timer_config_t (TARGET) = 									\
	{															\
		.id = MISC_TIMER_ID,									\
		.timeout = SCREEN_TEST_TIMER_RELOAD_VALUE,				\
		.p_context	= CONTEXT								  	\
	}	

#define SET_BRC_REPORT_FW_TIMER_INSTANCE(TARGET, CONTEXT)			\
	timer_config_t (TARGET) = 									\
	{															\
		.id = MISC_TIMER_ID,									\
		.timeout = BRC_REPORT_FW_TIMER_RELOAD_VALUE,				\
		.p_context	= CONTEXT								  	\
	}	
// clang-format on
/*Exported types*/
typedef enum
{
	GET_STATUS_TIMER_ID = 0,
	TIMESLOT_TIMER_ID,
	BLINK_TIMER_ID,
	BUTTON_TIMER_ID,
	AUTO_SLEEP_TIMER_ID,
	USER_TIMEOUT_TIMER_ID,
	MISC_TIMER_ID,
	CONT_PARAM_CHANGE_TIMER_ID,
	NUMBER_OF_USER_TIMERS,
}list_of_user_timers_t;

typedef enum
{
	START_TIMER = 0,
	STOP_TIMER,
	RELOAD_TIMER,
	GET_TIMER_STATE,
}timer_drv_input_evt_type_t;

typedef enum
{
	GET_STATUS_TIMER_OVF = 0,
	TIMESLOT_TIMER_OVF,
	BLINK_TIMER_OVF,
	BUTTON_TIMER_OVF,
	AUTO_SLEEP_TIMER_OVF,
	USER_TIMEOUT_TIMER_OVF,
	MISC_TIMER_OVF,
	CONT_PARAM_CHANGE_TIMER_OVF,
	GET_STATUS_TIMER_FAIL,
	TIMESLOT_TIMER_FAIL,
	BLINK_TIMER_FAIL,
	REPORT_TIMER_STATE,
}timer_drv_output_evt_type_t;

typedef struct _tag_timer_config_t
{
	uint32_t 	timeout;
	uint8_t 	id;
	void		* p_context;
}timer_config_t;

typedef uint32_t (*timer_drv_input_evt_handler_t)(timer_drv_input_evt_type_t event,
 				uint8_t * p_data, uint16_t data_len);
typedef void (*timer_drv_output_evt_handler_t)(timer_drv_output_evt_type_t event, 
				uint8_t * p_data, uint16_t data_len);

/*Exported constants*/

/*Exported functions prototypes*/
timer_drv_input_evt_handler_t timer_drv_init(timer_drv_output_evt_handler_t 
										 timer_drv_output_evt_handler);

#endif /*__TIMER_DRV_H*/
/******************************* END_OF_FILE **********************************/