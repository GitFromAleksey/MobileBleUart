/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "br4s_main.h"
/*Private defines*/
// clang-format off
#define PERIPHERAL_NAME 			"Breezer 4S"
#define SHORT_PERIPHERAL_NAME		"Tion "
#define BLE_RX_BUFFER_SIZE          96

#define BR4S_SYSTEM_TYPE			((uint16_t)0x8003)
#define BR4S_SYSTEM_STYPE			((uint16_t)0x0000)

#define ADAPTER_ID 					0x3D
#define BREEZER_CUB_ID				0xB3
#define FRAME_SIZE					20
#define CALCULATE_CRC				0x5A

/* queue size defines */
#define RAW_INPUT_QUEUE_SIZE					3
#define RAW_BUFFER_SIZE							sizeof(ble_raw_frame_t)

#define BR4S_MAGIC_NUMBER			0x3A

#define SECONDS_PER_YEAR    (int64_t)31436000
#define SECONDS_PER_DAY     (int64_t)86400
#define SECONDS_PER_HOUR    (int64_t)3600
#define DAYS_BEFORE_1970    (int64_t)719499
// clang-format on

/*Private types*/
typedef uint32_t (*create_com_t)(void * p_context);

/*Private functions prototypes*/
static uint32_t init_protocol_instance(void * p_context);
static uint32_t deinit_protocol_instance(void * p_context);
static uint32_t input_data_handler(void * p_context, uint32_t length);
static uint32_t output_data_handler(void * p_context, uint32_t length);
static uint32_t input_raw_data_handler(void * p_context, uint32_t length);
static uint32_t output_raw_data_handler(void * p_context, uint32_t length);

uint32_t output_frame_handler (void * p_context, uint32_t length);

static uint32_t br4s_rem_get_params_from_frame(void * p_context, uint32_t length);
static uint32_t br4s_rem_get_sys_param_from_frame(void * p_context, uint32_t length);
static uint32_t br4s_rem_get_timer_settings_from_frame(void * p_context, uint32_t length);
static uint32_t br4s_rem_get_time_from_frame(void * p_context, uint32_t length);
static uint32_t prepare_frame_to_send(void * p_context, uint32_t length);
static uint32_t concate_frame_meta_with_data(void * p_context, uint32_t frame_type);
static uint32_t create_set_com_template(void * p_context);

static uint32_t create_request_sys_param_com (void* p_context);
static uint32_t create_request_dev_param_com (void* p_context);
static uint32_t create_set_params_com (void* p_context);
static uint32_t create_set_filter_time_com (void* p_context);
static uint32_t create_reset_filter_time_com (void* p_context);
static uint32_t create_hard_reset_com (void* p_context);
static uint32_t create_request_time_com (void* p_context);
static uint32_t create_set_time_com (void* p_context);
static uint32_t create_request_on_timer_param_com (void* p_context);
static uint32_t create_request_off_timer_param_com (void* p_context);
static uint32_t create_set_on_timer_com (void* p_context);
static uint32_t create_set_off_timer_com (void* p_context);
static uint32_t create_set_on_timer_off_com (void* p_context);
static uint32_t create_set_off_timer_off_com (void* p_context);


rtc_time_format_t unix_to_time (int64_t unix_time);
int64_t time_to_unix (rtc_time_format_t time);

static uint32_t adv_data_parser(uint32_t pair_mode,
				const ble_gap_evt_adv_report_t * p_adv_data);
static uint32_t adv_report_parse(uint8_t type, uint8_array_t * p_advdata, 
								 uint8_array_t * p_typedata);
/*Exported constants*/
const protocol_m_list_t	br4s_protocol_vptr = {
	.init 				= &init_protocol_instance,
	.deinit 			= &deinit_protocol_instance,
	.input_data 		= &input_data_handler,
	.output_data 		= &output_data_handler,
	.input_raw_data 	= &input_raw_data_handler,
	.output_raw_data 	= &output_raw_data_handler,
};

static br4s_protocol_t br4s_protocol = {0};

static const create_com_t br4s_create_com[BR4S_COM_COUNT] = {
	NULL,
	&create_request_sys_param_com,
	&create_request_dev_param_com,
	&create_set_params_com,
	&create_set_filter_time_com,
	&create_reset_filter_time_com,
	&create_hard_reset_com,
	&create_request_time_com,
	&create_set_time_com,
	&create_request_on_timer_param_com,
	&create_request_off_timer_param_com,
	&create_set_on_timer_com,
	&create_set_off_timer_com,
	&create_set_off_timer_off_com,
	&create_set_on_timer_off_com,
};

