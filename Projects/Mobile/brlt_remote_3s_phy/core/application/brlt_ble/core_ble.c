/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Includes*/
#include "main_common.h"

#ifndef FSTORAGE_H__
#include "fstorage.h"
#endif /*FSTORAGE_H__*/
#ifndef FDS_H__
#include "fds.h"
#endif /*FDS_H__*/
#ifndef PEER_MANAGER_H__
#include "peer_manager.h"
#endif /*PEER_MANAGER_H__*/
#ifndef BLE_CONN_STATE_H__
#include "ble_conn_state.h"
#endif /*BLE_CONN_STATE_H__*/


#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "protocol_interface.h"

/*Private defines*/
#define UART_TX_BUF_SIZE 	32 /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 	32 /**< UART RX buffer size. */
#define UART_BUF_SIZE 		32 /**< UART buffer size. */

#define NUS_SERVICE_UUID_TYPE                                             \
	BLE_UUID_TYPE_VENDOR_BEGIN /**< UUID type for the Nordic UART Service \
								  (vendor specific). */
#define MA_NUS_SERVICE_UUID_TYPE                                             \
	BLE_UUID_TYPE_VENDOR_BEGIN /**< UUID type for the Nordic UART Service \
								  (vendor specific). */
#define SEC_PARAM_BOND 1 /**< Perform bonding. */
//#define SEC_PARAM_MITM             1                                  /**<
//Man In The Middle protection not required. */
#define SEC_PARAM_MITM 0	 /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC 0	 /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS 0 /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES \
	BLE_GAP_IO_CAPS_NONE		  /**< No I/O capabilities. */
#define SEC_PARAM_OOB 0			  /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE 7  /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE 16 /**< Maximum encryption key size. */

#define SCAN_INTERVAL \
	0x0140 /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW \
	0x00A0 /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_ACTIVE 1 /**< If 1, performe active scanning (scan requests). */
#define SCAN_SELECTIVE \
	0 /**< If 1, ignore unknown devices (non whitelisted). */
#define SCAN_TIMEOUT \
	0x0000 /**< Timout when scanning. 0x0000 disables timeout. */

#define MIN_CONNECTION_INTERVAL                                        \
	MSEC_TO_UNITS(23, UNIT_1_25_MS) /**< Determines minimum connection \
									   interval in millisecond. */
#define MAX_CONNECTION_INTERVAL                                        \
	MSEC_TO_UNITS(27, UNIT_1_25_MS) /**< Determines maximum connection \
									   interval in millisecond. */
#define SLAVE_LATENCY \
	0 /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT                                                 \
	MSEC_TO_UNITS(2000, UNIT_10_MS) /**< Determines supervision time-out in \
									   units of 10 millisecond. */

#define UUID16_SIZE 2   /**< Size of 16 bit UUID */
#define UUID32_SIZE 4   /**< Size of 32 bit UUID */
#define UUID128_SIZE 16 /**< Size of 128 bit UUID */
		
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

/**@brief Variable length data encapsulation in terms of length and pointer to
 * data */


// clang-format off
#define CENTRAL_LINK_COUNT		1
#define PERIPHERAL_LINK_COUNT	0
#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE    GATT_MTU_SIZE_DEFAULT
#endif


// clang-format on 
/*Private macro*/
// clang-format off

// clang-format on
/*Private types*/

/*Private functions prototypes*/
static void whitelist_disable(void);
static void erase_bonds(void);

static bool is_uuid_present(const ble_uuid_t *p_target_uuid,
							const ble_gap_evt_adv_report_t *p_adv_report);
/*static uint32_t handles_assign(uint8_t found_dev_id, 
							   const ble_gap_evt_t * p_gap_evt);*/

static void ble_stack_init(void);
static void db_discovery_init(void);
static void peer_manager_init(bool erase_bonds);
static void nus_c_init(void);
static void ma_nus_c_init(void);

static void ble_nus_c_evt_handler(ble_nus_c_t *p_ble_nus_c,
								  const ble_nus_c_evt_t *p_ble_nus_evt);
static void ma_ble_nus_c_evt_handler(ma_ble_nus_c_t * p_ma_ble_nus_c,
								  const ma_ble_nus_c_evt_t * p_ma_ble_nus_evt);
static void db_disc_handler(ble_db_discovery_evt_t *p_evt);
static void on_sys_evt(uint32_t sys_evt);
static void on_ble_evt(ble_evt_t *p_ble_evt);
static void pm_evt_handler(pm_evt_t const * p_evt);

static void ble_evt_dispatch(ble_evt_t *p_ble_evt);
static void sys_evt_dispatch(uint32_t sys_evt);


static uint32_t core_ble_init (void * p_context, 
									ble_rx_callback_t p_ble_rx_callback);
