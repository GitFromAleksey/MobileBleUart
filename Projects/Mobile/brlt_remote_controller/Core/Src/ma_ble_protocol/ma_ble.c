#include "ma_ble.h"
#include "core_ble.h"
#include "crc_calculate.h"

#include "string.h"
#include "stdlib.h"
//MA Defines
#define IRM_SYSTEM_TYPE				((uint16_t)0x8001)
#define IRM_SYSTEM_STYPE			((uint16_t)0x0000)
#define BRLT_SYSTEM_TYPE			((uint16_t)0x8002)
#define BRLT_SYSTEM_STYPE			((uint16_t)0x0000)

#define BLE_FRAME_FULL_SIZE(data_size)	(sizeof(not_wrapped_ma_ble_frame_t) \
										+ (data_size))
#define BLE_MAGIC_NUMBER				0x3A
#define BLE_CRC_SIZE					(sizeof(ble_crc_t))
#define BLE_RX_BUFFER_SIZE				(1024 + 64)

#define FRAME_QUEUE_DEPTH				8

ma_ble_event_handler_t m_ma_event_handler;
ma_ble_frame_callback_t m_ma_frame_callback;

static bool m_search_device_in_pairing_mode = false;

typedef enum{
	BLE_PACKET_TYPE_FIRST   = 0,
	BLE_PACKET_TYPE_CURRENT = 1,
	BLE_PACKET_TYPE_FEND    = 2,
	BLE_PACKET_TYPE_END     = 3, //Bytes of MA packets
}ble_packet_type_t;

typedef __packed uint16_t ble_crc_t;

typedef __packed struct ble_frame_type_t_
{
	uint16_t size;
	uint8_t  magic_number;
	uint8_t  random_number;
	uint16_t type;
	uint32_t request_id;
	uint8_t  data[BLE_CRC_SIZE];
}not_wrapped_ma_ble_frame_t;


struct ma_ble_frame_queue
{
	not_wrapped_ma_ble_frame_t* qu[FRAME_QUEUE_DEPTH];
	uint8_t first, last;
};
struct ma_ble_frame_queue ma_queue = {.first = 1, .last = 1};

static bool put_in_frame_queue(not_wrapped_ma_ble_frame_t* frame)
{
	if( (ma_queue.last % (FRAME_QUEUE_DEPTH - 1)) + 1 != ma_queue.first)
	{
		ma_queue.qu[ma_queue.last] = frame;
		ma_queue.last = (ma_queue.last % (FRAME_QUEUE_DEPTH - 1)) + 1;
		return true;
	}
	else
		return false;
}

static not_wrapped_ma_ble_frame_t* take_from_frame_queue(void)
{
	not_wrapped_ma_ble_frame_t* temp;
	if(ma_queue.first != ma_queue.last)
	{		
		temp = ma_queue.qu[ma_queue.first];		
		ma_queue.first = (ma_queue.first % (FRAME_QUEUE_DEPTH - 1)) + 1;		
		return temp;
	}
	else
		return NULL;
}


static uint32_t adv_report_parse(uint8_t type, uint8_array_t * p_advdata, uint8_array_t * p_typedata)
{
    uint32_t  index = 0;
    uint8_t * p_data;

    p_data = p_advdata->p_data;

    while (index < p_advdata->size)
    {
        uint8_t field_length = p_data[index];
        uint8_t field_type   = p_data[index + 1];

        if (field_type == type)
        {
            p_typedata->p_data = &p_data[index + 2];
            p_typedata->size   = field_length - 1;
            return NRF_SUCCESS;
        }
        index += field_length + 1;
    }
    return NRF_ERROR_NOT_FOUND;
}