/*Private variables*/

NRF_QUEUE_DEF(br4s_raw_queue_frame_t, br4s_raw_input,
			  BR4S_RAW_INPUT_QUEUE_SIZE, NRF_QUEUE_MODE_NO_OVERFLOW);
NRF_QUEUE_DEF(br4s_raw_queue_frame_t, br4s_output,
			  BR4S_RAW_INPUT_QUEUE_SIZE, NRF_QUEUE_MODE_NO_OVERFLOW);

/*Private macro*/
// clang-format off
// clang-format on
/*Private constants*/

/*Private functions*/
static uint32_t adv_data_parser(uint32_t pair_mode,
				const ble_gap_evt_adv_report_t * p_adv_data)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	br4s_ble_adv_dev_info_t * p_local_adv_data;
	uint8_array_t b_local_raw_adv_data = {0};
	uint8_array_t b_local_adv_data = {0};
	
	if ((p_adv_data->data[14] == 0x80) &&
	    (p_adv_data->data[13] == 0x03))
	{
		b_local_raw_adv_data.p_data = (uint8_t *)p_adv_data->data;
		b_local_raw_adv_data.size = p_adv_data->dlen;
		
		if(NRF_SUCCESS == adv_report_parse(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 
						&b_local_raw_adv_data, &b_local_adv_data))
		{
			p_local_adv_data = (br4s_ble_adv_dev_info_t *)b_local_adv_data.p_data;
			if(p_local_adv_data->dev_type == BR4S_SYSTEM_TYPE && p_local_adv_data->dev_stype == BR4S_SYSTEM_STYPE
			   && p_local_adv_data->dev_pair == (pair_mode? 1 : 0))
			{
				ret_val = true;
			}
			else
			{
				ret_val = false;
			}
		}
		else
		{
			ret_val = false;
		}
	}
	return ret_val;
}


static uint32_t adv_report_parse(uint8_t type, uint8_array_t * p_advdata, 
								 uint8_array_t * p_typedata)
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

static uint32_t init_protocol_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	br4s_main_t * p_local_br4s_main;
	
	p_local_br4s_main = (br4s_main_t *)p_context;
	
	br4s_protocol.p_br4s_params = &p_local_br4s_main->br4s_state;
	br4s_protocol.p_brc_main	= p_local_br4s_main->p_brc_main;
	br4s_protocol.p_br4s_params->br4s_invalid_speed = 0;
	br4s_protocol.p_br4s_state_valid	= &p_local_br4s_main->br4s_state_valid;
	br4s_protocol.p_br4s_callback		= p_local_br4s_main->p_br4s_callback;

	return ret_val;
}

static uint32_t deinit_protocol_instance(void * p_context)
{
	br4s_protocol.p_br4s_params 		= NULL;
	br4s_protocol.p_brc_main			= NULL;
	br4s_protocol.p_br4s_state_valid	= NULL;
	br4s_protocol.p_br4s_callback		= NULL;


	nrf_queue_reset(&br4s_raw_input);
	nrf_queue_reset(&br4s_output);

	return NRF_SUCCESS;
}

static uint32_t input_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val;
	uint8_t com_type;
	static data_t b_frame;
	
	com_type = *(uint8_t *)p_context;
	if (com_type < BR4S_COM_COUNT && br4s_create_com[com_type] != NULL)
		ret_val = br4s_create_com[com_type]((void *)&b_frame);
	
	if(ret_val == NRF_SUCCESS)
	{
		ret_val = prepare_frame_to_send((void *)b_frame.p_data, b_frame.data_len);
	}

	return ret_val;
}