static uint32_t core_ble_deinit (void * p_context);
static uint32_t core_ble_write (void * p_context);
static uint32_t core_ble_read (void * p_context);
static uint32_t core_ble_pair (void * p_context);
static uint32_t core_ble_erase_bonds (void * p_context);
static uint32_t core_ble_execute (void * p_context);
static uint32_t core_ble_connect (void * p_context);
static uint32_t core_ble_disconnect (void * p_context);
static uint32_t core_ble_forbid_scaning (void * p_context);
static uint32_t core_ble_permit_scaning (void * p_context);
static uint32_t core_ble_set_dev_filter (void * p_context, 
										 uint32_t * filter_id);
/*Private variables*/
static ble_nus_c_t              m_ble_nus_c;
static ma_ble_nus_c_t           m_ma_ble_nus_c;

static ble_db_discovery_t       m_ble_db_discovery;

static ble_rx_callback_t 		p_rx_callback = NULL;

static ble_gap_scan_params_t m_scan_param; 	/**< Scan parameters requested for
											  scanning and connection. */
static ble_gap_evt_adv_report_t	last_adv_report = {0};
static uint8_t 				b_rx_buffer[UART_TX_BUF_SIZE] = {0};
static ble_rx_t 			b_ble_rx;

static ble_sys_t 			b_ble_sys = 
{
	.m_scan_mode 		= BLE_NO_SCAN,
	.m_conn_handle		= BLE_CONN_HANDLE_INVALID,
	.m_active_dev_type	= 0,
	.m_peer_count		= 0,
	.m_in_scan 			= false,
	.m_memory_access_in_progress 		= false,
	.m_delete_bond_in_progress			= false,
	.m_whitelist_temporarily_disabled	= false,
	.m_scan_permition	= false,
};

static ble_adv_data_parser_t ble_adv_data_parser[SUP_DEV_COUNT] = {0};


/*Private constants*/
static const ble_gap_conn_params_t m_connection_param = {
	(uint16_t)MIN_CONNECTION_INTERVAL,  // Minimum connection
	(uint16_t)MAX_CONNECTION_INTERVAL,  // Maximum connection
	0,									// Slave latency
	(uint16_t)SUPERVISION_TIMEOUT		// Supervision time-out
};

static const ble_gap_scan_params_t m_scan_params = {
	.active		 	= SCAN_ACTIVE,
	.selective   	= SCAN_SELECTIVE,
	.p_whitelist 	= NULL,
	.interval		= SCAN_INTERVAL,
	.window		 	= SCAN_WINDOW,
	.timeout	 	= SCAN_TIMEOUT
};

static const ble_uuid_t m_nus_uuid = {.uuid = BLE_UUID_NUS_SERVICE,
									  .type = NUS_SERVICE_UUID_TYPE
};

static const ble_uuid_t m_ma_nus_uuid = {.uuid = BLE_UUID_MA_NUS_SERVICE,
									  .type = MA_NUS_SERVICE_UUID_TYPE
};

const ble_m_list_t ble_m_list = {
	.init 				= &core_ble_init,
	.deinit 			= &core_ble_deinit,
	.write 				= &core_ble_write,
	.read 				= &core_ble_read,
	.sys.pair 			= &core_ble_pair,
	.sys.erase_bonds 	= &core_ble_erase_bonds,
	.sys.execute		= &core_ble_execute,
	.sys.connect		= &core_ble_connect,
	.sys.disconnect		= &core_ble_disconnect,
	.sys.scan_permit	= &core_ble_permit_scaning,
	.sys.scan_forbid	= &core_ble_forbid_scaning,
	.sys.dev_filter		= &core_ble_set_dev_filter,
};

/*Private functions*/
/*______________misc_functions___________________________________*/
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
	app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

uint32_t ble_send_array(uint8_t *array_pointer, uint16_t size)
{
	uint32_t err_code;
	uint8_t b_size = size;
	
	do
	{
		if(b_ble_sys.m_active_dev_type != NULL)
		{
			switch (b_ble_sys.m_active_dev_type)
			{
				case 1:
					err_code = ma_ble_nus_c_string_send(&m_ma_ble_nus_c, 
														array_pointer, b_size);
					break;
				case 2:
					err_code = ble_nus_c_string_send(&m_ble_nus_c, 
													 array_pointer, b_size);
					break;
				case 3:
					err_code = ma_ble_nus_c_string_send(&m_ma_ble_nus_c, 
														array_pointer, b_size);
					break;
			}
		}
		if (err_code != NRF_ERROR_INVALID_STATE && err_code != NRF_ERROR_BUSY &&
			err_code != NRF_ERROR_RESOURCES)
			APP_ERROR_CHECK(err_code);
	} while (err_code == NRF_ERROR_BUSY || err_code == NRF_ERROR_RESOURCES);
	return err_code;
}

static void peer_list_get(pm_peer_id_t * p_peers, uint32_t * p_size)
{
    pm_peer_id_t peer_id;
    uint32_t     peers_to_copy;

    peers_to_copy = (*p_size < BLE_GAP_WHITELIST_ADDR_MAX_COUNT) ?
                     *p_size : BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

    peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    *p_size = 0;

    while ((peer_id != PM_PEER_ID_INVALID) && (peers_to_copy--))
    {
        p_peers[(*p_size)++] = peer_id;
        peer_id = pm_next_peer_id_get(peer_id);
    }
}

