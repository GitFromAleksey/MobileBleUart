/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "brlt_main.h"
#include "device_interface.h"
/*Private defines*/
// clang-format off
#define BLE_RX_BUFFER_SIZE          96
#define BRLT_SYSTEM_TYPE			((uint16_t)0x8002)
#define BRLT_SYSTEM_STYPE			((uint16_t)0x0000)
// clang-format on
/*Private types*/
typedef uint32_t (*create_com_t)(void * p_context, uint32_t length);

/*Private functions prototypes*/
static uint32_t init_protocol_instance(void * p_context);
static uint32_t deinit_protocol_instance(void * p_context);
static uint32_t input_data_handler(void * p_context, uint32_t length);
static uint32_t output_data_handler(void * p_context, uint32_t length);
static uint32_t input_raw_data_handler(void * p_context, uint32_t length);
static uint32_t output_raw_data_handler(void * p_context, uint32_t length);

static uint32_t create_request_sys_state_com(void * p_context, uint32_t length);
static uint32_t create_request_dev_state_com(void * p_context, uint32_t length);
static uint32_t create_set_params_com(void * p_context, uint32_t length);
static uint32_t create_set_filter_time_com(void * p_context, uint32_t length);
static uint32_t create_reset_filter_time_com(void * p_context, uint32_t length);
static uint32_t create_hard_reset_com(void * p_context, uint32_t length);
static uint32_t create_set_service_mode_com(void * p_context, uint32_t length);
static uint32_t create_set_autokiv_params_com(void * p_context, uint32_t length);
static uint32_t create_get_autokiv_params_com(void * p_context, uint32_t length);
static uint32_t create_kiv_set_com_template(void * p_context);

static uint32_t create_set_com_template(void * p_context);
static uint32_t prepare_frame_to_send(void * p_context, uint32_t length);
static uint32_t adv_data_parser(uint32_t pair_mode,
				const ble_gap_evt_adv_report_t * p_adv_data);
/*static bool is_ma_device(const uint16_t type,const uint16_t stype, bool pairing,
                            const ble_gap_evt_adv_report_t * p_adv_report);*/
static uint32_t adv_report_parse(uint8_t type, uint8_array_t * p_advdata, 
								 uint8_array_t * p_typedata);

/*Exported constants*/
const protocol_m_list_t	brlt_protocol_vptr = {
	.init 				= &init_protocol_instance,
	.deinit 			= &deinit_protocol_instance,
	.input_data 		= &input_data_handler,
	.output_data 		= &output_data_handler,
	.input_raw_data 	= &input_raw_data_handler,
	.output_raw_data 	= &output_raw_data_handler,
};

static brlt_protocol_t brlt_protocol;

static const create_com_t create_com[BRLT_COM_COUNT] = {
	NULL,
	&create_request_sys_state_com,
	&create_request_dev_state_com,
	&create_set_params_com,
	&create_set_filter_time_com,
	&create_reset_filter_time_com,
	&create_hard_reset_com,
	&create_set_service_mode_com,
	&create_set_autokiv_params_com,
	&create_get_autokiv_params_com,
};
/*Private variables*/
NRF_QUEUE_DEF(ble_raw_queue_frame_t, brlt_raw_input,
			  RAW_INPUT_QUEUE_SIZE, NRF_QUEUE_MODE_NO_OVERFLOW);
NRF_QUEUE_DEF(ble_raw_queue_frame_t, brlt_output,
			  RAW_INPUT_QUEUE_SIZE, NRF_QUEUE_MODE_NO_OVERFLOW);
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/