static uint32_t output_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static uint8_t b_local_data[BLE_RX_BUFFER_SIZE];  
	static uint8_t *p_local_data = b_local_data;
	static uint8_t b_prev_packet_type = BR4S_PACKET_TYPE_NONE;
	static br4s_raw_queue_frame_t b_buffer_frame;
	
	if(nrf_queue_peek(&br4s_raw_input, (void *)&b_buffer_frame) 
	   == NRF_ERROR_NOT_FOUND)
		return NRF_ERROR_NOT_FOUND;
	
	ret_val = nrf_queue_pop(&br4s_raw_input, (void *)&b_buffer_frame);
	
	if(ret_val == NRF_SUCCESS)
	{
		switch (b_buffer_frame.data.packet_type)
		{
			case BR4S_PACKET_TYPE_FIRST:
			{
				if (b_buffer_frame.data_len <= BLE_RX_BUFFER_SIZE)
				{
					memcpy(b_local_data, b_buffer_frame.data.data, 
						   b_buffer_frame.data_len);
					p_local_data = b_local_data + b_buffer_frame.data_len;
					b_prev_packet_type = BR4S_PACKET_TYPE_FIRST;
					ret_val = NRF_SUCCESS;
				}
				else
						ret_val = NRF_ERROR_INVALID_LENGTH;
			}	break;
			case BR4S_PACKET_TYPE_CURRENT:
			{
				if ((b_prev_packet_type == BR4S_PACKET_TYPE_FIRST)||
				    (b_prev_packet_type == BR4S_PACKET_TYPE_CURRENT))
				{
					if (b_buffer_frame.data_len <= (BLE_RX_BUFFER_SIZE - 
										(p_local_data - b_local_data)))
					{
						memcpy(p_local_data, b_buffer_frame.data.data, 
							   b_buffer_frame.data_len);
						p_local_data += b_buffer_frame.data_len;
						b_prev_packet_type = BR4S_PACKET_TYPE_CURRENT;
						ret_val = NRF_SUCCESS;
					}
					else
						ret_val = NRF_ERROR_INVALID_LENGTH;
				}
				else
					ret_val = NRF_ERROR_INVALID_DATA;
			}	break;
			case BR4S_PACKET_TYPE_FEND:
			{
				memcpy(b_local_data, b_buffer_frame.data.data, 
						   b_buffer_frame.data_len);
				//p_local_data = b_local_data + b_buffer_frame.data_len;
				ret_val = output_frame_handler((void *)&b_local_data, 
												b_buffer_frame.data_len);
				p_local_data = b_local_data;
				b_prev_packet_type = BR4S_PACKET_TYPE_FEND;
			}	break;
			case BR4S_PACKET_TYPE_END:
			{
				if ((b_prev_packet_type == BR4S_PACKET_TYPE_FIRST)||
				    (b_prev_packet_type == BR4S_PACKET_TYPE_CURRENT))
				{
					if ((b_buffer_frame.data_len <= (BLE_RX_BUFFER_SIZE - 
										(p_local_data - b_local_data))) &&
						(p_local_data != b_local_data))
					{
						memcpy(p_local_data, b_buffer_frame.data.data, 
							   b_buffer_frame.data_len);
						p_local_data += b_buffer_frame.data_len;
						ret_val = output_frame_handler((void *)&b_local_data, 
												(p_local_data - b_local_data));
						p_local_data = b_local_data;
						b_prev_packet_type = BR4S_PACKET_TYPE_END;
					}
					else
						ret_val = NRF_ERROR_INVALID_LENGTH;
				}
				else
					ret_val = NRF_ERROR_INVALID_DATA;
			}	break;
			default:
				
				break;
		}
	}
	return ret_val;
}

static uint32_t input_raw_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val;
	static br4s_raw_queue_frame_t b_buffer_frame;
	static uint8_t * p_local_raw_data;
	
	p_local_raw_data = (uint8_t *)p_context;
	length--;
	b_buffer_frame.data_len = length;
	b_buffer_frame.data.packet_type = p_local_raw_data[0];
	memcpy(b_buffer_frame.data.data, &p_local_raw_data[1], length);
	
	ret_val = nrf_queue_push(&br4s_raw_input, (const void *)&b_buffer_frame);
	
	return ret_val;
}

static uint32_t output_raw_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_raw_queue_frame_t 	b_local_raw_data;
	static br4s_data_t * p_input_raw_data;
	
	p_input_raw_data = (br4s_data_t *)p_context;
	
	if(nrf_queue_peek(&br4s_output, (void *)&b_local_raw_data) 
	   == NRF_ERROR_NOT_FOUND)
		return NRF_ERROR_NOT_FOUND;
	
	ret_val = nrf_queue_pop(&br4s_output, (void *)&b_local_raw_data);
	if(ret_val == NRF_SUCCESS)
	{
		p_input_raw_data->data_len = b_local_raw_data.data_len;
		p_input_raw_data->p_data = (uint8_t *)&b_local_raw_data.data;
	}
	else
	{
		p_input_raw_data->data_len = NULL;
		p_input_raw_data->p_data = NULL;
	}
	return ret_val;
}


