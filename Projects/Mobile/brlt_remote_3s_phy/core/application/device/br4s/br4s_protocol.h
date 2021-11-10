/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BR4S_PROTOCOL_H
#define __BR4S_PROTOCOL_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "core_ble.h"
#include "br4s_common_types.h"
#include "protocol_interface.h"
#include "main_common.h"
#include "nrf_queue.h"



/*Exported macro*/
// clang-format off
#define CONVERT_SEC_TO_DAYS(_target_, _source_)		\
	do{ if (_source_%(60*60*24))					\
		{_target_ = (_source_/(60*60*24)) + 1;}		\
		else										\
		{_target_ = _source_/(60*60*24);}}while(0)		
#define CONVERT_DAYS_TO_SEC(_target_, _source_)		\
	do{target_ = (_source_)*(60*60*24);}while(0)	
// clang-format on
/*Exported defines*/
// clang-format off
/* MA_transport frame types*/	
#define BLE_MAGIC_NUMBER 						0x3A

typedef enum{
    BR4S_PACKET_TYPE_FIRST   = 0x00,
    BR4S_PACKET_TYPE_CURRENT = 0x40,
    BR4S_PACKET_TYPE_FEND    = 0x80,
    BR4S_PACKET_TYPE_END     = 0xC0, //Bytes of MA packets
    BR4S_PACKET_TYPE_NONE	= 0x01,
}br4s_packet_type_t;

/* Breezer 4S frame types*/	 
enum{
    BR4S_FRAME_TYPE_SET_MODE_NO_SAVE_REQ	=	0x3230,
    BR4S_FRAME_TYPE_GET_MODE_RESP		=	0x3231,
    BR4S_FRAME_TYPE_GET_MODE_REQ         =	0x3232,
    BR4S_FRAME_TYPE_SET_MODE_SAVE_REQ	=	0x3234,
    BR4S_FRAME_TYPE_DEV_STATUS_REQ		=   0x3332,
    BR4S_FRAME_TYPE_DEV_STATUS_RESP 		=	0x3331,
    BR4S_FRAME_TYPE_TEST_REQ             =   0x3132,
    BR4S_FRAME_TYPE_TEST_RESP            =   0x3131,
    BR4S_FRAME_TYPE_SET_TIMER_REQ        =   0x3430,
    BR4S_FRAME_TYPE_GET_TIMER_REQ        =   0x3432,
    BR4S_FRAME_TYPE_GET_TIMER_RESP       =   0x3431,
    BR4S_FRAME_TYPE_GET_TIMERS_STATE_REQ =   0x3532,
    BR4S_FRAME_TYPE_GET_TIMERS_STATE_RESP=   0x3531,
    BR4S_FRAME_TYPE_SET_TIME_REQ         =   0x3630,
    BR4S_FRAME_TYPE_GET_TIME_REQ         =   0x3632,
    BR4S_FRAME_TYPE_GET_TIME_RESP        =   0x3631,
    BR4S_FRAME_TYPE_GET_ERR_CNT_REQ      =   0x3732,
    BR4S_FRAME_TYPE_GET_ERR_CNT_RESP     =   0x3731,
    BR4S_FRAME_TYPE_SET_TEST_REQ         =   0x3830,
    BR4S_FRAME_TYPE_GET_CURR_TEST_REQ    =   0x3832,
    BR4S_FRAME_TYPE_GET_CURR_TEST_RESP   =   0x3831,
};

#define BR4S_CRC_SIZE            				sizeof(br4s_crc_t)
#define BR4S_META_SIZE            				(sizeof(ma_meta_br4s_frame_t) - BR4S_CRC_SIZE)

#define BR4S_RAW_INPUT_QUEUE_SIZE		5
#define BR4S_RAW_BUFFER_SIZE			sizeof(br4s_frame_t)
#define BR4S_RAW_OUTPUT_BUFFER_SIZE		sizeof(br4s_data_t)
// clang-format on
/*Exported macro*/
// clang-format off
// clang-format on
/*Exported types*/
typedef __packed uint16_t br4s_crc_t;

typedef __packed struct _tagBLE_DevStatusFrame_TypeDef
{
	uint8_t WorkMode;
	uint16_t DevType;
	uint16_t DevSType;
	uint16_t SoftVer;
	uint16_t HardVer;
	uint8_t Reserved[16];
} br4s_dev_status_t;

