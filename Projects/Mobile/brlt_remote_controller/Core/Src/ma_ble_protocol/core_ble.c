#include "core_ble.h"
#include "ma_ble_nus_c.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "softdevice_handler.h"
#include "nrf_ble_gatt.h"
#include "ble_advdata.h"
#include "ble_db_discovery.h"
#include "fstorage.h"
#include "fds.h"
#include "peer_manager.h"
#include "ble_conn_state.h"

#define NRF_CLOCK_LFCLKSRC  {.source        = NRF_CLOCK_LF_SRC_XTAL,          \
                             .rc_ctiv       = 0,                              \
                             .rc_temp_ctiv  = 0,                              \
                             .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

#define CENTRAL_LINK_COUNT      	1                                       /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT   	0                                       /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/
#define CONN_CFG_TAG            	1                                       /**< A tag that refers to the BLE stack configuration we set with @ref sd_ble_cfg_set. Default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */

#define SEC_PARAM_BOND              1                                   /**< Perform bonding. */
#define SEC_PARAM_MITM              0                                   /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC              0                                   /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS          0                                   /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES   BLE_GAP_IO_CAPS_NONE                /**< No I/O capabilities. */
#define SEC_PARAM_OOB               0                                   /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE      7                                   /**< Minimum encryption key size in octets. */
#define SEC_PARAM_MAX_KEY_SIZE      16                                  /**< Maximum encryption key size in octets. */

#define NUS_SERVICE_UUID_TYPE		BLE_UUID_TYPE_VENDOR_BEGIN              /**< UUID type for the Nordic UART Service (vendor specific). */

#define SCAN_INTERVAL				0x00A0                                  /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW					0x0050                                  /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_TIMEOUT	            0x0000                                  /**< Timout when scanning. 0x0000 disables timeout. */

#define MIN_CONNECTION_INTERVAL 	MSEC_TO_UNITS(20, UNIT_1_25_MS)         /**< Determines minimum connection interval in millisecond. */
#define MAX_CONNECTION_INTERVAL 	MSEC_TO_UNITS(30, UNIT_1_25_MS)         /**< Determines maximum connection interval in millisecond. */
#define SLAVE_LATENCY           	0                                       /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT     	MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Determines supervision time-out in units of 10 millisecond. */

#define UUID16_SIZE             	2                                       /**< Size of 16 bit UUID */
#define UUID32_SIZE             	4                                       /**< Size of 32 bit UUID */
#define UUID128_SIZE            	16                                      /**< Size of 128 bit UUID */

static ble_nus_c_t			m_ble_nus_c;                            /**< Instance of NUS service. Must be passed to all NUS_C API calls. */
static nrf_ble_gatt_t		m_gatt;                                 /**< GATT module instance. */
static ble_db_discovery_t	m_ble_db_discovery;                     /**< Instance of database discovery module. Must be passed to all db_discovert API calls */
static uint16_t				m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH; /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
/** @brief Parameters used when scanning. */
static ble_gap_scan_params_t m_scan_param; 
static bool m_memory_access_in_progress;  /**< Flag to keep track of ongoing operations on persistent memory. */

static core_ble_event_callback_t m_evt_callback;
static core_ble_data_callback_t m_data_callback;
static core_ble_adv_filter_t m_adv_filter;
static bool m_retry_db_disc = false;
static bool m_in_scan = false;

/**@brief Connection parameters requested for connection. */
static ble_gap_conn_params_t const m_connection_param =
  {
    (uint16_t)MIN_CONNECTION_INTERVAL,  // Minimum connection
    (uint16_t)MAX_CONNECTION_INTERVAL,  // Maximum connection
    (uint16_t)SLAVE_LATENCY,            // Slave latency
    (uint16_t)SUPERVISION_TIMEOUT       // Supervision time-out
  };



/**@brief NUS uuid. */
static ble_uuid_t const m_nus_uuid =
{
    .uuid = BLE_UUID_NUS_SERVICE,
    .type = NUS_SERVICE_UUID_TYPE
};


/**@brief Retrieve a list of peer manager peer IDs.
 *
 * @param[inout] p_peers   The buffer where to store the list of peer IDs.
 * @param[inout] p_size    In: The size of the @p p_peers buffer.
 *                         Out: The number of peers copied in the buffer.
 */
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