uint32_t output_frame_handler (void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static ma_meta_br4s_frame_t b_local_frame_meta;
	static uint8_t * p_local_frame_data;
	uint8_t * p_local_crc;
	
	p_local_crc = (uint8_t *)p_context + length - BR4S_CRC_SIZE;
	
	memcpy(&b_local_frame_meta, p_context, 
		   (sizeof(ma_meta_br4s_frame_t) - BR4S_CRC_SIZE));
	memcpy(b_local_frame_meta.data, 
		   p_local_crc, 
		   BR4S_CRC_SIZE);
	p_local_frame_data = (uint8_t *)p_context + BR4S_META_SIZE;

	if(b_local_frame_meta.frame_size == length)
	{
		switch(b_local_frame_meta.frame_type)
    	{
		case BR4S_FRAME_TYPE_GET_MODE_RESP:
			br4s_rem_get_params_from_frame(p_local_frame_data, 
										  (length - BR4S_META_SIZE - BR4S_CRC_SIZE));
			break;
		case BR4S_FRAME_TYPE_DEV_STATUS_RESP:
			br4s_rem_get_sys_param_from_frame(p_local_frame_data, 
									  (length - BR4S_META_SIZE - BR4S_CRC_SIZE));
			break;
		case BR4S_FRAME_TYPE_GET_TIMER_RESP:
			br4s_rem_get_timer_settings_from_frame(p_local_frame_data, 
										   (length - BR4S_META_SIZE - BR4S_CRC_SIZE));
			break;
		case BR4S_FRAME_TYPE_GET_TIMERS_STATE_RESP:

			break;
		case BR4S_FRAME_TYPE_GET_TIME_RESP:
			br4s_rem_get_time_from_frame(p_local_frame_data, 
										(length - BR4S_META_SIZE - BR4S_CRC_SIZE));
			break;
		case BR4S_FRAME_TYPE_GET_ERR_CNT_RESP:
		case BR4S_FRAME_TYPE_GET_CURR_TEST_REQ:
		default:
			break;
    	}
	}
        return ret_val;
}

static uint32_t prepare_frame_to_send(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_raw_queue_frame_t	ble_raw_frame;
	static uint8_t * p_local_br4s_frame;
	static uint8_t * p_local_current_buff;
	
	p_local_br4s_frame = (uint8_t *)p_context;
	p_local_current_buff = p_local_br4s_frame;
	if(length > (BLE_NUS_MAX_DATA_LEN - 1))
	{
		while(p_local_current_buff != (p_local_br4s_frame + length))
		{
			if(p_local_current_buff == p_local_br4s_frame)
			{
				ble_raw_frame.data_len = BLE_NUS_MAX_DATA_LEN;
				ble_raw_frame.data.packet_type = BR4S_PACKET_TYPE_FIRST;
				memcpy(ble_raw_frame.data.data, p_local_current_buff, ble_raw_frame.data_len);
				ret_val = nrf_queue_push(&br4s_output, (void *)&ble_raw_frame);
				p_local_current_buff = p_local_br4s_frame + BLE_NUS_MAX_DATA_LEN - 1;
			}
			else if(p_local_current_buff <= 
					(p_local_br4s_frame + length - BLE_NUS_MAX_DATA_LEN - 1))
			{
				ble_raw_frame.data_len = BLE_NUS_MAX_DATA_LEN;
				ble_raw_frame.data.packet_type = BR4S_PACKET_TYPE_CURRENT;
				memcpy(ble_raw_frame.data.data, p_local_current_buff, ble_raw_frame.data_len);
				ret_val = nrf_queue_push(&br4s_output, (void *)&ble_raw_frame);
				p_local_current_buff += (BLE_NUS_MAX_DATA_LEN - 1);
			}
			else
			{
				ble_raw_frame.data_len = ((p_local_br4s_frame + length) - p_local_current_buff)+1;
				ble_raw_frame.data.packet_type = BR4S_PACKET_TYPE_END;
				memcpy(ble_raw_frame.data.data, p_local_current_buff, ble_raw_frame.data_len);
				ret_val = nrf_queue_push(&br4s_output, (void *)&ble_raw_frame);
				p_local_current_buff = p_local_br4s_frame + length;
			}
		}
	}
	else
	{
		ble_raw_frame.data_len = length + 1;
		ble_raw_frame.data.packet_type = BR4S_PACKET_TYPE_FEND;
		memcpy(ble_raw_frame.data.data, p_local_br4s_frame, ble_raw_frame.data_len);
		ret_val = nrf_queue_push(&br4s_output, (void *)&ble_raw_frame);
	}
	return ret_val;
}

