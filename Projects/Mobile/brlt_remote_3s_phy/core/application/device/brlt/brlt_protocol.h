/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRLT_PROTOCOL_H
#define __BRLT_PROTOCOL_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "core_ble.h"
#include "brlt_common_types.h"
#include "protocol_interface.h"
#include "main_common.h"
#include "nrf_queue.h"
/*Exported defines*/
// clang-format off
/* MA_transport frame types*/	
#define BLE_MAGIC_NUMBER 						0x3A

#define BLE_PACKET_TYPE_NONE 					0x01
#define BLE_PACKET_TYPE_FIRST 					0x00
#define BLE_PACKET_TYPE_CURRENT 				0x40
#define BLE_PACKET_TYPE_FEND 					0x80
#define BLE_PACKET_TYPE_END 					0xC0

/* Breezer Lite frame types*/	  
#define BLE_FRAME_TYPE_DEV_STATUS_REQ			0x4009
#define BLE_FRAME_TYPE_DEV_STATUS_RESP			0x400A

#define BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ		0x1230
#define BLE_FRAME_TYPE_GET_MODE_RESP			0x1231
#define BLE_FRAME_TYPE_GET_MODE_REQ				0x1232	
#define BLE_FRAME_TYPE_SET_MODE_SAVE_REQ		0x1234

#define BLE_FRAME_TYPE_SET_AUTOKIV_PARAM_REQ	0x1240
#define BLE_FRAME_TYPE_GET_AUTOKIV_PARAM_RESP	0x1241
#define BLE_FRAME_TYPE_GET_AUTOKIV_PARAM_REQ	0x1242

#define BLE_CRC_SIZE            				sizeof(ble_crc_t)
#define BLE_META_SIZE            				(sizeof(ma_meta_ble_frame_t) - BLE_CRC_SIZE)
/* queue size defines */
#define RAW_INPUT_QUEUE_SIZE					3
#define RAW_BUFFER_SIZE							sizeof(ble_raw_frame_t)

// clang-format on
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
/*Exported types*/
typedef __packed struct _tag_brlt_ble_adv_dev_info_t
{
	uint16_t com_id;
	uint8_t dev_mac[BLE_GAP_ADDR_LEN]; 	/*WTF? BLE Mac in advertising packets by
                                        default */
	uint16_t dev_type; 					/* Br3.0, CO2+, Ir-remote */
	uint16_t dev_stype; 				/* sub-type */
	uint8_t  dev_pair; 					/* ready for pairing */
} brlt_ble_adv_dev_info_t;

/* Frame's format */
typedef __packed uint16_t ble_crc_t;

typedef __packed struct _tag_ma_meta_ble_frame_t
{
	uint16_t 	frame_size;
	uint8_t  	magic_number;
	uint8_t  	random_number;
	uint16_t 	frame_type;
	uint32_t 	request_id;
	uint8_t  	data[BLE_CRC_SIZE];
}ma_meta_ble_frame_t;

typedef __packed struct _tag_ble_raw_frame_t
{
	uint8_t  	packet_type;
	uint8_t  	data[BLE_NUS_MAX_DATA_LEN - 1];
}ble_raw_frame_t;

typedef __packed struct _tag_ble_raw_queue_frame_t
{
	uint8_t			data_len;
	ble_raw_frame_t	data;
}ble_raw_queue_frame_t;
/*data field 0x400A*/
typedef __packed struct _tag_sys_state_resp_frame_t
{
	brlt_sys_info_t		sys_info;
}sys_state_resp_frame_t;

/*data field 0x1231*/
typedef __packed struct _tag_dev_state_resp_frame_t
{
	uint32_t cmd_id;
	brlt_dev_info_t		dev_info;
}dev_state_resp_frame_t;

/* data field 0x1241 */
typedef __packed struct _tag_autokiv_state_resp_frame_t
{
	uint32_t cmd_id;
	brlt_dev_autokiv_info_t		autokiv_info;
}autokiv_state_resp_frame_t;

/*0x1230 exclusive subtypes*/
typedef __packed struct _tag_brlt_dev_set_state_flags_t
{
	uint16_t		power_mode : 1;
	uint16_t		snd_mode : 1;
	uint16_t		led_mode : 1;
	uint16_t		auto_co2_mode : 1;
	uint16_t		heater_mode : 1;
	uint16_t		last_com_source : 1;
	uint16_t		factory_reset : 1;
	uint16_t		error_reset : 1;
	uint16_t		filter_reset : 1;
	uint16_t		kiv_active : 1; // state of the KIV mode
	uint16_t		reserved : 6;
} brlt_dev_set_state_flags_t;

typedef __packed struct _tag_brlt_dev_misc_set_t
{
	uint16_t		filter_time;
	uint8_t			test_type;
} brlt_dev_misc_set_t;

/*data field 0x1230*/
typedef __packed struct _tag_dev_state_set_frame_t
{
	uint32_t 					cmd_id;
	brlt_dev_set_state_flags_t	set_flags;
	brlt_dev_user_set_t			set_user_set;
	brlt_dev_preset_t			set_presets;
	brlt_dev_misc_set_t			set_misc;
} dev_state_set_frame_t;

typedef struct _tag_brlt_protocol_t
{
	brc_main_t					* p_brc_main;
	brlt_state_t  				* p_brlt_params;
	uint8_t						* p_brlt_state_valid;
	brlt_ble_adv_dev_info_t 	* p_brlt_adv_info;
	device_instance_callback_t	p_brlt_callback;
	uint32_t 					request_cnt;
} brlt_protocol_t;
/*Exported constants*/
extern const protocol_m_list_t	brlt_protocol_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
ble_adv_data_parser_t brlt_get_adv_parser (void);
#endif /*__BRLT_PROTOCOL_H*/
/******************************* END_OF_FILE **********************************/