static void whitelist_load()
{
    ret_code_t   ret;
    pm_peer_id_t peers[8];
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


/**@brief Function to start scanning. */
bool core_ble_scan_start(bool* is_whitelist)
{
	if (m_in_scan)
	{
	  NRF_LOG_INFO("Already in scanning!\r\n");
	  return false;
	}
	
	ret_code_t ret;
    uint32_t flash_busy;

    // If there is any pending write to flash, defer scanning until it completes.
    (void) fs_queued_op_count_get(&flash_busy);

    if (flash_busy != 0)
    {
        m_memory_access_in_progress = true;
        return false;
    }

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

    m_scan_param.active   = 1;
    m_scan_param.interval = SCAN_INTERVAL;
    m_scan_param.window   = SCAN_WINDOW;

    if (((addr_cnt == 0) && (irk_cnt == 0)))
    {
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
	
	m_in_scan = true;
	return true;
}

/**@brief Reads an advertising report and checks if a UUID is present in the service list.
 *
 * @details The function is able to search for 16-bit, 32-bit and 128-bit service UUIDs.
 *          To see the format of a advertisement packet, see
 *          https://www.bluetooth.org/Technical/AssignedNumbers/generic_access_profile.htm
 *
 * @param[in]   p_target_uuid The UUID to search for.
 * @param[in]   p_adv_report  Pointer to the advertisement report.
 *
 * @retval      true if the UUID is present in the advertisement report. Otherwise false
 */
static bool is_uuid_present(ble_uuid_t               const * p_target_uuid,
                            ble_gap_evt_adv_report_t const * p_adv_report)
{
    ret_code_t   err_code;
    ble_uuid_t   extracted_uuid;
    uint16_t     index  = 0;
    uint8_t    * p_data = (uint8_t *)p_adv_report->data;

    while (index < p_adv_report->dlen)
    {
        uint8_t field_length = p_data[index];
        uint8_t field_type   = p_data[index + 1];

        if (   (field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE)
            || (field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE))
        {
            for (uint32_t i = 0; i < (field_length / UUID16_SIZE); i++)
            {
                err_code = sd_ble_uuid_decode(UUID16_SIZE,
                                              &p_data[i * UUID16_SIZE + index + 2],
                                              &extracted_uuid);

                if (err_code == NRF_SUCCESS)
                {
                    if ((extracted_uuid.uuid == p_target_uuid->uuid)
                        && (extracted_uuid.type == p_target_uuid->type))
                    {
                        return true;
                    }
                }
            }
        }
        else if (   (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE)
                 || (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE))
        {
            for (uint32_t i = 0; i < (field_length / UUID32_SIZE); i++)
            {
                err_code = sd_ble_uuid_decode(UUID32_SIZE,
                                              &p_data[i * UUID32_SIZE + index + 2],
                                              &extracted_uuid);

                if (err_code == NRF_SUCCESS)
                {
                    if (   (extracted_uuid.uuid == p_target_uuid->uuid)
                        && (extracted_uuid.type == p_target_uuid->type))
                    {
                        return true;
                    }
                }
            }
        }

        else if (   (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE)
                 || (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE))
        {
            err_code = sd_ble_uuid_decode(UUID128_SIZE, &p_data[index + 2], &extracted_uuid);
            if (err_code == NRF_SUCCESS)
            {
                if (   (extracted_uuid.uuid == p_target_uuid->uuid)
                    && (extracted_uuid.type == p_target_uuid->type))
                {
                    return true;
                }
            }
        }
        index += field_length + 1;
    }
    return false;
}

bool is_nordic_uart_name(ble_gap_evt_adv_report_t const * p_adv_report)
{
	uint32_t index = 0;
	uint8_t *p_data = (uint8_t *)p_adv_report->data;
	char *name;
	
	while (index < p_adv_report->dlen)
	{
		uint8_t field_length = p_data[index];
		uint8_t field_type   = p_data[index + 1];

		if (field_type == BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME ||
			field_type == BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME)
		{
			name =  (char *)&p_data[index + 2];
			name[field_length-1] = '\0';	
			
			if(strcmp("Nordic_UART",name) == 0)
				return true;
		}
		index += field_length + 1;
	}
	return false;	
}

bool default_adv_filter (ble_gap_evt_adv_report_t const * p_adv_report)
{
	return is_uuid_present(&m_nus_uuid, p_adv_report);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_INFO("Connected to a previously bonded device.\r\n");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_INFO("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.\r\n",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
			//if(p_evt->params.conn_sec_failed.error == 0x1006) //if peripheral lost keys
			//{
				//pm_peers_delete(); // delete our key and try to rebond	
			//}
				
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            // Bonds are deleted. Start scanning.
            //core_ble_scan_start();
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}

/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
}

/**@brief Callback handling NUS Client events.
 *
 * @details This function is called to notify the application of NUS client events.
 *
 * @param[in]   p_ble_nus_c   NUS Client Handle. This identifies the NUS client
 * @param[in]   p_ble_nus_evt Pointer to the NUS Client event.
 */

/**@snippet [Handling events from the ble_nus_c module] */
static void ble_nus_c_evt_handler(ble_nus_c_t * p_ble_nus_c, const ble_nus_c_evt_t * p_ble_nus_evt)
{
    ret_code_t err_code;

    switch (p_ble_nus_evt->evt_type)
    {
        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_INFO("Discovery complete.\r\n");
            err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
            APP_ERROR_CHECK(err_code);
			
            err_code = pm_conn_secure(p_ble_nus_c->conn_handle, false);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
			
            //err_code = ble_nus_c_tx_notif_enable(p_ble_nus_c);
            //APP_ERROR_CHECK(err_code);
            NRF_LOG_INFO("Connected to device with Nordic UART Service.\r\n");
            break;

        case BLE_NUS_C_EVT_NUS_TX_EVT:
            m_data_callback(p_ble_nus_evt->p_data, p_ble_nus_evt->data_len);
            break;
    }
}
/**@snippet [Handling events from the ble_nus_c module] */


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    ret_code_t            err_code;
    const ble_gap_evt_t * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_ADV_REPORT:
        {
            const ble_gap_evt_adv_report_t * p_adv_report = &p_gap_evt->params.adv_report;

            if (m_adv_filter(p_adv_report))
            {

                err_code = sd_ble_gap_connect(&p_adv_report->peer_addr,
                                              &m_scan_param,
                                              &m_connection_param,
                                              CONN_CFG_TAG);

                if (err_code == NRF_SUCCESS)
                {
                    // scan is automatically stopped by the connect
					m_evt_callback(p_ble_evt);
					
                    NRF_LOG_INFO("Connecting to target %02x%02x%02x%02x%02x%02x\r\n",
                             p_adv_report->peer_addr.addr[0],
                             p_adv_report->peer_addr.addr[1],
                             p_adv_report->peer_addr.addr[2],
                             p_adv_report->peer_addr.addr[3],
                             p_adv_report->peer_addr.addr[4],
                             p_adv_report->peer_addr.addr[5]
                             );
                }
            }
        }break; // BLE_GAP_EVT_ADV_REPORT

        case BLE_GAP_EVT_CONNECTED:
		  	m_in_scan = false;
			
            NRF_LOG_INFO("Connected to target\r\n");
            err_code = ble_nus_c_handles_assign(&m_ble_nus_c, p_ble_evt->evt.gap_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);

			m_evt_callback(p_ble_evt);
			m_retry_db_disc = false;
            // start discovery of services. The NUS Client waits for a discovery result

			err_code = ble_db_discovery_start(&m_ble_db_discovery,p_ble_evt->evt.gap_evt.conn_handle);
            if (err_code == NRF_ERROR_BUSY)
            {
                NRF_LOG_INFO("ble_db_discovery_start() returned busy, will retry later.\r\n");
                m_retry_db_disc = true;
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }

            break; // BLE_GAP_EVT_CONNECTED
		case BLE_GAP_EVT_DISCONNECTED:
			m_in_scan = false;
            NRF_LOG_INFO("Disconnected.\r\n");
			m_evt_callback(p_ble_evt);	
			
			            // Reset DB discovery structure.
            memset(&m_ble_db_discovery, 0 , sizeof (m_ble_db_discovery));
			
			break;
        case BLE_GAP_EVT_TIMEOUT:
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_INFO("Connection Request timed out.\r\n");
            }
            break; // BLE_GAP_EVT_TIMEOUT

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            // Accepting parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST
			
		case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
			//NRF_LOG_INFO("GATT TX complete.\r\n");
		  //ble_flag_tx_complete = true;
		  //ble_packet_counter += p_ble_evt->evt.gatts_evt.params.hvn_tx_complete.count;

			break; // BLE_EVT_TX_COMPLETE
	  
        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

        default:
            break;
    }
}