static uint32_t br4s_rem_get_params_from_frame(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_response_t * p_local_sys_state_resp;
	
	p_local_sys_state_resp = (br4s_rc_response_t *)p_context;

	if (br4s_protocol.p_br4s_params != NULL)
	{
		if (br4s_protocol.p_br4s_params->br4s_first_responce_dev_received != 1)
		{
			br4s_protocol.p_br4s_params->br4s_first_responce_dev_received = 1;
			br4s_protocol.p_br4s_callback((void *)&br4s_protocol, NULL, NULL, 
							BUTTON_EVT_BLOCK_OVF);
		}
		
		memcpy(&(br4s_protocol.p_br4s_params->dev_info.flags), &(p_local_sys_state_resp->flags), sizeof(br4s_rc_response_flags_t));

		br4s_protocol.p_br4s_params->dev_info.main_params =  p_local_sys_state_resp->params;
		br4s_protocol.p_br4s_params->dev_info.sensors_state =  p_local_sys_state_resp->sensor_state;
		br4s_protocol.p_br4s_params->dev_info.counters =  p_local_sys_state_resp->counters;
		br4s_protocol.p_br4s_params->dev_info.errors =  p_local_sys_state_resp->errors;
		br4s_protocol.p_br4s_params->dev_info.max_speed =  p_local_sys_state_resp->limits.max_fan_speed;
		br4s_protocol.p_br4s_params->dev_info.heater_var =  p_local_sys_state_resp->heater_var;

		*br4s_protocol.p_br4s_state_valid = 1;
		br4s_protocol.p_br4s_callback((void *)&br4s_protocol, 
				NULL, NULL, START_DEV_POLL);
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t br4s_rem_get_sys_param_from_frame(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_dev_status_t * p_local_sys_state_resp;
	
	p_local_sys_state_resp = (br4s_dev_status_t *)p_context;
	
	if(length > sizeof(br4s_sys_info_t))
		return NRF_ERROR_NOT_FOUND;

	if (br4s_protocol.p_br4s_params != NULL)
	{
		if (!br4s_protocol.p_br4s_params->br4s_first_responce_sys_received)
			br4s_protocol.p_br4s_params->br4s_first_responce_sys_received = 1;
		memcpy(br4s_protocol.p_br4s_params, p_local_sys_state_resp, length);
		*br4s_protocol.p_br4s_state_valid = 1;
		br4s_protocol.p_br4s_callback((void *)&br4s_protocol, 
				NULL, NULL, START_DEV_POLL);
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t br4s_rem_get_timer_settings_from_frame(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_timer_pack_t * p_local_sys_state_resp;
	
	p_local_sys_state_resp = (br4s_rc_timer_pack_t *)p_context;
	
	if(length > sizeof(br4s_rc_timer_pack_t))
		return NRF_ERROR_NOT_FOUND;

	if (p_local_sys_state_resp->timer_id >= NUM_OF_TIMERS)
		return NRF_ERROR_NOT_FOUND;	

	if (br4s_protocol.p_br4s_params != NULL)
	{
		/*memcpy(&(br4s_protocol.p_br4s_params->timers[p_local_sys_state_resp->timer_id]), 
					&(p_local_sys_state_resp->timer), length);*/
                int timer_id = p_local_sys_state_resp->timer_id;
                if (timer_id == 0)
                  br4s_protocol.p_br4s_params->timers[0] = p_local_sys_state_resp->timer;
                else if (timer_id == 1)
                  br4s_protocol.p_br4s_params->timers[1] = p_local_sys_state_resp->timer;
                
		*br4s_protocol.p_br4s_state_valid = 1;
		br4s_protocol.p_br4s_callback((void *)&br4s_protocol, 
				NULL, NULL, START_DEV_POLL);
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t br4s_rem_get_time_from_frame(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_time_pack_t * p_local_sys_state_resp;
	
	p_local_sys_state_resp = (br4s_rc_time_pack_t *)p_context;
	
	if(length > sizeof(br4s_rc_time_pack_t))
		return NRF_ERROR_NOT_FOUND;

	if (br4s_protocol.p_br4s_params != NULL)
	{
		br4s_protocol.p_br4s_params->dev_time = unix_to_time(p_local_sys_state_resp->time.unix_time);

		*br4s_protocol.p_br4s_state_valid = 1;
		br4s_protocol.p_br4s_callback((void *)&br4s_protocol, 
				NULL, NULL, START_DEV_POLL);
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t create_set_com_template(void * p_context) 
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_set_cmd_t * p_local_br4s_frame;
	
	if(br4s_protocol.p_br4s_params != NULL)
	{
		p_local_br4s_frame = (br4s_rc_set_cmd_t *)p_context;

		p_local_br4s_frame->cmd_id = br4s_protocol.request_cnt;
			
		p_local_br4s_frame->flags.power_state = 
						br4s_protocol.p_br4s_params->dev_info.flags.power_state;
		p_local_br4s_frame->flags.sound_ind_state = 
						br4s_protocol.p_br4s_params->dev_info.flags.sound_ind_state;
		p_local_br4s_frame->flags.led_ind_state = 
						br4s_protocol.p_br4s_params->dev_info.flags.led_ind_state;
		p_local_br4s_frame->flags.heater_mode = 
						br4s_protocol.p_br4s_params->dev_info.flags.heater_mode;
		p_local_br4s_frame->flags.magic_air = 
						br4s_protocol.p_br4s_params->dev_info.flags.magic_air;
		p_local_br4s_frame->flags.ma_auto = 0;

		p_local_br4s_frame->flags.last_com_source = 1;
		p_local_br4s_frame->flags.error_reset = 0;
		p_local_br4s_frame->flags.factory_reset = 0; 
		p_local_br4s_frame->flags.filter_reset = 0;
		
		p_local_br4s_frame->params.substate = 
						br4s_protocol.p_br4s_params->dev_info.main_params.substate;
		p_local_br4s_frame->params.target_temp = 
						br4s_protocol.p_br4s_params->dev_info.main_params.target_temp;
		p_local_br4s_frame->params.fan_speed = 
						br4s_protocol.p_br4s_params->dev_info.main_params.fan_speed;

		p_local_br4s_frame->filter_time_in_days = 
					br4s_protocol.p_br4s_params->dev_info.counters.filter_time;

		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t concate_frame_meta_with_data(void * p_context, uint32_t frame_type)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static uint8_t b_local_buff[BLE_RX_BUFFER_SIZE];
	static ma_meta_br4s_frame_t b_local_meta = {0};
	static data_t * p_data;
	
	p_data = (data_t *)p_context;
	
	b_local_meta.data[0] = 0xAA;
	b_local_meta.data[1] = 0xBB;
	b_local_meta.frame_size = p_data->data_len + BR4S_META_SIZE + BR4S_CRC_SIZE;
	b_local_meta.frame_type = frame_type;
	b_local_meta.magic_number = BR4S_MAGIC_NUMBER;
	b_local_meta.random_number = (uint8_t)rand();
	b_local_meta.request_id = ++br4s_protocol.request_cnt;
	
	if(b_local_meta.frame_size <= BLE_RX_BUFFER_SIZE)
	{
		memcpy(b_local_buff, &b_local_meta, BR4S_META_SIZE);
		if(p_data->data_len != 0)
			memcpy(&b_local_buff[BR4S_META_SIZE], p_data->p_data,
				   p_data->data_len);
		memcpy(&b_local_buff[(BR4S_META_SIZE + p_data->data_len)],
			   b_local_meta.data, BR4S_CRC_SIZE);
		p_data->p_data = b_local_buff;
		p_data->data_len = b_local_meta.frame_size;
		
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_INVALID_LENGTH;
	}
	return ret_val;	
}

static uint32_t create_request_sys_param_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	p_input_frame->data_len = 0;
	p_input_frame->p_data = NULL;
	
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_DEV_STATUS_REQ);
	
	return ret_val;
}

static uint32_t create_request_dev_param_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	p_input_frame->data_len = 0;
	p_input_frame->p_data = NULL;
	
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_GET_MODE_REQ);
	
	return ret_val;
}

static uint32_t create_set_params_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_set_cmd_t b_local_br4s_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_br4s_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
		p_input_frame->data_len	= sizeof(br4s_rc_set_cmd_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_set_filter_time_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_set_cmd_t b_local_br4s_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_br4s_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		b_local_br4s_frame.flags.filter_reset = 1;
		CONVERT_SEC_TO_DAYS(b_local_br4s_frame.filter_time_in_days, 
					br4s_protocol.p_br4s_params->dev_info.counters.filter_time);
		p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
		p_input_frame->data_len	= sizeof(br4s_rc_set_cmd_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_reset_filter_time_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_set_cmd_t b_local_br4s_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_br4s_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		b_local_br4s_frame.flags.filter_reset = 1;
		p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
		p_input_frame->data_len	= sizeof(br4s_rc_set_cmd_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}


static uint32_t create_hard_reset_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_set_cmd_t b_local_br4s_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_br4s_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		b_local_br4s_frame.flags.factory_reset = 1;
		p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
		p_input_frame->data_len	= sizeof(br4s_rc_set_cmd_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_request_time_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	p_input_frame->data_len = 0;
	p_input_frame->p_data = NULL;
	
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_GET_TIME_REQ);
	
	return ret_val;
}

static uint32_t create_set_time_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_time_pack_t b_local_br4s_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	b_local_br4s_frame.cmd_id = br4s_protocol.request_cnt;
	b_local_br4s_frame.time.unix_time = time_to_unix(br4s_protocol.p_br4s_params->dev_time);

	p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
	p_input_frame->data_len	= sizeof(br4s_rc_time_pack_t);
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
									BR4S_FRAME_TYPE_SET_TIME_REQ);
	
	return ret_val;
}

static uint32_t create_request_on_timer_param_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_timer_set_req_pack_t b_local_br4s_frame_tim_on;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	b_local_br4s_frame_tim_on.timer_id = BR4S_ON_TIMER_ID;

	p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame_tim_on;
	p_input_frame->data_len	= sizeof(br4s_rc_timer_set_req_pack_t);
	
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_GET_TIMER_REQ);
	
	return ret_val;
}

static uint32_t create_request_off_timer_param_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_timer_set_req_pack_t b_local_br4s_frame_tim_off;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	b_local_br4s_frame_tim_off.timer_id = BR4S_OFF_TIMER_ID;

	p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame_tim_off;
	p_input_frame->data_len	= sizeof(br4s_rc_timer_set_req_pack_t);
	
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BR4S_FRAME_TYPE_GET_TIMER_REQ);
	
	return ret_val;
}

