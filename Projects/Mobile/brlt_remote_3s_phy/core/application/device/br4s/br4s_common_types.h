/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BR4S_COMMON_TYPES_H
#define __BR4S_COMMON_TYPES_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*Exported defines*/
// clang-format off
#define BR4S_ERROR_TYPE_NUMBER			20
#define BR4S_MAX_TEMP				25
#define BR4S_MIN_TEMP				0

#define BR4S_ON_TIMER_ID			0
#define BR4S_OFF_TIMER_ID			1

#define BR4S_ON_TIMER_REM_IND			0
#define BR4S_OFF_TIMER_REM_IND			1

#define BR4S_INFLOW_SUBSTATE			0
#define BR4S_RECYCLE_SUBSTATE			1

enum
{
	BR4S_DEFAULT_COM = 0,
	BR4S_REQ_SYS_PARAMS_ID,
	BR4S_REQ_DEV_PARAMS_ID,
	BR4S_SET_PARAMS_ID,
	BR4S_SET_FILTER_TIME_ID,
	BR4S_RESET_FILTER_TIME_ID,
	BR4S_HARD_RESET_ID,
	BR4S_REQ_TIME_ID,
	BR4S_SET_TIME_ID,
	BR4S_REQ_ON_TIMER_ID,
	BR4S_REQ_OFF_TIMER_ID,
	BR4S_SET_ON_TIMER_ID,
	BR4S_SET_OFF_TIMER_ID,
	BR4S_SET_OFF_TIMER_OFF_ID,
	BR4S_SET_ON_TIMER_OFF_ID,

	BR4S_COM_COUNT,
};
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef __packed struct _tag_br4s_sys_info_t
{
	uint8_t 		system_mode;
	uint16_t		device_type;
	uint16_t		device_sub_type;
	uint16_t		firmware_version;
	uint16_t		hardware_version;
	uint8_t 		reserved[16];
} br4s_sys_info_t;


#define ERROR_TYPE_NUMBER			20
typedef struct
{
	uint8_t errors[ERROR_TYPE_NUMBER];
}br4s_error_counter_t;

typedef uint32_t br4s_error_register_t;

typedef struct _tagblrt_error_data_t
{
	br4s_error_register_t reg;
	br4s_error_counter_t cnt;
} br4s_error_data_t;

typedef enum {
  	BR4S_GATE_MIX,
	BR4S_GATE_CLOSE,
	BR4S_GATE_OPEN
} br4s_gate_st_t;

typedef enum {
	BR4S_HEATER_NONE = 0,
	BR4S_HEATER_1_KW,
	BR4S_HEATER_1_4_KW,

	BR4S_HEATER_TYPES,
}	br4s_heater_type_t;

typedef enum {
 	BR4S_NO_TEST,
 	BR4S_DEFAULT_TEST,

	BR4S_MAX_VALUE_TEST,
}br4s_test_type_t;

typedef struct
{
	uint8_t substate; /* 0 - inflow, 1 - recirculation */
	int8_t target_temp;
	uint8_t fan_speed;
} br4s_main_settings_t;

typedef struct
{
	uint8_t max_fan_speed;
} br4s_settings_limits_t;

typedef struct
{
	int8_t 	in_temp;
	int8_t 	out_temp;
	int8_t 	ctrl_pcb_temp;
	int8_t	pwr_pcb_temp;
} br4s_sensors_state_t;

typedef struct
{
	uint32_t work_time;
	uint32_t fan_time;
	uint32_t filter_time;
	uint32_t speed_counter;
} br4s_counters_t;

typedef struct
{
	uint16_t power_state : 1;
	uint16_t sound_ind_state : 1;
	uint16_t led_ind_state : 1;
	uint16_t heater_state : 1;
	uint16_t heater_mode : 1;
	uint16_t last_com_source : 1;
	uint16_t filter_wornout : 1;
	uint16_t heater_present : 3;
	uint16_t magic_air : 1;
	uint16_t ma_auto : 1;
	uint16_t active_timer : 1;
	uint16_t reserved : 3;
} br4s_established_flags_t;

typedef struct
{
	br4s_established_flags_t estabilished;
	uint16_t filter_wornout : 1;
	uint16_t ready_to_update : 1;
} br4s_status_flags_t;

typedef struct
{
	uint8_t		fan_state;
	 int8_t		t_set;
	uint8_t		heater_mode : 1;
	uint8_t		reserved	: 7;
}	br4s_substate_options_t;

typedef struct
{
	uint8_t		Monday : 1;
	uint8_t		Tuesday : 1;
	uint8_t		Wednesday : 1;
	uint8_t		Thursday : 1;
	uint8_t		Friday : 1;
	uint8_t		Saturday : 1;
	uint8_t		Sunday : 1;
	uint8_t		reserved : 1;
	uint8_t		hours;
	uint8_t		minutes;
}	br4s_timer_time_t;

typedef struct
{
	uint8_t power_state : 1;
	uint8_t sound_ind_state : 1;
	uint8_t led_ind_state : 1;
	uint8_t heater_mode : 1; // 1 - temperature maintenance, 0 - heating
	uint8_t	timer_state : 1; /* on/off */
	uint8_t reserved : 3;	
}	br4s_timer_flags_t;

typedef __packed struct
{
	br4s_timer_time_t		timer_time;
	br4s_timer_flags_t		timer_flags;
	 int8_t					target_temp;
	uint8_t					fan_state;
	uint8_t					device_mode;
}	br4s_timer_settings_t;

typedef struct
{
	int64_t		unix_time;
	int64_t 	timezone_offset;
}	br4s_time_format_t;

typedef struct 
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint16_t year;
}   rtc_time_format_t;

#define NUM_OF_TIMERS	2
typedef struct
{
	bool timer_state[NUM_OF_TIMERS];
}	br4s_timers_state_t;

typedef  struct 
{
	br4s_established_flags_t 	flags;
	br4s_main_settings_t		main_params;
	br4s_sensors_state_t		sensors_state;
	br4s_counters_t				counters;
	br4s_error_register_t		errors;
	uint8_t						max_speed;
	 int8_t						heater_var;
}	br4s_dev_info_t;

typedef struct _tag_br4s_state_t 
{
	br4s_sys_info_t			sys_info;
	br4s_dev_info_t			dev_info;
	uint8_t 				br4s_first_responce_sys_received;
	uint8_t					br4s_first_responce_dev_received;
	uint8_t					waiting_responce;
	br4s_timer_settings_t	timers[NUM_OF_TIMERS];
	rtc_time_format_t		dev_time;
	int64_t 				timezone_offset;
	uint8_t					br4s_invalid_speed;
} br4s_state_t;

/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/

#endif /*__BR4S_COMMON_TYPES_H*/
/******************************* END_OF_FILE **********************************/