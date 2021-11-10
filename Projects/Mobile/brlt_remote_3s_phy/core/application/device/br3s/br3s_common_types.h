/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BR3S_COMMON_TYPES_H
#define __BR3S_COMMON_TYPES_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*Exported defines*/
// clang-format off
#define ERROR_TYPE_NUMBER			20

#define NUM_OF_PRESETS				3

#define GATE_POSITION_RECYCLE		0
#define GATE_POSITION_MIX			1
#define GATE_POSITION_INLET			2
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef __packed struct _tag_br3s_sys_info_t
{
	uint8_t 		system_mode;
	uint16_t		device_type;
	uint16_t		device_sub_type;
	uint16_t		firmware_version;
	uint16_t		hardware_version;
	uint8_t 		reserved[16];
} br3s_sys_info_t;

typedef __packed struct _tag_br3s_dev_state_flags_t
{	
	uint16_t heater_state           : 1;  
	uint16_t power_state            : 1;
	uint16_t timer_state            : 1;
	uint16_t buzzer_state           : 1;
	uint16_t auto_state             : 1;
	uint16_t ma_connect             : 1;
	uint16_t save_bit               : 1;
	uint16_t ma_pairing             : 1;
	uint16_t preset_state           : 1;
	uint16_t reserved              	: 7;
}br3s_dev_state_flags_t;

typedef __packed struct _tag_br3s_dev_info_t
{
  	uint8_t fan_speed :4;
	uint8_t dumper_pos:4;
	 int8_t t_set;
	br3s_dev_state_flags_t flags;
	 int8_t output_temp1;
	 int8_t output_temp2;
	 int8_t input_temp;
	uint16_t filter_time; 
	uint8_t us_hour;
	uint8_t us_minutes;  
	uint8_t er_last_error_num; 
	uint8_t m3_per_hour;          
	uint16_t filter_time_days; 
	uint16_t br3s_fv;
	uint8_t alarm_on_hours;
	uint8_t alarm_on_minutes;
	uint8_t alarm_off_hours;
	uint8_t alarm_off_minutes;
} br3s_dev_info_t;

typedef struct _tag_br3s_state_t
{
	br3s_sys_info_t		sys_info;
	br3s_dev_info_t		dev_info;
	uint8_t 			br3s_invalid_speed;
	uint8_t 			br3s_first_responce_received;
	uint8_t 			waiting_responce;
} br3s_state_t;

/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/

#endif /*__BR3S_COMMON_TYPES_H*/
/******************************* END_OF_FILE **********************************/