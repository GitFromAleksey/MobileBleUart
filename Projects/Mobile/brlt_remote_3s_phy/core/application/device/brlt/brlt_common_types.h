/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRLT_COMMON_TYPES_H
#define __BRLT_COMMON_TYPES_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*Exported defines*/
// clang-format off
#define ERROR_TYPE_NUMBER			20
#define BRLT_MAX_TEMP				25
#define BRLT_MIN_TEMP				0
#define NUM_OF_PRESETS				3

#define BRLT_AUTOKIV_MAX_TEMP		15
#define BRLT_AUTOKIV_MIN_TEMP		-15

enum
{
	BRLT_DEFAULT_COM = 0,
	BRLT_REQ_SYS_PARAMS_ID,
	BRLT_REQ_DEV_PARAMS_ID,
	BRLT_SET_PARAMS_ID,
	BRLT_SET_FILTER_TIME_ID,
	BRLT_RESET_FILTER_TIME_ID,
	BRLT_HARD_RESET_ID,
	BRLT_SET_SRV_MODE_ID,
	BRLT_SET_AUTOKIV_PARAMS_ID,
	BRLT_GET_AUTOKIV_PARAMS_ID,
	
	BRLT_COM_COUNT,
};
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef __packed struct _tag_brlt_sys_info_t
{
	uint8_t 		system_mode;
	uint16_t		device_type;
	uint16_t		device_sub_type;
	uint16_t		firmware_version;
	uint16_t		hardware_version;
	uint8_t 		reserved[16];
} brlt_sys_info_t;

typedef __packed struct _tag_brlt_dev_state_flags_t
{
	uint16_t		power_mode : 1;
	uint16_t		snd_mode : 1;
	uint16_t		led_mode : 1;
	uint16_t		last_com_source : 1;
	uint16_t		filter_wornout : 1;
	uint16_t		auto_co2_mode : 1;
	uint16_t		heater_mode : 1;
	uint16_t		heater_present : 1;
	uint16_t		kiv_present : 1; //  the presence of the KIV mode
	uint16_t		kiv_active : 1; // state of the KIV mode
	uint16_t		reserved : 6;
}  brlt_dev_state_flags_t; /* from brlt */

typedef __packed struct _tag_brlt_dev_user_set_t
{
	uint8_t 		gate_state;
	 int8_t			target_temp;
	uint8_t			fan_speed;
} brlt_dev_user_set_t;

typedef __packed struct _tag_brlt_dev_sensor_state_t
{
	 int8_t			temp_input;
	 int8_t			temp_output;
	 int8_t			temp_pcb;
} brlt_dev_sensor_state_t;

typedef __packed struct _tag_brlt_dev_conters_t
{
	uint32_t		power_up_time;
	uint32_t		fan_work_time;
	uint32_t		filter_time;
	uint32_t		airflow_cnt;
} brlt_dev_counters_t;

typedef __packed struct _tag_brlt_dev_errors_t
{
	uint32_t		flag_register;
	uint8_t			counter[ERROR_TYPE_NUMBER];
} brlt_dev_errors_t;

typedef __packed struct _tag_brlt_dev_preset_t
{
	 int8_t			target_temp[NUM_OF_PRESETS];
	uint8_t			fan_speed[NUM_OF_PRESETS];
} brlt_dev_preset_t;

typedef __packed struct _tag_brlt_dev_autokiv_t
{
	uint8_t			autokiv_state : 1;
	uint8_t			reserved : 7;
	 int8_t			autokiv_temp;
} brlt_dev_autokiv_info_t;

typedef __packed struct _tag_brlt_dev_misc_info_t
{
	uint8_t			max_fan_speed;
	uint8_t			pid_value;
	uint8_t			test_type;
} brlt_dev_misc_info_t;

typedef __packed struct _tag_brlt_dev_info_t
{
	brlt_dev_state_flags_t		flags;
	brlt_dev_user_set_t			user_set;
	brlt_dev_sensor_state_t		sensors;
	brlt_dev_counters_t			counters;
	brlt_dev_errors_t			errors;
	brlt_dev_preset_t			presets;
	brlt_dev_misc_info_t		misc_info;
	brlt_dev_autokiv_info_t		autokiv_info;
} brlt_dev_info_t;

typedef struct _tag_brlt_state_t
{
	brlt_sys_info_t		sys_info;
	brlt_dev_info_t		dev_info;
	uint8_t 			brlt_invalid_speed;
	uint8_t 			brlt_first_responce_sys_received;
	uint8_t 			brlt_first_responce_dev_received;
} brlt_state_t;

/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/

#endif /*__BRLT_COMMON_TYPES_H*/
/******************************* END_OF_FILE **********************************/