typedef struct
{
	uint16_t power_state : 1;
	uint16_t sound_ind_state : 1;
	uint16_t led_ind_state : 1;
	uint16_t heater_mode : 1;
	uint16_t last_com_source : 1;
	uint16_t factory_reset : 1;
	uint16_t error_reset : 1;
	uint16_t filter_reset : 1;
	uint16_t magic_air : 1;
	uint16_t ma_auto : 1;
	uint16_t reserved : 6;
} 	br4s_rc_command_flags_t;

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
} 	br4s_rc_response_flags_t;

/* set dev params request data field */
typedef __packed struct _tagbr4s_rc_set_cmd_t
{
	uint32_t cmd_id;

	br4s_rc_command_flags_t  flags;
	br4s_main_settings_t 	params;
	uint16_t 	filter_time_in_days;
} 	br4s_rc_set_cmd_t;

/* dev_params responce data field */
typedef __packed struct _tagbr4s_rc_response_t
{
	uint32_t last_cmd_id;
	
	br4s_rc_response_flags_t flags;
	br4s_main_settings_t 	params;
	br4s_sensors_state_t 	sensor_state;
	br4s_counters_t 		counters;
	br4s_error_register_t	errors;
	br4s_settings_limits_t	limits;
	uint8_t					heater_var;
} br4s_rc_response_t;

typedef struct
{
	uint8_t pairing_state : 1;
	uint8_t reserved : 7;
}	br4s_dev_state_flags_t;

typedef __packed struct _tag_br4s_ble_adv_dev_info_t
{
	uint16_t com_id;
	uint8_t dev_mac[BLE_GAP_ADDR_LEN]; 	/*WTF? BLE Mac in advertising packets by
                                        default */
	uint16_t dev_type; 					/* Br3.0, CO2+, Ir-remote */
	uint16_t dev_stype; 				/* sub-type */
	uint8_t  dev_pair; 					/* ready for pairing */
} br4s_ble_adv_dev_info_t;

typedef __packed struct _tagbr4s_rc_time_t
{
	uint32_t 				cmd_id;
	br4s_time_format_t		time;
}	br4s_rc_time_pack_t;

typedef __packed struct _tagbr4s_rc_timers_state_t
{
	uint32_t 				cmd_id;
	br4s_timers_state_t		timers_state;
}	br4s_rc_timers_state_pack_t;

typedef __packed struct _tagbr4s_rc_timer_pack_t
{
	uint32_t					cmd_id;
	uint8_t					timer_id;
	br4s_timer_settings_t	timer;
}	br4s_rc_timer_pack_t;

typedef __packed struct _tagbr4s_rc_timer_req_pack_t
{
	uint32_t					cmd_id;
	uint8_t					timer_id;
}	br4s_rc_timer_set_req_pack_t;

typedef __packed struct _tagbr4s_rc_errors_state_t
{
	int32_t 				cmd_id;
	br4s_error_counter_t	errors;
}	br4s_rc_errors_pack_t;

typedef __packed struct _tagbr4s_rc_dev_state_t
{
	br4s_dev_state_flags_t	dev_state;
}	br4s_rc_dev_state_pack_t;

typedef struct tag_br4s_data_t
{
	uint8_t *p_data;   
	uint16_t data_len;
}br4s_data_t;

typedef __packed struct _tag_ma_meta_br4s_frame_t
{
	uint16_t 	frame_size;
	uint8_t  	magic_number;
	uint8_t  	random_number;
	uint16_t 	frame_type;
	uint32_t 	request_id;
	uint8_t  	data[BR4S_CRC_SIZE];
}ma_meta_br4s_frame_t;

typedef __packed struct _tag_br4s_raw_frame_t
{
	uint8_t  	packet_type;
	uint8_t  	data[BLE_NUS_MAX_DATA_LEN - 1];
}br4s_raw_frame_t;

typedef __packed struct _tag_br4s_raw_queue_frame_t
{
	uint8_t			data_len;
	br4s_raw_frame_t	data;
}br4s_raw_queue_frame_t;

typedef struct _tag_br4s_protocol_t
{
	brc_main_t					* p_brc_main;
	br4s_state_t  				* p_br4s_params;
	uint8_t						* p_br4s_state_valid;
	br4s_ble_adv_dev_info_t 	* p_br4s_adv_info;
	device_instance_callback_t	p_br4s_callback;
	uint32_t					request_cnt;
} br4s_protocol_t;
/*Exported constants*/
extern const protocol_m_list_t	br4s_protocol_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
ble_adv_data_parser_t br4s_get_adv_parser (void);
#endif /*__BR4S_PROTOCOL_H*/
/******************************* END_OF_FILE **********************************/