static void whitelist_load(void)
{
    ret_code_t   ret;
    static pm_peer_id_t peers[8];
    uint32_t     peer_cnt;

    memset(peers, PM_PEER_ID_INVALID, sizeof(peers));
    peer_cnt = (sizeof(peers) / sizeof(pm_peer_id_t));
	
    // Load all peers from flash and whitelist them.
    peer_list_get(peers, &peer_cnt);

    ret = pm_whitelist_set(peers, peer_cnt);
    APP_ERROR_CHECK(ret);

    // Setup the device identies list.
    // Some SoftDevices do not support this feature.
    ret = pm_device_identities_list_set(peers, peer_cnt);
    if (ret != NRF_ERROR_NOT_SUPPORTED)
    {
        APP_ERROR_CHECK(ret);
    }
}

bool scan_start(bool* is_whitelist)
{
	if (!b_ble_sys.m_scan_permition)
		return false;
	if (b_ble_sys.m_in_scan)
	{
		NRF_LOG_INFO("Already in scanning!\r\n");
		if(b_ble_sys.m_scan_mode == BLE_WHITELIST_SCAN)
		{
			if (is_whitelist != NULL)
				*is_whitelist = false;
		}
		else
		{
			if (is_whitelist != NULL)
				*is_whitelist = true;			
		}
		return false;
	}

	ret_code_t ret;
	
    // Whitelist buffers.
    ble_gap_addr_t whitelist_addrs[8];
    ble_gap_irk_t  whitelist_irks[8];

    memset(whitelist_addrs, 0x00, sizeof(whitelist_addrs));
    memset(whitelist_irks,  0x00, sizeof(whitelist_irks));

    uint32_t addr_cnt = (sizeof(whitelist_addrs) / sizeof(ble_gap_addr_t));
    uint32_t irk_cnt  = (sizeof(whitelist_irks)  / sizeof(ble_gap_irk_t));

#if (NRF_SD_BLE_API_VERSION <= 2)

    ble_gap_addr_t * p_whitelist_addrs[8];
    ble_gap_irk_t  * p_whitelist_irks[8];

    for (uint32_t i = 0; i < 8; i++)
    {
		p_whitelist_addrs[i] = &whitelist_addrs[i];
		p_whitelist_irks[i]  = &whitelist_irks[i];
	}

	ble_gap_whitelist_t whitelist =
	{
		.pp_addrs = p_whitelist_addrs,
		.pp_irks  = p_whitelist_irks,
	};
#endif
    // Reload the whitelist and whitelist all peers.
    whitelist_load();
    // Get the whitelist previously set using pm_whitelist_set().
    ret = pm_whitelist_get(whitelist_addrs, &addr_cnt,
                           whitelist_irks,  &irk_cnt);
	APP_ERROR_CHECK(ret);
    m_scan_param.active   = 1;
    m_scan_param.interval = SCAN_INTERVAL;
    m_scan_param.window   = SCAN_WINDOW;

	if(((addr_cnt != 0) || (irk_cnt != 0)))
	{
		b_ble_sys.m_scan_mode = BLE_WHITELIST_SCAN;
	}
	
    if (((addr_cnt == 0) && (irk_cnt == 0)))
    {
		b_ble_sys.m_scan_mode = BLE_FAST_SCAN;
        // Don't use whitelist.
        #if (NRF_SD_BLE_API_VERSION <= 2)
            m_scan_param.selective   = 0;
            m_scan_param.p_whitelist = NULL;
        #endif
        #if (NRF_SD_BLE_API_VERSION >= 3)
            m_scan_param.use_whitelist  = 0;
            m_scan_param.adv_dir_report = 0;
        #endif
        m_scan_param.timeout  = 0x0000; // No timeout.
		if (is_whitelist != NULL)
			*is_whitelist = true;
    }
    else
    {
        // Use whitelist.
#if (NRF_SD_BLE_API_VERSION <= 2)
		whitelist.addr_count     = addr_cnt;
		whitelist.irk_count      = irk_cnt;
		m_scan_param.selective   = 1;
		m_scan_param.p_whitelist = &whitelist;
#endif
#if (NRF_SD_BLE_API_VERSION >= 3)
		m_scan_param.use_whitelist  = 1;
		m_scan_param.adv_dir_report = 0;
#endif
		m_scan_param.timeout  = 0x0000;
		if (is_whitelist != NULL)
			*is_whitelist = false;
    }
    ret = sd_ble_gap_scan_start(&m_scan_param);
	if(ret == NRF_SUCCESS)
	{
		NRF_LOG_INFO("Start scanning...\r\n");
	}
	else
		APP_ERROR_CHECK(ret);

	b_ble_sys.m_in_scan = true;
	return true;
}