static uint32_t create_set_on_timer_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_timer_pack_t b_local_br4s_frame;
	static data_t * p_timer_input_frame;

	if (p_context == NULL)
	  return NRF_ERROR_NO_MEM;
	
	p_timer_input_frame = (data_t *)p_context;	
	
	b_local_br4s_frame.cmd_id = br4s_protocol.request_cnt;
	b_local_br4s_frame.timer_id = BR4S_ON_TIMER_ID;
	b_local_br4s_frame.timer = br4s_protocol.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND];

	b_local_br4s_frame.timer.timer_time.Monday = 1;
	b_local_br4s_frame.timer.timer_time.Tuesday = 1;
	b_local_br4s_frame.timer.timer_time.Wednesday = 1;
	b_local_br4s_frame.timer.timer_time.Thursday = 1;
	b_local_br4s_frame.timer.timer_time.Friday = 1;
	b_local_br4s_frame.timer.timer_time.Saturday = 1;
	b_local_br4s_frame.timer.timer_time.Sunday = 1;

	b_local_br4s_frame.timer.timer_flags.timer_state = 1;
	b_local_br4s_frame.timer.timer_flags.power_state = 1;
	/***************************/
	
	p_timer_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
	p_timer_input_frame->data_len	= sizeof(br4s_rc_timer_pack_t);
	ret_val = concate_frame_meta_with_data((void *)p_timer_input_frame,
									BR4S_FRAME_TYPE_SET_TIMER_REQ);

	return ret_val;
}