static bool is_ma_device(const uint16_t type,const uint16_t stype, bool pairing,
                            const ble_gap_evt_adv_report_t *p_adv_report)
{
	uint8_array_t adv_data;
	uint8_array_t dev_data;
	
	__packed struct ma_adv_dev_info //struct for create adv packet
	{
	  uint16_t com_id;	
	  uint8_t dev_mac[6]; //BLE address of custom???
	  
	  uint16_t dev_type; //Br3.0, CO2+, Ir-remote
	  uint16_t dev_stype; //sub-type
	  uint8_t  dev_pair; //ready for pairing 
	};	
	struct ma_adv_dev_info manuf_data;
	
    adv_data.p_data = (uint8_t *)p_adv_report->data;
	adv_data.size = p_adv_report->dlen;

	if (adv_report_parse(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,&adv_data,&dev_data) == NRF_SUCCESS)		
	{	
		memcpy(&manuf_data,dev_data.p_data,dev_data.size);
		if(manuf_data.dev_type == type && manuf_data.dev_stype == stype
		   && manuf_data.dev_pair == (pairing? 1 : 0) )
			return true;
	}
	return false;
}


static bool is_ma_irm(const ble_gap_evt_adv_report_t* p_adv_report)
{
	return is_ma_device(IRM_SYSTEM_TYPE, IRM_SYSTEM_STYPE,
						m_search_device_in_pairing_mode, p_adv_report);
}

static bool is_ma_brlt(const ble_gap_evt_adv_report_t* p_adv_report)
{
	return is_ma_device(BRLT_SYSTEM_TYPE, BRLT_SYSTEM_STYPE,
						m_search_device_in_pairing_mode, p_adv_report);
}


static void ma_ble_data_recieve_callback (void *data, uint32_t size) // fire after the end of recive packet(frame)
{
	not_wrapped_ma_ble_frame_t* rx_frame = data;
	ma_ble_frame_t wrapped_frame = {0}; 
	
	if(rx_frame->magic_number == BLE_MAGIC_NUMBER &&
		rx_frame->size   == size &&
		CRC_CCITT(0xFFFF, rx_frame, size, 1) == 0)
	{
		wrapped_frame.data = rx_frame->data;
		wrapped_frame.data_size = rx_frame->size;
		wrapped_frame.request_id = rx_frame->request_id;
		wrapped_frame.type = rx_frame->type;
			
		m_ma_frame_callback(wrapped_frame);	
	}
}

static void ble_data_handler(uint8_t * p_data, uint16_t length)
{
	static uint8_t buffer[BLE_RX_BUFFER_SIZE]; //big MA packet - 1024 bytes
	static uint8_t *pbuffer = buffer; //pointer in big MA packet
	  
	if(p_data && length) //if data is available
	{
		length--; //Subtract the length of the first byte. Only the TYPE is read from it.
		switch(*p_data++ >> 6) // 1.Dereferencing of a pointer 2.Bit shift 3. Address increment
		{
			case BLE_PACKET_TYPE_FIRST:
				if(length <= BLE_RX_BUFFER_SIZE)
				{
					memcpy(buffer, p_data, length);
					pbuffer = buffer + length;
				}
				else
					NRF_LOG_ERROR("ble_tx_packet: err first\r\n");
				break;
					
			case BLE_PACKET_TYPE_CURRENT:
				if(length <= BLE_RX_BUFFER_SIZE - (pbuffer - buffer)) //If there is place left after the first TYPE packets
				{
					memcpy(pbuffer, p_data, length);
					pbuffer += length;
				}
				else
				{
					NRF_LOG_ERROR("ble_tx_packet: err cur\r\n");
					pbuffer = buffer;
				}
				break;
					
			case BLE_PACKET_TYPE_FEND:
				ma_ble_data_recieve_callback(p_data, length); //?
				break;
					
			case BLE_PACKET_TYPE_END:
				if(length <= BLE_RX_BUFFER_SIZE - (pbuffer - buffer) && pbuffer != buffer) //If there is place left after the first TYPE packets and current TY packets
				{
					memcpy(pbuffer, p_data, length);
					pbuffer += length;
					ma_ble_data_recieve_callback(buffer, pbuffer - buffer);
				}
				else
					NRF_LOG_ERROR("ble_tx_packet: err end\r\n");
				pbuffer = buffer;
				break;
		}
	}		
}

/**@brief 
 */