static void erase_bonds(void)
{
	uint32_t err_code;
	
    static pm_peer_id_t peers[8];
    uint32_t     peer_cnt;

    memset(peers, PM_PEER_ID_INVALID, sizeof(peers));
    peer_cnt = (sizeof(peers) / sizeof(pm_peer_id_t));
	
    // Load all peers from flash and whitelist them.
    peer_list_get(peers, &peer_cnt);
	
	for (uint8_t cnt = 0; cnt < 8; cnt++)
	{
		if (peers[cnt] != PM_PEER_ID_INVALID)
		{
			err_code = pm_peer_delete(peers[cnt]);
			APP_ERROR_CHECK(err_code);
			err_code = fds_gc();
			if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
			{
				// Retry.
				while(FDS_SUCCESS != fds_gc());
			}
			else
			{
				APP_ERROR_CHECK(err_code);
			}
			return;
		}
	}
}

static void whitelist_disable(void)
{
	uint32_t err_code;

	if ((b_ble_sys.m_scan_mode == BLE_WHITELIST_SCAN) &&
		!b_ble_sys.m_whitelist_temporarily_disabled)
	{
		b_ble_sys.m_whitelist_temporarily_disabled = true;

		err_code = sd_ble_gap_scan_stop();
		if (err_code == NRF_SUCCESS)
		{
			scan_start(NULL);
		}
		else if (err_code != NRF_ERROR_INVALID_STATE)
		{
			APP_ERROR_CHECK(err_code);
		}
	}
	b_ble_sys.m_whitelist_temporarily_disabled = true;
}

/*______________adv_package_parsers___________________________________*/
/*static uint32_t handles_assign(uint8_t found_dev_id, 
							   const ble_gap_evt_t * p_gap_evt)
{
	uint32_t err_code;
	
	switch(found_dev_id)
	{
		case BRLT_DEV:
		{
			err_code = ma_ble_nus_c_handles_assign(&m_ma_ble_nus_c,
													p_gap_evt->conn_handle, NULL);
			APP_ERROR_CHECK(err_code);
		}	break;
		
		case BR3S_DEV:
		{
			err_code = ble_nus_c_handles_assign(&m_ble_nus_c,
													p_gap_evt->conn_handle, NULL);
			APP_ERROR_CHECK(err_code);
		}	break;

		case BR4S_DEV:
		{
			err_code = ma_ble_nus_c_handles_assign(&m_ma_ble_nus_c,
													p_gap_evt->conn_handle, NULL);
			APP_ERROR_CHECK(err_code);
		}	break;
		
		default:
			break;
	}
	
	return err_code;
}*/
static bool is_uuid_present(const ble_uuid_t *p_target_uuid,
							const ble_gap_evt_adv_report_t *p_adv_report)
{
	uint32_t err_code;
	uint32_t index  = 0;
	uint8_t *p_data = (uint8_t *)p_adv_report->data;
	ble_uuid_t extracted_uuid;

	while (index < p_adv_report->dlen)
	{
		uint8_t field_length = p_data[index];
		uint8_t field_type   = p_data[index + 1];

		if ((field_type ==
			 BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE) ||
			(field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE))
		{
			for (uint32_t u_index = 0; u_index < (field_length / UUID16_SIZE);
				 u_index++)
			{
				err_code = sd_ble_uuid_decode(
					UUID16_SIZE, &p_data[u_index * UUID16_SIZE + index + 2],
					&extracted_uuid);
				if (err_code == NRF_SUCCESS)
				{
					if ((extracted_uuid.uuid == p_target_uuid->uuid) &&
						(extracted_uuid.type == p_target_uuid->type))
					{
						return true;
					}
				}
			}
		}
		else if ((field_type ==
				  BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE) ||
				 (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE))
		{
			for (uint32_t u_index = 0; u_index < (field_length / UUID32_SIZE);
				 u_index++)
			{
				err_code = sd_ble_uuid_decode(
					UUID16_SIZE, &p_data[u_index * UUID32_SIZE + index + 2],
					&extracted_uuid);
				if (err_code == NRF_SUCCESS)
				{
					if ((extracted_uuid.uuid == p_target_uuid->uuid) &&
						(extracted_uuid.type == p_target_uuid->type))
					{
						return true;
					}
				}
			}
		}
		else if ((field_type ==
				  BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE) ||
				 (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE))
		{
			err_code = sd_ble_uuid_decode(UUID128_SIZE, &p_data[index + 2],
										  &extracted_uuid);
			if (err_code == NRF_SUCCESS)
			{
				if ((extracted_uuid.uuid == p_target_uuid->uuid) &&
					(extracted_uuid.type == p_target_uuid->type))
				{
					return true;
				}
			}
		}
		index += field_length + 1;
	}
	return false;
}
/*______________evt_handlers___________________________________*/
static void ble_nus_c_evt_handler(ble_nus_c_t *p_ble_nus_c,
								  const ble_nus_c_evt_t *p_ble_nus_evt)
{
	uint32_t err_code;
	
	switch (p_ble_nus_evt->evt_type)
	{
		case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
		{
			b_ble_sys.m_in_scan = false;
			b_ble_sys.m_scan_mode = BLE_FAST_SCAN;
			err_code = ble_nus_c_handles_assign(p_ble_nus_c,
												p_ble_nus_evt->conn_handle,
												&p_ble_nus_evt->handles);
			APP_ERROR_CHECK(err_code);
			
			pm_conn_sec_status_t b_local_conn_sec_status;
			err_code = pm_conn_sec_status_get(p_ble_nus_c->conn_handle, 
											  &b_local_conn_sec_status);
			
			err_code = pm_conn_secure(p_ble_nus_c->conn_handle, false);
			if (err_code != NRF_ERROR_INVALID_STATE)
			{
				if(err_code == NRF_ERROR_STORAGE_FULL)
				{
					pm_evt_t pm_evt;
					pm_evt.evt_id = PM_EVT_STORAGE_FULL;
					pm_evt.conn_handle = p_ble_nus_c->conn_handle;
					pm_evt_handler((pm_evt_t const *)&pm_evt.evt_id);
				}
				APP_ERROR_CHECK(err_code);
			}
			err_code = ble_nus_c_rx_notif_enable(p_ble_nus_c);
			APP_ERROR_CHECK(err_code);
			
			if(p_rx_callback != NULL)
			{
				b_ble_rx.evt_type = CONNECTION;
				b_ble_rx.p_rx_buff = b_rx_buffer;
				b_ble_rx.lenght = p_ble_nus_evt->data_len;
				b_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
				err_code = p_rx_callback(&b_ble_rx);
			}
			else
			{
				//NO HANDLER
			}
			
			
		}	break;

		case BLE_NUS_C_EVT_NUS_RX_EVT:
		{
			memcpy(b_rx_buffer, p_ble_nus_evt->p_data, 
				   p_ble_nus_evt->data_len);
			if(p_rx_callback != NULL)
			{
				b_ble_rx.evt_type = RX_DATA;
				b_ble_rx.p_rx_buff = b_rx_buffer;
				b_ble_rx.lenght = p_ble_nus_evt->data_len;
				b_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
				err_code = p_rx_callback(&b_ble_rx);
			}
			else
			{
				//NO HANDLER
			}
		}	break;

		case BLE_NUS_C_EVT_DISCONNECTED:
		{
			ble_rx_t b_local_ble_rx = {0};
			b_local_ble_rx.evt_type = DISCONNECTION;
			b_local_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
			p_rx_callback(&b_local_ble_rx);
			b_ble_sys.m_active_dev_type = DEFAULT_DEV;
		}	break;
		default:
			break;
	}
}