/**@brief Function for handling the Application's system events.
 *
 * @param[in]   sys_evt   system event.
 */
static void on_sys_evt(uint32_t sys_evt)
{
    switch (sys_evt)
    {
        case NRF_EVT_FLASH_OPERATION_SUCCESS:
            /* fall through */
        case NRF_EVT_FLASH_OPERATION_ERROR:

            if (m_memory_access_in_progress)
            {
                m_memory_access_in_progress = false;
                core_ble_scan_start(NULL);
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack event has
 *          been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
	ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);	
    nrf_ble_gatt_on_ble_evt(&m_gatt, p_ble_evt);
    ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
    ble_nus_c_on_ble_evt(&m_ble_nus_c,p_ble_evt);
	on_ble_evt(p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    fs_sys_event_handler(sys_evt);
    on_sys_evt(sys_evt);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = softdevice_app_ram_start_get(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Overwrite some of the default configurations for the BLE stack.
    ble_cfg_t ble_cfg;

    // Configure the maximum number of connections.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = PERIPHERAL_LINK_COUNT;
    ble_cfg.gap_cfg.role_count_cfg.central_role_count = CENTRAL_LINK_COUNT;
    ble_cfg.gap_cfg.role_count_cfg.central_sec_count  = 1;
    err_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Configure the maximum ATT MTU.
    memset(&ble_cfg, 0x00, sizeof(ble_cfg));
    ble_cfg.conn_cfg.conn_cfg_tag                 = CONN_CFG_TAG;
    ble_cfg.conn_cfg.params.gatt_conn_cfg.att_mtu = NRF_BLE_GATT_MAX_MTU_SIZE;
    err_code = sd_ble_cfg_set(BLE_CONN_CFG_GATT, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Configure the maximum event length.
    memset(&ble_cfg, 0x00, sizeof(ble_cfg));
    ble_cfg.conn_cfg.conn_cfg_tag                     = CONN_CFG_TAG;
    ble_cfg.conn_cfg.params.gap_conn_cfg.event_length = 320;
    ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count   = BLE_GAP_CONN_COUNT_DEFAULT;
    err_code = sd_ble_cfg_set(BLE_CONN_CFG_GAP, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Configure the ATT table size.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gatts_cfg.attr_tab_size.attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_MIN;
    err_code = sd_ble_cfg_set(BLE_GATTS_CFG_ATTR_TAB_SIZE, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = softdevice_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
	
	    // Register with the SoftDevice handler module for System events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
	uint32_t err_code;
    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
    {
        NRF_LOG_INFO("ATT MTU exchange completed.\r\n");

        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Ble NUS max data length set to 0x%X(%d)\r\n", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
	if (m_retry_db_disc)
    {
        NRF_LOG_DEBUG("Retrying DB discovery.\r\n");

        m_retry_db_disc = false;

        // Discover peer's services.
        err_code = ble_db_discovery_start(&m_ble_db_discovery, p_evt->conn_handle);

        if (err_code == NRF_ERROR_BUSY)
        {
            NRF_LOG_DEBUG("ble_db_discovery_start() returned busy, will retry later.\r\n");
            m_retry_db_disc = true;
        }
        else
        {
            APP_ERROR_CHECK(err_code);
        }
    }
}


/**@brief Function for initializing the GATT library. */
void gatt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_central_set(&m_gatt, NRF_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the NUS Client. */
static void nus_c_init(void)
{
    ret_code_t       err_code;
    ble_nus_c_init_t init;

    init.evt_handler = ble_nus_c_evt_handler;

    err_code = ble_nus_c_init(&m_ble_nus_c, &init);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for initializing the Database Discovery Module.
 */
static void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
}


void core_ble_set_adv_filter (bool (*is_adv_correct)(ble_gap_evt_adv_report_t const * p_adv_report))
{
	m_adv_filter = 	is_adv_correct;	
}
					 
					 
void core_ble_init (core_ble_event_callback_t evt_callback, 
					core_ble_data_callback_t data_callback)
{
	m_evt_callback  = evt_callback;
	m_data_callback = data_callback;
	m_adv_filter = default_adv_filter;
	db_discovery_init();
    ble_stack_init();
    gatt_init();
    nus_c_init();
	peer_manager_init();
}

void core_ble_disconnect(void)
{
	ret_code_t err_code = sd_ble_gap_disconnect(m_ble_nus_c.conn_handle,
					BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
	if (err_code != NRF_ERROR_INVALID_STATE)
	{
		APP_ERROR_CHECK(err_code);
	}	
}

uint16_t core_ble_get_current_data_len (void)
{
	return m_ble_nus_max_data_len;
}

uint32_t core_ble_send_data (uint8_t* data, uint32_t size)
{
	ret_code_t ret_val; 
	if(size > m_ble_nus_max_data_len)
	{
		ret_val = NRF_ERROR_INVALID_PARAM;
		return ret_val;
	}
	
	do
	{
		ret_val = ble_nus_c_string_send(&m_ble_nus_c, data, size);

		if (ret_val != NRF_ERROR_INVALID_STATE && ret_val != NRF_ERROR_BUSY &&
			ret_val != NRF_ERROR_RESOURCES)
			APP_ERROR_CHECK(ret_val);
	} while (ret_val == NRF_ERROR_BUSY || ret_val == NRF_ERROR_RESOURCES);
	
	return ret_val;
}

bool core_ble_is_connect(void)
{
	return m_ble_nus_c.conn_handle != BLE_CONN_HANDLE_INVALID;
}

void core_ble_erase_bond(void)
{
	NRF_LOG_INFO("Erase bond\r\n");
	pm_peers_delete(); // delete our key and try to rebond	
}