/*Private functions*/
static uint32_t adv_data_parser(uint32_t pair_mode,
				const ble_gap_evt_adv_report_t * p_adv_data)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	brlt_ble_adv_dev_info_t * p_local_adv_data;
	uint8_array_t b_local_raw_adv_data = {0};
	uint8_array_t b_local_adv_data = {0};
	
	if ((p_adv_data->data[14] == 0x80) &&
	    (p_adv_data->data[13] == 0x02))
	{
		b_local_raw_adv_data.p_data = (uint8_t *)p_adv_data->data;
		b_local_raw_adv_data.size = p_adv_data->dlen;
		
		if(NRF_SUCCESS == adv_report_parse(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 
						&b_local_raw_adv_data, &b_local_adv_data))
		{
			p_local_adv_data = (brlt_ble_adv_dev_info_t *)b_local_adv_data.p_data;
			if(p_local_adv_data->dev_type == BRLT_SYSTEM_TYPE && p_local_adv_data->dev_stype == BRLT_SYSTEM_STYPE
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

static uint32_t create_set_com_template(void * p_context) 
{
	ret_code_t ret_val = NRF_SUCCESS;
	static dev_state_set_frame_t * p_local_brlt_frame;
	
	if(brlt_protocol.p_brlt_params != NULL)
	{
		p_local_brlt_frame = (dev_state_set_frame_t *)p_context;

		p_local_brlt_frame->cmd_id = brlt_protocol.request_cnt;
			
		p_local_brlt_frame->set_flags.auto_co2_mode = 
			brlt_protocol.p_brlt_params->dev_info.flags.auto_co2_mode;
		p_local_brlt_frame->set_flags.heater_mode = 
			brlt_protocol.p_brlt_params->dev_info.flags.heater_mode;
		p_local_brlt_frame->set_flags.led_mode = 
			brlt_protocol.p_brlt_params->dev_info.flags.led_mode;
		p_local_brlt_frame->set_flags.power_mode = 
			brlt_protocol.p_brlt_params->dev_info.flags.power_mode;
		p_local_brlt_frame->set_flags.snd_mode = 
			brlt_protocol.p_brlt_params->dev_info.flags.snd_mode;
		p_local_brlt_frame->set_flags.kiv_active = 
			brlt_protocol.p_brlt_params->dev_info.flags.kiv_active;

		p_local_brlt_frame->set_flags.last_com_source = 1;
		p_local_brlt_frame->set_flags.error_reset = 0;
		p_local_brlt_frame->set_flags.factory_reset = 0; 
		p_local_brlt_frame->set_flags.filter_reset = 0;
		
		p_local_brlt_frame->set_misc.filter_time = 0;
		p_local_brlt_frame->set_misc.test_type = 0;
		
		memcpy(&p_local_brlt_frame->set_presets, 
			   &brlt_protocol.p_brlt_params->dev_info.presets,
			   sizeof(brlt_dev_preset_t));
		memcpy(&p_local_brlt_frame->set_user_set, 
			   &brlt_protocol.p_brlt_params->dev_info.user_set,
			   sizeof(brlt_dev_user_set_t));
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t create_kiv_set_com_template(void * p_context) 
{
	ret_code_t ret_val = NRF_SUCCESS;
	static autokiv_state_resp_frame_t * p_local_brlt_frame;
	
	if(brlt_protocol.p_brlt_params != NULL)
	{
		p_local_brlt_frame = (autokiv_state_resp_frame_t *)p_context;

		p_local_brlt_frame->cmd_id = brlt_protocol.request_cnt;

		memcpy(&(p_local_brlt_frame->autokiv_info), 
			   &brlt_protocol.p_brlt_params->dev_info.autokiv_info,
			   sizeof(autokiv_state_resp_frame_t));
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t prepare_frame_to_send(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static ble_raw_queue_frame_t	ble_raw_frame;
	static uint8_t * p_local_brlt_frame;
	static uint8_t * p_local_current_buff;
	
	p_local_brlt_frame = (uint8_t *)p_context;
	p_local_current_buff = p_local_brlt_frame;
	if(length > (BLE_NUS_MAX_DATA_LEN - 1))
	{
		while(p_local_current_buff != (p_local_brlt_frame + length))
		{
			if(p_local_current_buff == p_local_brlt_frame)
			{
				ble_raw_frame.data_len = BLE_NUS_MAX_DATA_LEN;
				ble_raw_frame.data.packet_type = BLE_PACKET_TYPE_FIRST;
				memcpy(ble_raw_frame.data.data, p_local_current_buff, ble_raw_frame.data_len);
				ret_val = nrf_queue_push(&brlt_output, (const void *)&ble_raw_frame);
				p_local_current_buff = p_local_brlt_frame + BLE_NUS_MAX_DATA_LEN - 1;
			}
			else if(p_local_current_buff <= 
					(p_local_brlt_frame + length - BLE_NUS_MAX_DATA_LEN - 1))
			{
				ble_raw_frame.data_len = BLE_NUS_MAX_DATA_LEN;
				ble_raw_frame.data.packet_type = BLE_PACKET_TYPE_CURRENT;
				memcpy(ble_raw_frame.data.data, p_local_current_buff, ble_raw_frame.data_len);
				ret_val = nrf_queue_push(&brlt_output, (const void *)&ble_raw_frame);
				p_local_current_buff += (BLE_NUS_MAX_DATA_LEN - 1);
			}
			else
			{
				ble_raw_frame.data_len = ((p_local_brlt_frame + length) - p_local_current_buff)+1;
				ble_raw_frame.data.packet_type = BLE_PACKET_TYPE_END;
				memcpy(ble_raw_frame.data.data, p_local_current_buff, ble_raw_frame.data_len);
				ret_val = nrf_queue_push(&brlt_output, (const void *)&ble_raw_frame);
				p_local_current_buff = p_local_brlt_frame + length;
			}
		}
	}
	else
	{
		ble_raw_frame.data_len = length + 1;
		ble_raw_frame.data.packet_type = BLE_PACKET_TYPE_FEND;
		memcpy(ble_raw_frame.data.data, p_local_brlt_frame, ble_raw_frame.data_len);
		ret_val = nrf_queue_push(&brlt_output, (const void *)&ble_raw_frame);
	}
	return ret_val;
}

static uint32_t concate_frame_meta_with_data(void * p_context, uint32_t frame_type)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static uint8_t b_local_buff[BLE_RX_BUFFER_SIZE];
	static ma_meta_ble_frame_t b_local_meta = {0};
	static data_t * p_data;
	
	p_data = (data_t *)p_context;
	
	b_local_meta.data[0] = 0xAA;
	b_local_meta.data[1] = 0xBB;
	b_local_meta.frame_size = p_data->data_len + BLE_META_SIZE + BLE_CRC_SIZE;
	b_local_meta.frame_type = frame_type;
	b_local_meta.magic_number = BLE_MAGIC_NUMBER;
	b_local_meta.random_number = (uint8_t)rand();
	b_local_meta.request_id = ++brlt_protocol.request_cnt;
	
	if(b_local_meta.frame_size <= BLE_RX_BUFFER_SIZE)
	{
		memcpy(b_local_buff, &b_local_meta, BLE_META_SIZE);
		if(p_data->data_len != 0)
			memcpy(&b_local_buff[BLE_META_SIZE], p_data->p_data,
				   p_data->data_len);
		memcpy(&b_local_buff[(BLE_META_SIZE + p_data->data_len)],
			   b_local_meta.data, BLE_CRC_SIZE);
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

static uint32_t create_request_sys_state_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	p_input_frame->data_len = 0;
	p_input_frame->p_data = NULL;
	
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_DEV_STATUS_REQ);
	
	return ret_val;
}

static uint32_t create_request_dev_state_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	p_input_frame->data_len = 0;
	p_input_frame->p_data = NULL;
	
	ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_GET_MODE_REQ);
	
	return ret_val;
}

static uint32_t create_set_params_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static dev_state_set_frame_t b_local_brlt_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_brlt_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		p_input_frame->p_data = (uint8_t *)&b_local_brlt_frame;
		p_input_frame->data_len	= sizeof(dev_state_set_frame_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_set_filter_time_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static dev_state_set_frame_t b_local_brlt_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_brlt_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		b_local_brlt_frame.set_flags.filter_reset = 1;
		CONVERT_SEC_TO_DAYS(b_local_brlt_frame.set_misc.filter_time, 
					brlt_protocol.p_brlt_params->dev_info.counters.filter_time);
		p_input_frame->p_data = (uint8_t *)&b_local_brlt_frame;
		p_input_frame->data_len	= sizeof(dev_state_set_frame_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_reset_filter_time_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static dev_state_set_frame_t b_local_brlt_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_brlt_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		b_local_brlt_frame.set_flags.filter_reset= 1;
		p_input_frame->p_data = (uint8_t *)&b_local_brlt_frame;
		p_input_frame->data_len	= sizeof(dev_state_set_frame_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_hard_reset_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static dev_state_set_frame_t b_local_brlt_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_brlt_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		b_local_brlt_frame.set_flags.factory_reset = 1;
		p_input_frame->p_data = (uint8_t *)&b_local_brlt_frame;
		p_input_frame->data_len	= sizeof(dev_state_set_frame_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_set_service_mode_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static dev_state_set_frame_t b_local_brlt_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_set_com_template((void *)&b_local_brlt_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		b_local_brlt_frame.set_user_set.target_temp = 40;
		p_input_frame->p_data = (uint8_t *)&b_local_brlt_frame;
		p_input_frame->data_len	= sizeof(dev_state_set_frame_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ);
	}
	return ret_val;
}

static uint32_t create_set_autokiv_params_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static autokiv_state_resp_frame_t b_local_autokiv_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_kiv_set_com_template((void *)&b_local_autokiv_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		p_input_frame->p_data = (uint8_t *)&(b_local_autokiv_frame);
		p_input_frame->data_len	= sizeof(autokiv_state_resp_frame_t);
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_SET_AUTOKIV_PARAM_REQ);
	}
	return ret_val;
}

static uint32_t create_get_autokiv_params_com(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static autokiv_state_resp_frame_t b_local_brlt_frame;
	static data_t * p_input_frame;
	
	p_input_frame = (data_t *)p_context;
	
	ret_val = create_kiv_set_com_template((void *)&b_local_brlt_frame); 
	if(ret_val == NRF_SUCCESS)
	{
		p_input_frame->p_data = NULL;
		p_input_frame->data_len	= 0;
		ret_val = concate_frame_meta_with_data((void *)p_input_frame,
										BLE_FRAME_TYPE_GET_AUTOKIV_PARAM_REQ);
	}
	return ret_val;
}

static uint32_t update_brlt_sys_state_from_resp(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static sys_state_resp_frame_t * p_local_sys_state_resp;
	
	p_local_sys_state_resp = (sys_state_resp_frame_t *)p_context;
	
	if (brlt_protocol.p_brlt_params != NULL)
	{
		if (!brlt_protocol.p_brlt_params->brlt_first_responce_sys_received)
			brlt_protocol.p_brlt_params->brlt_first_responce_sys_received = 1;
		memcpy(brlt_protocol.p_brlt_params, p_local_sys_state_resp, length);
		*brlt_protocol.p_brlt_state_valid = 1;
		brlt_protocol.p_brlt_callback((void *)&brlt_protocol, 
				NULL, NULL, START_DEV_POLL);
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t update_brlt_dev_state_from_resp(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static dev_state_resp_frame_t * p_local_dev_state_resp;
	
	p_local_dev_state_resp = (dev_state_resp_frame_t *)p_context;
	
	if (brlt_protocol.p_brlt_params != NULL)
	{
		if (!brlt_protocol.p_brlt_params->brlt_first_responce_dev_received)
		{
			brlt_protocol.p_brlt_params->brlt_first_responce_dev_received = 1;
			brlt_protocol.p_brlt_callback((void *)&brlt_protocol, NULL, NULL, 
							BUTTON_EVT_BLOCK_OVF);
		}
		memcpy(&brlt_protocol.p_brlt_params->dev_info, 
			   &p_local_dev_state_resp->dev_info, (length-4));
		*brlt_protocol.p_brlt_state_valid = 1;
		brlt_protocol.p_brlt_callback((void *)&brlt_protocol, 
				NULL, NULL, START_DEV_POLL);
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t update_brlt_autokiv_state_from_resp(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static autokiv_state_resp_frame_t * p_local_dev_state_resp;
	
	p_local_dev_state_resp = (autokiv_state_resp_frame_t *)p_context;
	
	if (brlt_protocol.p_brlt_params != NULL)
	{
		if (!brlt_protocol.p_brlt_params->brlt_first_responce_dev_received)
		{
			brlt_protocol.p_brlt_params->brlt_first_responce_dev_received = 1;
			brlt_protocol.p_brlt_callback((void *)&brlt_protocol, NULL, NULL, 
			BUTTON_EVT_BLOCK_OVF);
		}
		memcpy(&brlt_protocol.p_brlt_params->dev_info.autokiv_info, 
			   &p_local_dev_state_resp->autokiv_info, (length-4));
		*brlt_protocol.p_brlt_state_valid = 1;
		brlt_protocol.p_brlt_callback((void *)&brlt_protocol, 
				NULL, NULL, START_DEV_POLL);
		ret_val = NRF_SUCCESS;
	}
	else
	{
		ret_val = NRF_ERROR_NOT_FOUND;
	}
	return ret_val;
}

static uint32_t output_frame_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static ma_meta_ble_frame_t b_local_frame_meta;
	static uint8_t * p_local_frame_data;
	uint8_t * p_local_crc;
	p_local_crc = (uint8_t *)p_context + length - BLE_CRC_SIZE;
	memcpy(&b_local_frame_meta, p_context, 
		   (sizeof(ma_meta_ble_frame_t) - BLE_CRC_SIZE));
	memcpy(b_local_frame_meta.data, 
		   p_local_crc, 
		   BLE_CRC_SIZE);
	p_local_frame_data = (uint8_t *)p_context + BLE_META_SIZE;
	if(b_local_frame_meta.frame_size == length)
	{
		switch(b_local_frame_meta.frame_type)
		{
			case BLE_FRAME_TYPE_DEV_STATUS_RESP:
			{
				ret_val = update_brlt_sys_state_from_resp(p_local_frame_data,
									(length - BLE_META_SIZE - BLE_CRC_SIZE));
			}	break;
			case BLE_FRAME_TYPE_GET_MODE_RESP:
			{
				ret_val = update_brlt_dev_state_from_resp(p_local_frame_data,
									(length - BLE_META_SIZE- BLE_CRC_SIZE));
			}	break;
			case BLE_FRAME_TYPE_GET_AUTOKIV_PARAM_RESP:
				ret_val = update_brlt_autokiv_state_from_resp(p_local_frame_data,
									(length - BLE_META_SIZE- BLE_CRC_SIZE));
				break;
			default:
			{
				ret_val = NRF_ERROR_INVALID_DATA;
			}	break;
		}
	}
	else
	{
		ret_val = NRF_ERROR_INVALID_LENGTH;
	}
	return ret_val;
}


/*class interface section*/

static uint32_t init_protocol_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	brlt_main_t * p_local_brlt_main;
	
	p_local_brlt_main = (brlt_main_t *)p_context;
	
	brlt_protocol.p_brlt_params 		= &p_local_brlt_main->brlt_state;
	brlt_protocol.p_brc_main			= p_local_brlt_main->p_brc_main;
	brlt_protocol.p_brlt_state_valid	= &p_local_brlt_main->brlt_state_valid;
	brlt_protocol.p_brlt_callback		= p_local_brlt_main->p_brlt_callback;
	brlt_protocol.p_brlt_params->brlt_invalid_speed = 0;
	return ret_val;
}

static uint32_t deinit_protocol_instance(void * p_context)
{
	brlt_protocol.p_brlt_params 		= NULL;
	brlt_protocol.p_brc_main			= NULL;
	brlt_protocol.p_brlt_state_valid	= NULL;
	brlt_protocol.p_brlt_callback		= NULL;
	
	nrf_queue_reset(&brlt_raw_input);
	nrf_queue_reset(&brlt_output);
	return NRF_SUCCESS;
}

static uint32_t input_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val;
	uint8_t com_type;
	static data_t b_frame;
	
	com_type = *(uint8_t *)p_context;
	ret_val = create_com[com_type]((void *)&b_frame, NULL);
	if(ret_val == NRF_SUCCESS)
	{
		ret_val = prepare_frame_to_send((void *)b_frame.p_data, b_frame.data_len);
	}
	return ret_val;
}

static uint32_t output_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val;
	static uint8_t b_local_data[BLE_RX_BUFFER_SIZE];  
	static uint8_t *p_local_data = b_local_data;
	static uint8_t b_prev_packet_type = BLE_PACKET_TYPE_NONE;
	static ble_raw_queue_frame_t b_buffer_frame;

	if(nrf_queue_peek(&brlt_raw_input, (void *)&b_buffer_frame) 
	   == NRF_ERROR_NOT_FOUND)
		return NRF_ERROR_NOT_FOUND;
	
	ret_val = nrf_queue_pop(&brlt_raw_input, (void *)&b_buffer_frame);
	
	if(ret_val == NRF_SUCCESS)
	{
		switch (b_buffer_frame.data.packet_type)
		{
			case BLE_PACKET_TYPE_FIRST:
			{
				if (b_buffer_frame.data_len <= BLE_RX_BUFFER_SIZE)
				{
					memcpy(b_local_data, b_buffer_frame.data.data, 
						   b_buffer_frame.data_len);
					p_local_data = b_local_data + b_buffer_frame.data_len;
					b_prev_packet_type = BLE_PACKET_TYPE_FIRST;
					ret_val = NRF_SUCCESS;
				}
				else
						ret_val = NRF_ERROR_INVALID_LENGTH;
			}	break;
			case BLE_PACKET_TYPE_CURRENT:
			{
				if ((b_prev_packet_type == BLE_PACKET_TYPE_FIRST)||
				    (b_prev_packet_type == BLE_PACKET_TYPE_CURRENT))
				{
					if (b_buffer_frame.data_len <= (BLE_RX_BUFFER_SIZE - 
										(p_local_data - b_local_data)))
					{
						memcpy(p_local_data, b_buffer_frame.data.data, 
							   b_buffer_frame.data_len);
						p_local_data += b_buffer_frame.data_len;
						b_prev_packet_type = BLE_PACKET_TYPE_CURRENT;
						ret_val = NRF_SUCCESS;
					}
					else
						ret_val = NRF_ERROR_INVALID_LENGTH;
				}
				else
					ret_val = NRF_ERROR_INVALID_DATA;
			}	break;
			case BLE_PACKET_TYPE_FEND:
			{
				memcpy(b_local_data, b_buffer_frame.data.data, 
						   b_buffer_frame.data_len);
				//p_local_data = b_local_data + b_buffer_frame.data_len;
				ret_val = output_frame_handler((void *)&b_local_data, 
												b_buffer_frame.data_len);
				p_local_data = b_local_data;
				b_prev_packet_type = BLE_PACKET_TYPE_FEND;
			}	break;
			case BLE_PACKET_TYPE_END:
			{
				if ((b_prev_packet_type == BLE_PACKET_TYPE_FIRST)||
				    (b_prev_packet_type == BLE_PACKET_TYPE_CURRENT))
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
						b_prev_packet_type = BLE_PACKET_TYPE_END;
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
	static ble_raw_queue_frame_t b_buffer_frame;
	static uint8_t * p_local_raw_data;
	
	p_local_raw_data = (uint8_t *)p_context;
	length--;
	b_buffer_frame.data_len = length;
	b_buffer_frame.data.packet_type = p_local_raw_data[0];
	memcpy(b_buffer_frame.data.data, &p_local_raw_data[1], length);
	
	ret_val = nrf_queue_push(&brlt_raw_input, (const void *)&b_buffer_frame);
	
	return ret_val;
}

static uint32_t output_raw_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val;
	static ble_raw_queue_frame_t 	b_local_raw_data;
	static data_t * p_input_raw_data;
	
	p_input_raw_data = (data_t *)p_context;
	
	if(nrf_queue_peek(&brlt_output, (void *)&b_local_raw_data) 
	   == NRF_ERROR_NOT_FOUND)
		return NRF_ERROR_NOT_FOUND;
	
	ret_val = nrf_queue_pop(&brlt_output, (void *)&b_local_raw_data);
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

/*Public functions*/
ble_adv_data_parser_t brlt_get_adv_parser (void)
{
	return &adv_data_parser;
}
/******************************* END_OF_FILE **********************************/