static void ma_ble_nus_c_evt_handler(ma_ble_nus_c_t * p_ma_ble_nus_c,
								  const ma_ble_nus_c_evt_t * p_ma_ble_nus_evt)
{
	uint32_t err_code;
	
	switch (p_ma_ble_nus_evt->evt_type)
	{
		case BLE_MA_NUS_C_EVT_DISCOVERY_COMPLETE:
		{
			b_ble_sys.m_in_scan = false;
			b_ble_sys.m_scan_mode = BLE_FAST_SCAN;
			err_code = ma_ble_nus_c_handles_assign(p_ma_ble_nus_c,
												p_ma_ble_nus_evt->conn_handle,
												&p_ma_ble_nus_evt->handles);
			APP_ERROR_CHECK(err_code);
			
			pm_conn_sec_status_t b_local_conn_sec_status;
			err_code = pm_conn_sec_status_get(p_ma_ble_nus_c->conn_handle, 
											  &b_local_conn_sec_status);
			
			err_code = pm_conn_secure(p_ma_ble_nus_c->conn_handle, false);
			if (err_code != NRF_ERROR_INVALID_STATE)
			{
				if(err_code == NRF_ERROR_STORAGE_FULL)
				{
					pm_evt_t pm_evt;
					pm_evt.evt_id = PM_EVT_STORAGE_FULL;
					pm_evt.conn_handle = p_ma_ble_nus_c->conn_handle;
					pm_evt_handler((pm_evt_t const *)&pm_evt);
				}
				APP_ERROR_CHECK(err_code);
			}
			
			err_code = ma_ble_nus_c_rx_notif_enable(p_ma_ble_nus_c);
			APP_ERROR_CHECK(err_code);
			
			if(p_rx_callback != NULL)
			{
				b_ble_rx.evt_type = CONNECTION;
				b_ble_rx.p_rx_buff = b_rx_buffer;
				b_ble_rx.lenght = p_ma_ble_nus_evt->data_len;
				b_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
				err_code = p_rx_callback(&b_ble_rx);
			}
			else
			{
				//NO HANDLER
			}
			
		}	break;

		case BLE_MA_NUS_C_EVT_NUS_RX_EVT:
		{
			memcpy(b_rx_buffer, p_ma_ble_nus_evt->p_data, 
				   p_ma_ble_nus_evt->data_len);
			if(p_rx_callback != NULL)
			{
				b_ble_rx.evt_type = RX_DATA;
				b_ble_rx.p_rx_buff = b_rx_buffer;
				b_ble_rx.lenght = p_ma_ble_nus_evt->data_len;
				b_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
				err_code = p_rx_callback(&b_ble_rx);
			}
			else
			{
				//NO HANDLER
			}
		}	break;

		case BLE_MA_NUS_C_EVT_DISCONNECTED:
		{
			ble_rx_t b_local_ble_rx = {0};
			b_local_ble_rx.evt_type = DISCONNECTION;
			b_local_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
			p_rx_callback(&b_local_ble_rx);
			b_ble_sys.m_active_dev_type = DEFAULT_DEV;
		}	break;
		default:
			break;
	}
}