static uint32_t create_set_off_timer_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_timer_pack_t b_local_br4s_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	b_local_br4s_frame.cmd_id = br4s_protocol.request_cnt;
	b_local_br4s_frame.timer_id = BR4S_OFF_TIMER_ID;
	b_local_br4s_frame.timer = br4s_protocol.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND];

	/*Timer settings set block */
	b_local_br4s_frame.timer.timer_time.Monday = 1;
	b_local_br4s_frame.timer.timer_time.Tuesday = 1;
	b_local_br4s_frame.timer.timer_time.Wednesday = 1;
	b_local_br4s_frame.timer.timer_time.Thursday = 1;
	b_local_br4s_frame.timer.timer_time.Friday = 1;
	b_local_br4s_frame.timer.timer_time.Saturday = 1;
	b_local_br4s_frame.timer.timer_time.Sunday = 1;

	b_local_br4s_frame.timer.timer_flags.timer_state = 1;
	b_local_br4s_frame.timer.timer_flags.power_state = 0;
	/***************************/

	p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
	p_input_frame->data_len	= sizeof(br4s_rc_timer_pack_t);
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
									BR4S_FRAME_TYPE_SET_TIMER_REQ);

	return ret_val;
}

static uint32_t create_set_on_timer_off_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	br4s_rc_timer_pack_t b_local_br4s_frame;
	data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	b_local_br4s_frame.cmd_id = br4s_protocol.request_cnt;
	b_local_br4s_frame.timer_id = BR4S_ON_TIMER_ID;
	b_local_br4s_frame.timer = br4s_protocol.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND];

	/*Timer settings set block */
	b_local_br4s_frame.timer.timer_flags.timer_state = 0;
	/***************************/

	p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
	p_input_frame->data_len	= sizeof(br4s_rc_timer_pack_t);
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
									BR4S_FRAME_TYPE_SET_TIMER_REQ);

	return ret_val;
}

