/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BR3S_PROTOCOL_H
#define __BR3S_PROTOCOL_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "core_ble.h"
#include "br3s_common_types.h"
#include "protocol_interface.h"
#include "main_common.h"
#include "nrf_queue.h"
#include "B3_Params.h"

/*Exported defines*/
// clang-format off
#define BR3S_RAW_INPUT_QUEUE_SIZE		3
#define BR3S_RAW_BUFFER_SIZE			sizeof(br3s_frame_t)
#define BR3S_RAW_OUTPUT_BUFFER_SIZE		sizeof(br3s_output_com_t)
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef __packed struct _tag_br3s_ble_adv_dev_info_t
{
	uint16_t com_id;
	uint8_t dev_mac[BLE_GAP_ADDR_LEN]; 	/*WTF? BLE Mac in advertising packets by
                                        default */
	uint16_t dev_type; 					/* Br3.0, CO2+, Ir-remote */
	uint16_t dev_stype; 				/* sub-type */
	uint8_t  dev_pair; 					/* ready for pairing */
} br3s_ble_adv_dev_info_t;

typedef __packed struct tag_flags_state_t
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
}flags_state_t;

typedef __packed struct tag_ble_rc_alarm_time_t
{
	uint8_t hours;  
	uint8_t minutes;
}ble_rc_alarm_time_t;

typedef __packed struct tag_system_time_t
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
}system_time_t;

typedef __packed struct tag_system_date_t
{
	uint8_t year;
	uint8_t week_day 	:4;
	uint8_t month   	:4;
	uint8_t day;
}system_date_t;

typedef __packed struct tag_system_time_package_t
{
	system_date_t		date;
	system_time_t		time;
}system_time_package_t;

typedef __packed struct tag_system_alarm_state_t
{
	uint8_t alarm_id         : 4;
	uint8_t alarm_state      : 1;
	uint8_t power_state      : 1; 
	uint8_t com_source       : 2; 
}system_alarm_state_t;

typedef __packed struct tag_system_user_set_t
{
	int8_t fan_speed        : 4;
	int8_t dumper_pos       : 4;
}system_user_set_t;

typedef __packed struct tag_system_alarm_t
{
	system_alarm_state_t   	alarm_status;
	system_user_set_t		alarm_set;
	 int8_t 				temp_set;
	system_time_t			alarm_time_start;
}system_alarm_t;

typedef __packed struct tag_ble_rc_filter_time_set_t
{
	uint8_t ft_save_bit     :1;
	uint8_t ft_reset       	:1;
	uint8_t ft_reserved    	:6;
	uint16_t ft_value;
}ble_rc_filter_time_set_t;

typedef __packed  struct  tag_br3s_get_param_t
{
	uint8_t fan_speed :4;
	uint8_t dumper_pos:4;
	 int8_t t_set;
	flags_state_t br3s_state_of_flags;
	 int8_t output_temp1;
	 int8_t output_temp2;
	 int8_t input_temp;
	uint16_t filter_time; 
	uint8_t us_hour;
	uint8_t us_minutes;  
	uint8_t last_error_nNum; 
	uint8_t m3_per_hour;          
	uint16_t filter_time_days; 
	uint16_t br3s_fv;
}br3s_get_param_t;

typedef __packed  struct tag_br3s_set_param_t
{
	uint8_t fan_speed;
	 int8_t t_set;
	uint8_t dumper_pos;
	flags_state_t 				br3s_state_of_flags;
	ble_rc_filter_time_set_t  	filter_time;
	uint8_t hard_reset_flag;        
	uint8_t service_mode_flag;        
	uint8_t reserved1;      
	uint8_t reserved2;  
	uint8_t reserved3;          
	uint8_t reserved4; 
	uint8_t reserved5;
	uint8_t reserved6;          
	uint8_t reserved7;         
}br3s_set_param_t;

typedef __packed  struct tag_br3s_set_time_t
{
  system_time_package_t us_date_time;
  uint8_t reserved1;     
  uint8_t reserved2;
  uint8_t reserved3;
  uint8_t reserved4;  
  uint8_t reserved5;
  uint8_t reserved6;
  uint8_t reserved7;
  uint8_t reserved8;
  uint8_t reserved9;  
  uint8_t reserved10;
  uint8_t reserved11;
}br3s_set_time_t;

typedef __packed  struct tag_br3s_set_alarm_t
{
  uint8_t com_type;
  system_alarm_t      alarm;
  uint8_t reserved1;          
  uint8_t reserved2;        
  uint8_t reserved3; 
  uint8_t reserved4;
  uint8_t reserved5;
  uint8_t reserved6;
  uint8_t reserved7;
  uint8_t reserved8;
  uint8_t reserved9;  
  uint8_t reserved10;
}br3s_set_alarm_t;

typedef __packed  struct tag_br3s_alarm_response_t
{
  ble_rc_alarm_time_t      alarm1;
  ble_rc_alarm_time_t      alarm2;
  ble_rc_alarm_time_t      alarm3;
  ble_rc_alarm_time_t      alarm4;
  uint8_t reserved1;          
  uint8_t reserved2; 
  uint8_t reserved3; 
  uint8_t reserved4;
  uint8_t reserved5; 
  uint8_t reserved6;
  uint8_t reserved7;          
  uint8_t reserved8; 
  uint8_t reserved9;
}br3s_alarm_response_t;

typedef __packed struct tag_br3s_frame_t
{
	uint8_t devID;
	uint8_t comID;
	union 	
	{
		br3s_get_param_t		get_param;
		br3s_set_param_t		set_param;
		br3s_set_time_t			set_time;
		br3s_set_alarm_t		set_alarm;
		br3s_alarm_response_t	resp_alarm;
	};
	uint8_t CRC8;   
}br3s_frame_t;

typedef struct tag_br3s_data_t
{
 	uint8_t * p_raw_data;
	uint16_t data_size;
}br3s_data_t;

typedef struct tag_br3s_output_com_t
{
 	uint8_t			resp_type;
	br3s_frame_t 	br3s_frame;
}br3s_output_com_t;

enum
{
	BR3S_DUMMY = 0,
	BR3S_REQ_PARAMS_ID,
	BR3S_SET_PARAMS_ID,
	BR3S_SET_FILTER_TIME_ID,
	BR3S_RESET_FILTER_TIME_ID,
	BR3S_SET_SRV_MODE_ID,
	BR3S_HARD_RESET_ID,
	BR3S_START_MA_PAIRING_ID,
	BR3S_SET_TIME_ID,
	BR3S_REQ_ALARM_ID,
	BR3S_SET_ON_ALARM_ID,
	BR3S_SET_OFF_ALARM_ID,
	
	BR3S_COM_COUNT,
};

typedef struct _tag_br3s_protocol_t
{
	brc_main_t					* p_brc_main;
	br3s_state_t  				* p_br3s_params;
	uint8_t						* p_br3s_state_valid;
	br3s_ble_adv_dev_info_t 	* p_br3s_adv_info;
	device_instance_callback_t	p_br3s_callback;
	uint8_t						* p_expected_resp_type; 
} br3s_protocol_t;
/*Exported constants*/
extern const protocol_m_list_t	br3s_protocol_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
ble_adv_data_parser_t br3s_get_adv_parser (void);
#endif /*__BR3S_PROTOCOL_H*/
/******************************* END_OF_FILE **********************************/