static void db_disc_handler(ble_db_discovery_evt_t *p_evt)
{
	switch (b_ble_sys.m_active_dev_type)
	{
		case BRLT_DEV:
			ma_ble_nus_c_on_db_disc_evt(&m_ma_ble_nus_c, p_evt);
			break;
		case BR3S_DEV:
			ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
			break;
		case BR4S_DEV:
			ma_ble_nus_c_on_db_disc_evt(&m_ma_ble_nus_c, p_evt);
			break;
		default:
			break;
	}	
}

static void pm_evt_handler(pm_evt_t const * p_evt)
{
	uint32_t err_code;

    switch(p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
		{
			NRF_LOG_INFO("Connected to a previously bonded device.\r\n");
			pm_conn_sec_status_t b_local_conn_sec_status;
			err_code = pm_conn_sec_status_get(p_evt->conn_handle, 
											  &b_local_conn_sec_status);
			APP_ERROR_CHECK(err_code);
		}	break;

        case PM_EVT_CONN_SEC_START:
		{
			pm_conn_sec_status_t b_local_conn_sec_status;
			err_code = pm_conn_sec_status_get(p_evt->conn_handle, 
											  &b_local_conn_sec_status);
			APP_ERROR_CHECK(err_code);
		}	break;
        case PM_EVT_CONN_SEC_SUCCEEDED:
		{
			NRF_LOG_INFO("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.\r\n",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);
		}	break;
        case PM_EVT_CONN_SEC_FAILED:
		{
			pm_conn_sec_status_t b_local_conn_sec_status;
			err_code = pm_conn_sec_status_get(p_evt->conn_handle, 
											  &b_local_conn_sec_status);
			APP_ERROR_CHECK(err_code);
			
			err_code = sd_ble_gap_disconnect(p_evt->conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
			
			/*err_code = pm_conn_secure(p_evt->conn_handle, true);
			APP_ERROR_CHECK(err_code);*/
		}	break;
        case PM_EVT_CONN_SEC_CONFIG_REQ:
		{
			// Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
		}	break;
        case PM_EVT_STORAGE_FULL:
		{
			err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
				while(FDS_SUCCESS != fds_gc());
				// Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
			err_code = pm_conn_secure(p_evt->conn_handle, false);
		}	break;
		case PM_EVT_PEER_DELETE_SUCCEEDED:	
		case PM_EVT_PEERS_DELETE_SUCCEEDED:
		{	
			if(p_rx_callback != NULL)
			{
				b_ble_rx.evt_type = CLEAR_W_LIST;
				b_ble_rx.p_rx_buff = NULL;
				b_ble_rx.lenght = NULL;
				b_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
				err_code = p_rx_callback(&b_ble_rx);
			}
			else
			{
				//NO HANDLER
			}
			memset(&m_ble_db_discovery, 0x0, sizeof(m_ble_db_discovery));
			b_ble_sys.m_active_dev_type = 0;
			b_ble_sys.m_scan_mode = BLE_NO_SCAN;
		}	break;
		case PM_EVT_PEER_DELETE_FAILED:
		case PM_EVT_PEERS_DELETE_FAILED:
		{
			if(p_rx_callback != NULL)
			{
				b_ble_rx.evt_type = CLEAR_W_LIST_FAIL;
				b_ble_rx.p_rx_buff = NULL;
				b_ble_rx.lenght = NULL;
				b_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
				err_code = p_rx_callback(&b_ble_rx);
			}
			else
			{
				//NO HANDLER
			}
		}	break;
		
        case PM_EVT_ERROR_UNEXPECTED:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}

static void on_ble_evt(ble_evt_t *p_ble_evt)
{
	uint32_t err_code;
	const ble_gap_evt_t * p_gap_evt = &p_ble_evt->evt.gap_evt;
	static ble_gap_evt_t b_gap_evt;
	memcpy(&b_gap_evt, p_gap_evt, sizeof(ble_gap_evt_t));
	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
		{
			if(b_ble_sys.m_conn_handle == BLE_CONN_HANDLE_INVALID)
			{
				b_ble_sys.m_conn_handle = p_gap_evt->conn_handle;
				memset(&m_ble_db_discovery,0,sizeof(m_ble_db_discovery)); 
				err_code = ble_db_discovery_start(&m_ble_db_discovery,
												  p_gap_evt->conn_handle);
				APP_ERROR_CHECK(err_code);
			}
		}	break;

		case BLE_GAP_EVT_DISCONNECTED:
        {
			/*if (p_gap_evt->conn_handle == b_ble_sys.m_conn_handle)
			{*/
				b_ble_sys.m_conn_handle = BLE_CONN_HANDLE_INVALID;
				b_ble_sys.m_active_dev_type = DEFAULT_DEV;
				memset(&last_adv_report, 0, sizeof(ble_gap_evt_adv_report_t));
			//}
			
			if (b_ble_sys.m_conn_handle == BLE_CONN_HANDLE_INVALID)
				scan_start(NULL);
		}	break;

		case BLE_GAP_EVT_ADV_REPORT:
		{
			bool found_supported_dev = false;
			if (!b_ble_sys.m_active_dev_type)
			{
				for (uint8_t cnt = 0; cnt < SUP_DEV_COUNT; cnt++)
				{
					if(ble_adv_data_parser[cnt] != NULL)
					{
						found_supported_dev = (bool)ble_adv_data_parser[cnt](
									(uint32_t)(!m_scan_param.selective)/*(b_ble_sys.m_scan_mode == BLE_FAST_SCAN)*/,
									&b_gap_evt.params.adv_report);
					}
					if(found_supported_dev)
					{
						ble_rx_t b_local_ble_rx = {0};
						b_local_ble_rx.evt_type = FOUND_CONNECTABLE_SUPPORTED_DEV;
						b_local_ble_rx.dev_type = cnt;
						p_rx_callback(&b_local_ble_rx);
						b_ble_sys.m_active_dev_type = cnt;
						cnt = SUP_DEV_COUNT;
					}
				}
			}
			else
			{
				found_supported_dev = (bool)ble_adv_data_parser[b_ble_sys.m_active_dev_type](
							(uint32_t)(!m_scan_param.selective)/*(b_ble_sys.m_scan_mode == BLE_FAST_SCAN)*/,
							&b_gap_evt.params.adv_report);
				
				if(found_supported_dev)
				{
					ble_rx_t b_local_ble_rx = {0};
					b_local_ble_rx.evt_type = FOUND_CONNECTABLE_SUPPORTED_DEV;
					b_local_ble_rx.dev_type = b_ble_sys.m_active_dev_type;
					p_rx_callback(&b_local_ble_rx);
				}
			}

			if(found_supported_dev)
			{
				m_scan_param.selective   = 0;
				m_scan_param.p_whitelist = NULL;
				err_code =
					sd_ble_gap_connect(&p_gap_evt->params.adv_report.peer_addr,
									   &m_scan_param,
									   &m_connection_param);
				memcpy(&last_adv_report, &p_gap_evt->params.adv_report, 
					   sizeof(ble_gap_evt_adv_report_t));
				
				if (err_code == NRF_SUCCESS)
				{
					b_ble_sys.m_whitelist_temporarily_disabled = false;
					b_ble_sys.m_in_scan = false;
					// scan is automatically stopped by the connect
				}
			}
		}	break;

		case BLE_GAP_EVT_TIMEOUT:
			if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_SCAN)
			{
				NRF_LOG_DEBUG("[APPL]: Scan timed out.\r\n");
				scan_start(NULL);
			}
			else if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
			{
				NRF_LOG_DEBUG("[APPL]: Connection Request timed out.\r\n");
			}
			break;

		case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
			err_code = sd_ble_gap_conn_param_update(
					p_gap_evt->conn_handle,
					&p_gap_evt->params.conn_param_update_request.conn_params);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GATTC_EVT_TIMEOUT:
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT
#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif
		default:
			break;
	}
}