static uint32_t create_set_off_timer_off_com (void* p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static br4s_rc_timer_pack_t b_local_br4s_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	b_local_br4s_frame.cmd_id = br4s_protocol.request_cnt;
	b_local_br4s_frame.timer_id = BR4S_OFF_TIMER_ID;
	b_local_br4s_frame.timer = br4s_protocol.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND];

	/*Timer settings set block */
	b_local_br4s_frame.timer.timer_flags.timer_state = 0;
	/***************************/

	p_input_frame->p_data = (uint8_t *)&b_local_br4s_frame;
	p_input_frame->data_len	= sizeof(br4s_rc_timer_pack_t);
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
									BR4S_FRAME_TYPE_SET_TIMER_REQ);

	return ret_val;
}

int64_t time_to_unix (rtc_time_format_t time)
{
    unsigned int mon = time.month, year = time.year;
    /* Взял в интернете в одной из стандартных либ. 
    Че за цифры и ветвление - я хз, но работает. В условии проверяется 
    "а не февраль ли сейчас или январь". Если да, то магия. */

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int) (mon -= 2)) {
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

    /* 719499 - количество дней до 1970 года.
       year/4 - year/100 + year/400 - расчет количества високосных 
       годов до текущего года, соответственно и количество дополнительных дней.
       Почему 367*mon/12 - хз. Дальше всё очевидно. */
        year += 2000; /* Поскольку в RTC хранится только десятичная часть, 
                          для корректного перевода прибавляем 2000 */
	return ((((int64_t)
		  (year/4 - year/100 + year/400 + 367*mon/12 + time.day) +
		  year*365 - DAYS_BEFORE_1970
	    )*24 + time.hour /* now have hours - midnight tomorrow handled here */
	  )*60 + time.min /* now have minutes */
	)*60 + time.sec; /* finally seconds */
}

rtc_time_format_t unix_to_time (int64_t unix_time)
{
    rtc_time_format_t b_time = {0};
    uint32_t aCoef,bCoef,cCoef,dCoef,eCoef,mCoef,jd,jdn;
	
	jd = ((unix_time + 43200)/(86400>>1)) + (2440587<<1) + 1;
	jdn = jd>>1;
	
	b_time.sec = unix_time%60;
	unix_time /= 60;
	b_time.min = unix_time%60;
	unix_time /= 60;
	b_time.hour = unix_time%24;
	unix_time /= 24;
	
	b_time.weekday = jdn%7 + 1;
	
	aCoef = jdn + 32044;
	bCoef = ((4 * aCoef) + 3) / 146097;
	cCoef = aCoef - (146097 * bCoef) / 4;
	dCoef = ((4 * cCoef) + 3) / 1461;
	eCoef = cCoef - (1461 * dCoef) / 4;
	mCoef = ((5 * eCoef) + 2) / 153;
	
	b_time.day = eCoef - ((153 * mCoef) + 2)/5 +1;
	b_time.month = mCoef + 3 - (12 * (mCoef / 10));
	b_time.year = ((100 * bCoef) + dCoef - 4800 + (mCoef / 10)-2000);

    return b_time;
}


/*Public functions*/
ble_adv_data_parser_t br4s_get_adv_parser (void)
{
	return &adv_data_parser;
}


/******************************* END_OF_FILE **********************************/