static int32_t ble_send_data(const void *data, uint32_t size)
{
	static uint8_t buffer[NRF_BLE_GATT_MAX_MTU_SIZE - 3];
	uint32_t data_packet_size = core_ble_get_current_data_len() - 1;
	const uint8_t *data8 = data;
  
	data_packet_size = (size > data_packet_size)?(data_packet_size):(size);
	size -= data_packet_size;
	buffer[0] = ((size)?(BLE_PACKET_TYPE_FIRST):(BLE_PACKET_TYPE_FEND)) << 6;
	memcpy(&buffer[1], data8, data_packet_size);
	data8 += data_packet_size;
 	
	core_ble_send_data(buffer, data_packet_size + 1);
	
	while(size)
	{
		data_packet_size = (size > data_packet_size)?(data_packet_size):(size);
		size -= data_packet_size;
		buffer[0] = ((size)?(BLE_PACKET_TYPE_CURRENT):(BLE_PACKET_TYPE_END)) << 6;
		memcpy(&buffer[1], data8, data_packet_size);
		data8 += data_packet_size;

		core_ble_send_data(buffer, data_packet_size + 1);
	}
	  
	return 0;
}


static void ble_evt_handler (ble_evt_t * p_ble_evt)
{
	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_ADV_REPORT:
			m_ma_event_handler(MA_BLE_FIND_DEVICE);
			break;
			
		case BLE_GAP_EVT_CONNECTED:
			m_ma_event_handler(MA_BLE_CONNECTION);
			break;
			
		case BLE_GAP_EVT_DISCONNECTED:
			m_ma_event_handler(MA_BLE_DISCONNECTION);
			break;
			
		default:		
			break;		
	}
}

void ma_ble_start_search (void)
{
	m_ma_event_handler(MA_BLE_START_SEARCH);
	bool is_wl = false;
	
	if (core_ble_scan_start(&is_wl))
	{
	  	if(is_wl)
		{
		NRF_LOG_INFO("Search only devices in pairing mode\r\n");
		m_search_device_in_pairing_mode = true;
		}
		else
		  m_search_device_in_pairing_mode = false;
	}				  
}

void ma_ble_disconnect (void)
{
	core_ble_disconnect();
}

int32_t ma_ble_put_frame_in_queue (ma_ble_frame_t frame)
{
	not_wrapped_ma_ble_frame_t* tx_frame;
	uint32_t frame_size;
	int32_t ret_val;
  
	frame_size = BLE_FRAME_FULL_SIZE(frame.data_size);
	
	if((tx_frame = malloc(frame_size)) != NULL)
	{
		tx_frame->size			= frame_size;
		tx_frame->type			= frame.type;
		tx_frame->request_id	= frame.request_id;
		tx_frame->magic_number	= BLE_MAGIC_NUMBER;
		tx_frame->random_number	= 0xAD;
		memcpy(tx_frame->data, frame.data, frame.data_size);
		
		*(ble_crc_t*)&tx_frame->data[frame.data_size] = 
			CRC_CCITT(0xFFFF, tx_frame, frame_size - 2, 1);
    
		if(!put_in_frame_queue(tx_frame))
			free(tx_frame);
	
  	}
	else
		ret_val = 0;
  
	return ret_val;
}

void ma_ble_frames_flush(void)
{
	not_wrapped_ma_ble_frame_t* temp;
	
	while(temp = take_from_frame_queue())
	{
		ble_send_data(temp, temp->size);
		free(temp);
	}
}

uint32_t ma_ble_init (uint8_t device_type, bool erase,
					  ma_ble_event_handler_t evt_h, 
					  ma_ble_frame_callback_t callback)
{
	core_ble_init(ble_evt_handler, ble_data_handler);
	
	if (erase)
	  core_ble_erase_bond();
	
	if(device_type == IRM)
		core_ble_set_adv_filter(is_ma_irm);
	else if (device_type == BRLT)
		core_ble_set_adv_filter(is_ma_brlt);

	m_ma_event_handler = evt_h;	
	m_ma_frame_callback = callback;
	
	return 0;
}

bool ma_ble_is_connect(void)
{
	return core_ble_is_connect();
}