static void on_sys_evt(uint32_t sys_evt)
{
	switch (sys_evt)
	{
		case NRF_EVT_FLASH_OPERATION_SUCCESS:
		/* fall through */
		case NRF_EVT_FLASH_OPERATION_ERROR:
			if (b_ble_sys.m_memory_access_in_progress)
			{
				b_ble_sys.m_memory_access_in_progress = false;
				scan_start(NULL);
			}
			break;

		default:
			// No implementation needed.
			break;
	}
}

static void fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
    if (p_fds_evt->id == FDS_EVT_GC){}
}

static void ble_evt_dispatch(ble_evt_t *p_ble_evt)
{
	ble_conn_state_on_ble_evt(p_ble_evt);
	pm_on_ble_evt(p_ble_evt);
	on_ble_evt(p_ble_evt);
	ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
	ble_nus_c_on_ble_evt(&m_ble_nus_c, p_ble_evt);
	ma_ble_nus_c_on_ble_evt(&m_ma_ble_nus_c, p_ble_evt);
}

static void sys_evt_dispatch(uint32_t sys_evt)
{
	fs_sys_event_handler(sys_evt);
	on_sys_evt(sys_evt);
}

static void ble_stack_init(void)
{
	uint32_t err_code;

	nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

	// Initialize the SoftDevice handler module.
	SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

	ble_enable_params_t ble_enable_params;
	
	err_code = softdevice_enable_get_default_config(
		CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT, &ble_enable_params);
	APP_ERROR_CHECK(err_code);

	ble_enable_params.common_enable_params.vs_uuid_count = 10;
	// Check the ram settings against the used number of links
	CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);
	
	
    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
	err_code = softdevice_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for System events.
	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);
}

static void db_discovery_init(void)
{
	uint32_t err_code = ble_db_discovery_init(db_disc_handler);
	APP_ERROR_CHECK(err_code);
}

