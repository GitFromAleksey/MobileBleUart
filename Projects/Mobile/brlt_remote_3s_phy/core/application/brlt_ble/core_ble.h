/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __CORE_BLE_H
#define __CORE_BLE_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ble_nus_c.h"
#include "ma_ble_nus_c.h"
#include "softdevice_handler.h"
/*Exported defines*/
// clang-format off
enum
{
	DEFAULT_READ = 0,
	SYS_READ,
	LAST_RX_READ,
	NUM_OF_READ_TYPES,
};

enum
{
	DEFAULT_BLE_EVT = 0,
	RX_DATA,
	CONNECTION,
	CLEAR_W_LIST,
	CLEAR_W_LIST_FAIL,
	FOUND_CONNECTABLE_SUPPORTED_DEV,
	DISCONNECTION,
};

typedef enum
{
	BLE_NO_SCAN,		/**< No advertising running. */
	BLE_WHITELIST_SCAN, /**< Advertising with whitelist. */
	BLE_FAST_SCAN,		/**< Fast advertising running. */
} ble_scan_mode_t;
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct
{
	uint8_t *p_data;   
	uint16_t data_len;
} data_t;


typedef struct _tag_ble_rx_t
{
	uint16_t	evt_type;
	uint16_t 	dev_type;
	uint16_t 	lenght;
	uint8_t 	* p_rx_buff;
}ble_rx_t;

typedef struct _tag_ble_sys_t
{
	uint16_t 	m_scan_mode;
	uint16_t 	m_conn_handle;
	uint16_t 	m_active_dev_type;
	uint8_t 	m_peer_count;
	bool 		m_in_scan;
	bool 		m_memory_access_in_progress;
	bool 		m_delete_bond_in_progress;
	bool 		m_whitelist_temporarily_disabled;
	bool		m_scan_permition;
}ble_sys_t;

typedef struct _tag_ble_read_out_t
{
	uint16_t 	read_type;
	union
	{
		ble_rx_t 	* p_ble_rx;
		ble_sys_t 	* p_ble_sys;
	};
}ble_read_out_t;

typedef uint32_t (*ble_adv_data_parser_t)(uint32_t pair_mode,
			const ble_gap_evt_adv_report_t * p_adv_data);

typedef uint32_t (*ble_rx_callback_t)(ble_rx_t * p_rx_data);

typedef uint32_t (*ble_init_instance_t)(void * p_context, 
									ble_rx_callback_t p_ble_rx_callback);
typedef uint32_t (*ble_deinit_instance_t)(void * p_context);
typedef uint32_t (*ble_write_data_to_instance_t)(void * p_context);
typedef uint32_t (*ble_read_data_from_instance_t)(void * p_context);
typedef uint32_t (*ble_sys_m_t)(void * p_context);
typedef uint32_t (*ble_sys_data_m_t)(void * p_context, uint32_t * data);

typedef struct _tag_ble_sys_m_list_t
{
	ble_sys_m_t 		pair;
	ble_sys_m_t			erase_bonds;
	ble_sys_m_t			execute;
	ble_sys_m_t			connect;
	ble_sys_m_t			disconnect;
	ble_sys_m_t			scan_permit;
	ble_sys_m_t			scan_forbid;
	ble_sys_data_m_t	dev_filter;
}ble_sys_m_list_t;

typedef struct _tag_ble_m_list_t
{
	ble_init_instance_t 			init;
	ble_deinit_instance_t			deinit;
	ble_write_data_to_instance_t	write;
	ble_read_data_from_instance_t	read;
	ble_sys_m_list_t				sys;
}ble_m_list_t;	
/*Exported constants*/
extern const ble_m_list_t	ble_m_list;
/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__CORE_BLE_H*/
/******************************* END_OF_FILE **********************************/