static void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond              = SEC_PARAM_BOND;
    sec_param.mitm              = SEC_PARAM_MITM;
    sec_param.lesc              = SEC_PARAM_LESC;
    sec_param.keypress          = SEC_PARAM_KEYPRESS;
    sec_param.io_caps           = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob               = SEC_PARAM_OOB;
    sec_param.min_key_size      = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size      = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc     = 1;
    sec_param.kdist_own.id      = 1;
    sec_param.kdist_peer.enc    = 1;
    sec_param.kdist_peer.id     = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);

	err_code = fds_register(fds_evt_handler);
    APP_ERROR_CHECK(err_code);
}

static void nus_c_init(void)
{
	uint32_t err_code;
	ble_nus_c_init_t nus_c_init_t;

	nus_c_init_t.evt_handler = ble_nus_c_evt_handler;

	err_code = ble_nus_c_init(&m_ble_nus_c, &nus_c_init_t);
	APP_ERROR_CHECK(err_code);
}

static void ma_nus_c_init(void)
{
	uint32_t err_code;
	ma_ble_nus_c_init_t ma_nus_c_init_t;

	ma_nus_c_init_t.evt_handler = ma_ble_nus_c_evt_handler;

	err_code = ma_ble_nus_c_init(&m_ma_ble_nus_c, &ma_nus_c_init_t);
	APP_ERROR_CHECK(err_code);
}

static uint32_t power_manage(void)
{
	uint32_t err_code = sd_app_evt_wait();

	APP_ERROR_CHECK(err_code);
	return err_code;
}

static uint32_t core_ble_init (void * p_context, 
									ble_rx_callback_t p_ble_rx_callback)
{
	uint32_t ret_val = 0;
	bool erase_bonds = 0;
	
	brc_main_t * p_brc_main;

	p_brc_main = (brc_main_t *)p_context;

	ble_stack_init();
	peer_manager_init(erase_bonds);
	db_discovery_init();
	ma_nus_c_init();
	nus_c_init();
	p_rx_callback = p_ble_rx_callback;

	for(uint8_t cnt = 0; cnt < SUP_DEV_COUNT; cnt++)
	{
		ble_adv_data_parser[cnt] = p_brc_main->dev_list[cnt].p_adv_data_parser;
	}
	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_deinit (void * p_context)
{
	uint32_t ret_val = 0;

	p_rx_callback = NULL; 
	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_write (void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	data_t * p_local_data = NULL;
	p_local_data = (data_t *)p_context;

	ret_val = ble_send_array(p_local_data->p_data, p_local_data->data_len);
	
	return ret_val;
}

static uint32_t core_ble_read (void * p_context)
{
	uint32_t ret_val = 0;
	ble_read_out_t * p_ble_read_out;
	
	p_ble_read_out = (ble_read_out_t *)p_context;
	switch (p_ble_read_out->read_type)
	{
		case SYS_READ:
			p_ble_read_out->p_ble_sys = &b_ble_sys;
			break;
			
		case LAST_RX_READ:
			p_ble_read_out->p_ble_rx = &b_ble_rx;
			break;
	}
	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_pair (void * p_context)
{
	uint32_t ret_val = 0;
	bool is_wl = false;
	scan_start(&is_wl);
	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_erase_bonds (void * p_context)
{
	uint32_t ret_val = 0;
	
	erase_bonds();
	
	b_ble_sys.m_scan_mode 		= BLE_NO_SCAN;
	b_ble_sys.m_active_dev_type	= 0,
	b_ble_sys.m_in_scan 		= false;
	b_ble_sys.m_scan_permition	= false;
	
	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_execute (void * p_context)
{
	uint32_t ret_val = power_manage();
	return ret_val;
}

static uint32_t core_ble_connect (void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	bool * p_is_wl = (bool *)p_context;
	if(scan_start(p_is_wl))
	{
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_BUSY;
	}
	
	return ret_val;
}
 
static uint32_t core_ble_disconnect (void * p_context)
{
	uint32_t ret_val = 0;
	
	if(b_ble_sys.m_conn_handle != BLE_CONN_HANDLE_INVALID)
	{
		sd_ble_gap_disconnect(b_ble_sys.m_conn_handle,
							BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
	}
	else
	{
		/*stop scan*/
	}
	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_forbid_scaning (void * p_context)
{
	uint32_t ret_val = 0;
	
	b_ble_sys.m_scan_permition = false;
	
	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_permit_scaning (void * p_context)
{
	uint32_t ret_val = 0;
	
	b_ble_sys.m_scan_permition = true;

	ret_val = NRF_SUCCESS;
	return ret_val;
}

static uint32_t core_ble_set_dev_filter (void * p_context, 
										 uint32_t * filter_id)
{
	uint32_t ret_val = 0;
	if (*filter_id != NO_VALUE)
	{
		b_ble_sys.m_active_dev_type = *filter_id;
	}
	ret_val = NRF_SUCCESS;
	return ret_val;
}
/*Public functions*/

/******************************* END_OF_FILE **********************************/