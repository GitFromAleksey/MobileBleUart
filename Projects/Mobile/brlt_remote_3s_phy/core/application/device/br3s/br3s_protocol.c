/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "br3s_main.h"
/*Private defines*/
// clang-format off
#define PERIPHERAL_NAME 			"Tion Breezer 3S"
#define SHORT_PERIPHERAL_NAME		"Tion "

#define ADAPTER_ID 					0x3D
#define BREEZER_CUB_ID				0xB3
#define FRAME_SIZE					20
#define CALCULATE_CRC				0x5A

#define SEND_REQ_COM_ID       		0x01
#define SEND_REQ_RESP_ID       		0x10

#define SEND_SET_PARAM_COM_ID		0x02
#define SEND_SET_PARAM_RESP_ID		0x20

#define SEND_SET_TIME_COM_ID       	0x03
#define SEND_SET_TIME_RESP_ID		0x30

#define SEND_SET_ALARM_COM_ID		0x04
#define SEND_SET_ALARM_RESP_ID		0x40

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

static uint32_t create_request_com(void * p_context);
static uint32_t create_set_params_com(void * p_context);
static uint32_t create_set_filter_time_com(void * p_context);
static uint32_t create_reset_filter_time_com(void * p_context);
static uint32_t create_set_service_mode_com(void * p_context);
static uint32_t create_hard_reset_com(void * p_context);
static uint32_t create_start_ma_pairing_com(void * p_context);
static uint32_t create_set_time_com(void * p_context);
static uint32_t create_alarm_request_com(void * p_context);
static uint32_t create_set_alarm_on_com(void * p_context);
static uint32_t create_set_alarm_off_com(void * p_context);

static uint32_t update_br3s_params_from_responce(br3s_get_param_t * p_data);
static uint32_t update_br3s_alarms_from_responce(br3s_alarm_response_t * p_data);
static uint32_t update_br3s_params_from_responce_on_set(br3s_get_param_t * p_data);

static uint32_t adv_data_parser(uint32_t pair_mode,
				const ble_gap_evt_adv_report_t * p_adv_data);
static bool is_name_present(const char *target_name,
							const ble_gap_evt_adv_report_t *p_adv_report);
static bool is_device_connectable(const ble_gap_evt_adv_report_t *p_adv_report);
static bool is_device_in_pair_mode(const ble_gap_evt_adv_report_t *p_adv_report);
/*Exported constants*/
const protocol_m_list_t	br3s_protocol_vptr = {
	.init 				= &init_protocol_instance,
	.deinit 			= &deinit_protocol_instance,
	.input_data 		= &input_data_handler,
	.output_data 		= &output_data_handler,
	.input_raw_data 	= &input_raw_data_handler,
	.output_raw_data 	= &output_raw_data_handler,
};

static br3s_protocol_t br3s_protocol = {0};

static const create_com_t create_com[BR3S_COM_COUNT] = {
	NULL,
	&create_request_com,
	&create_set_params_com,
	&create_set_filter_time_com,
	&create_reset_filter_time_com,
	&create_set_service_mode_com,
	&create_hard_reset_com,
	&create_start_ma_pairing_com,
	&create_set_time_com,
	&create_alarm_request_com,
	&create_set_alarm_on_com,
	&create_set_alarm_off_com,
};

/*Private variables*/

NRF_QUEUE_DEF(br3s_frame_t, br3s_raw_input,
			  BR3S_RAW_INPUT_QUEUE_SIZE, NRF_QUEUE_MODE_NO_OVERFLOW);
NRF_QUEUE_DEF(br3s_output_com_t, br3s_output,
			  BR3S_RAW_INPUT_QUEUE_SIZE, NRF_QUEUE_MODE_NO_OVERFLOW);
/*Private macro*/
// clang-format off
// clang-format on
/*Private constants*/

/*Private functions*/
static bool is_name_present(const char *target_name,
							const ble_gap_evt_adv_report_t *p_adv_report)
{
	uint32_t index  = 0;
	uint8_t *p_data = (uint8_t *)p_adv_report->data;
	char *name;
	while (index < p_adv_report->dlen)
	{
		uint8_t field_length = p_data[index];
		uint8_t field_type   = p_data[index + 1];

		if (field_type == BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME)
		{
			name				   = (char *)&p_data[index + 2];
			name[field_length - 1] = '\0';

			if (strcmp(target_name, name) == 0)
			{
				return true;
			}
		}
		index += field_length + 1;
	}
	return false;
}

static uint32_t adv_data_parser(uint32_t pair_mode,
				const ble_gap_evt_adv_report_t * p_adv_data)
{
	uint32_t ret_val = NRF_SUCCESS;

	if (is_name_present(PERIPHERAL_NAME,p_adv_data))
	{
		if (is_device_in_pair_mode(p_adv_data) == (bool)pair_mode)
		{
			if(is_device_connectable(p_adv_data))
			{
				ret_val = 1;
			}
			else
			{
				ret_val = 0;
			}
		}
	}
	return ret_val;
}

static bool is_device_connectable(const ble_gap_evt_adv_report_t *p_adv_report)
{
	if (p_adv_report->type == BLE_GAP_ADV_TYPE_ADV_NONCONN_IND) 
	{
		return false;
	}
	return true;
}

static bool is_device_in_pair_mode(const ble_gap_evt_adv_report_t *p_adv_report)
{
	if(p_adv_report->data[2] == 0x06)
	{
		return true;
	}
	return false;
}

static uint32_t update_br3s_params_from_responce(br3s_get_param_t * p_data)
{
	if(br3s_protocol.p_br3s_params != NULL)
	{
		if (br3s_protocol.p_br3s_params->br3s_first_responce_received == 1)
		{
			
		}
		else
		{
			br3s_protocol.p_br3s_params->br3s_first_responce_received = 1;
			br3s_protocol.p_br3s_callback((void *)&br3s_protocol, 
					NULL, NULL, BUTTON_EVT_BLOCK_OVF);
		}
		
		memcpy(&br3s_protocol.p_br3s_params->dev_info, p_data, 
			   sizeof(br3s_get_param_t));
		*br3s_protocol.p_br3s_state_valid = 1;
		br3s_protocol.p_br3s_callback((void *)&br3s_protocol, 
				NULL, NULL, START_DEV_POLL);
		
		return NRF_SUCCESS;
	}
	else
	{
		return NRF_ERROR_NOT_FOUND;
	}
}

static uint32_t update_br3s_params_from_responce_on_set(br3s_get_param_t * p_data)
{
	if(br3s_protocol.p_br3s_params != NULL)
	{
		if (br3s_protocol.p_br3s_params->br3s_first_responce_received == 1)
		{
			if (p_data->fan_speed != br3s_protocol.p_br3s_params->dev_info.fan_speed)
			{
				br3s_protocol.p_br3s_params->br3s_invalid_speed = 1;
			}
			else
			{
				br3s_protocol.p_br3s_params->br3s_invalid_speed = 2;
			}
		}
		else
		{
			br3s_protocol.p_br3s_params->br3s_first_responce_received = 1;
			br3s_protocol.p_br3s_callback((void *)&br3s_protocol, NULL, NULL, 
				BUTTON_EVT_BLOCK_OVF);
		}
		
		memcpy(&br3s_protocol.p_br3s_params->dev_info, p_data, 
			   sizeof(br3s_get_param_t));
		*br3s_protocol.p_br3s_state_valid = 1;
		br3s_protocol.p_br3s_callback((void *)&br3s_protocol, 
				NULL, NULL, START_DEV_POLL);
		
		return NRF_SUCCESS;
	}
	else
	{
		return NRF_ERROR_NOT_FOUND;
	}
}

static uint32_t update_br3s_alarms_from_responce(br3s_alarm_response_t * p_data)
{
	if(br3s_protocol.p_br3s_params != NULL)
	{
		br3s_protocol.p_br3s_params->dev_info.alarm_on_hours = 
														p_data->alarm1.hours;
		br3s_protocol.p_br3s_params->dev_info.alarm_on_minutes = 
														p_data->alarm1.minutes;
		br3s_protocol.p_br3s_params->dev_info.alarm_off_hours = 
														p_data->alarm2.hours;
		br3s_protocol.p_br3s_params->dev_info.alarm_off_minutes = 
														p_data->alarm2.minutes;
		*br3s_protocol.p_br3s_state_valid = 1;
		br3s_protocol.p_br3s_callback((void *)&br3s_protocol, 
				NULL, NULL, START_DEV_POLL);
		return NRF_SUCCESS;
	}
	else
	{
		return NRF_ERROR_NOT_FOUND;
	}
}

static uint32_t create_request_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	p_local_br3s_frame->br3s_frame.comID = SEND_REQ_COM_ID;
	p_local_br3s_frame->br3s_frame.devID = ADAPTER_ID;
	p_local_br3s_frame->br3s_frame.CRC8 = CALCULATE_CRC;
	
	p_local_br3s_frame->resp_type = SEND_REQ_RESP_ID;
	return NRF_SUCCESS;
}

static uint32_t create_set_params_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;

	p_local_br3s_frame->br3s_frame.devID = ADAPTER_ID;
	p_local_br3s_frame->br3s_frame.comID = SEND_SET_PARAM_COM_ID;
	memcpy(&p_local_br3s_frame->br3s_frame.set_param.br3s_state_of_flags,
		   &br3s_protocol.p_br3s_params->dev_info.flags,
		   sizeof(flags_state_t));
	p_local_br3s_frame->br3s_frame.set_param.dumper_pos =
		br3s_protocol.p_br3s_params->dev_info.dumper_pos;
	p_local_br3s_frame->br3s_frame.set_param.fan_speed =
		br3s_protocol.p_br3s_params->dev_info.fan_speed;
	p_local_br3s_frame->br3s_frame.set_param.t_set = 
		br3s_protocol.p_br3s_params->dev_info.t_set;
	p_local_br3s_frame->br3s_frame.set_param.filter_time.ft_value = 
		br3s_protocol.p_br3s_params->dev_info.filter_time;
	p_local_br3s_frame->br3s_frame.set_param.filter_time.ft_save_bit = 0;
	p_local_br3s_frame->br3s_frame.CRC8 = CALCULATE_CRC;
	
	p_local_br3s_frame->resp_type = SEND_SET_PARAM_RESP_ID;
	return NRF_SUCCESS;
}

static uint32_t create_set_filter_time_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	ret_code_t ret_val = NRF_SUCCESS;
	
	ret_val = create_set_params_com(p_context);
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.set_param.filter_time.ft_save_bit = 1;
	
	return ret_val;
}

static uint32_t create_reset_filter_time_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	ret_code_t ret_val = NRF_SUCCESS;
	
	ret_val = create_set_params_com(p_context);
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.set_param.filter_time.ft_reset = 1;

	return ret_val;
}

static uint32_t create_set_service_mode_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	ret_code_t ret_val = NRF_SUCCESS;
	
	ret_val = create_set_params_com(p_context);
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.set_param.service_mode_flag = 1;

	return ret_val;
}

static uint32_t create_hard_reset_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	ret_code_t ret_val = NRF_SUCCESS;
	
	ret_val = create_set_params_com(p_context);
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.set_param.hard_reset_flag = 1;
	
	return ret_val;
}

static uint32_t create_start_ma_pairing_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	ret_code_t ret_val = NRF_SUCCESS;
	
	ret_val = create_set_params_com(p_context);
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.set_param.br3s_state_of_flags.ma_pairing = 1;

	return ret_val;
}

static uint32_t create_set_time_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.devID = ADAPTER_ID;
	p_local_br3s_frame->br3s_frame.comID = SEND_SET_TIME_COM_ID;
	p_local_br3s_frame->br3s_frame.set_time.us_date_time.time.hours = 
		br3s_protocol.p_br3s_params->dev_info.us_hour;
	p_local_br3s_frame->br3s_frame.set_time.us_date_time.time.minutes = 
		br3s_protocol.p_br3s_params->dev_info.us_minutes;
	p_local_br3s_frame->br3s_frame.CRC8 = CALCULATE_CRC;
	
	p_local_br3s_frame->resp_type = SEND_SET_TIME_RESP_ID;
	return NRF_SUCCESS;
}

static uint32_t create_alarm_request_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.devID = ADAPTER_ID;
	p_local_br3s_frame->br3s_frame.comID = SEND_SET_ALARM_COM_ID;
	p_local_br3s_frame->br3s_frame.set_alarm.com_type = 0;
	p_local_br3s_frame->br3s_frame.CRC8 = CALCULATE_CRC;
	
	p_local_br3s_frame->resp_type = SEND_SET_ALARM_RESP_ID;
	return NRF_SUCCESS;
}

static uint32_t create_set_alarm_on_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.devID = ADAPTER_ID;
	p_local_br3s_frame->br3s_frame.comID = SEND_SET_ALARM_COM_ID;
	p_local_br3s_frame->br3s_frame.set_alarm.com_type = 1;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.temp_set =
		br3s_protocol.p_br3s_params->dev_info.t_set;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_set.fan_speed =
		br3s_protocol.p_br3s_params->dev_info.fan_speed;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_set.dumper_pos =
		br3s_protocol.p_br3s_params->dev_info.dumper_pos;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.alarm_id = 0;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.com_source = 0;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.alarm_state = 1;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.power_state = 1;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_time_start.hours = 
		br3s_protocol.p_br3s_params->dev_info.alarm_on_hours;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_time_start.minutes = 
		br3s_protocol.p_br3s_params->dev_info.alarm_on_minutes;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_time_start.seconds = 0;
	p_local_br3s_frame->br3s_frame.CRC8 = CALCULATE_CRC;
	
	p_local_br3s_frame->resp_type = SEND_SET_ALARM_RESP_ID;
	return NRF_SUCCESS;
}

static uint32_t create_set_alarm_off_com(void * p_context)
{
	static br3s_output_com_t * p_local_br3s_frame;
	
	p_local_br3s_frame = (br3s_output_com_t *)p_context;
	
	p_local_br3s_frame->br3s_frame.devID = ADAPTER_ID;
	p_local_br3s_frame->br3s_frame.comID = SEND_SET_ALARM_COM_ID;
	p_local_br3s_frame->br3s_frame.set_alarm.com_type = 1;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.temp_set =
		br3s_protocol.p_br3s_params->dev_info.t_set;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_set.fan_speed =
		br3s_protocol.p_br3s_params->dev_info.fan_speed;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_set.dumper_pos =
		br3s_protocol.p_br3s_params->dev_info.dumper_pos;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.alarm_id = 1;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.com_source = 0;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.alarm_state = 1;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_status.power_state = 0;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_time_start.hours = 
		br3s_protocol.p_br3s_params->dev_info.alarm_off_hours;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_time_start.minutes = 
		br3s_protocol.p_br3s_params->dev_info.alarm_off_minutes;
	p_local_br3s_frame->br3s_frame.set_alarm.alarm.alarm_time_start.seconds = 0;
	p_local_br3s_frame->br3s_frame.CRC8 = CALCULATE_CRC;
	
	p_local_br3s_frame->resp_type = SEND_SET_ALARM_RESP_ID;
	return NRF_SUCCESS;
}

static uint32_t init_protocol_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	br3s_main_t * p_local_br3s_main;
	
	p_local_br3s_main = (br3s_main_t *)p_context;
	
	br3s_protocol.p_br3s_params = &p_local_br3s_main->br3s_state;
	br3s_protocol.p_brc_main	= p_local_br3s_main->p_brc_main;
	br3s_protocol.p_br3s_state_valid	= &p_local_br3s_main->br3s_state_valid;
	br3s_protocol.p_br3s_callback		= p_local_br3s_main->p_br3s_callback;
	br3s_protocol.p_br3s_params->br3s_invalid_speed = 0;
	br3s_protocol.p_expected_resp_type = &p_local_br3s_main->expected_resp_type;
	return ret_val;
}

static uint32_t deinit_protocol_instance(void * p_context)
{
	br3s_protocol.p_br3s_params 		= NULL;
	br3s_protocol.p_brc_main			= NULL;
	br3s_protocol.p_br3s_state_valid	= NULL;
	br3s_protocol.p_br3s_callback		= NULL;
	br3s_protocol.p_expected_resp_type 	= NULL;

	nrf_queue_reset(&br3s_raw_input);
	nrf_queue_reset(&br3s_output);

	return NRF_SUCCESS;
}

static uint32_t input_data_handler(void * p_context, uint32_t length)
{
	static br3s_output_com_t b_local_br3s_frame = {0};
	ret_code_t ret_val;
	uint8_t com_type;

	if (br3s_protocol.p_br3s_params->waiting_responce)
	{
		ret_val = NRF_ERROR_BUSY;
		return ret_val;
	}
	
	memset(&b_local_br3s_frame, 0, sizeof(br3s_output_com_t));
	com_type = *(uint8_t *)p_context;
	if (com_type == 0)
	{
		ret_val = NRF_ERROR_BUSY;
		return ret_val;
	}
	ret_val = create_com[com_type]((void *)&b_local_br3s_frame);
	if(ret_val == NRF_SUCCESS)
	{
		if (!br3s_protocol.p_br3s_params->waiting_responce)
		{
			ret_val = br3s_device_vptr.event((void *)&br3s_protocol, 
					(void *)&b_local_br3s_frame, 
					BR3S_RAW_OUTPUT_BUFFER_SIZE, BLE_FORCE_SEND_DATA);
			if (ret_val != NRF_SUCCESS)
			{
				ret_val = nrf_queue_push(&br3s_output, 
					(const void *)&b_local_br3s_frame);
			}
		}
		else
			ret_val = nrf_queue_push(&br3s_output, 
					(const void *)&b_local_br3s_frame);
	}
	return ret_val;
}

static uint32_t output_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val;
	static br3s_frame_t b_buffer_frame;
	
	ret_val = nrf_queue_peek(&br3s_raw_input, (void *)&b_buffer_frame);
	if(ret_val != NRF_SUCCESS)
		return ret_val;
	ret_val = nrf_queue_pop(&br3s_raw_input, (void *)&b_buffer_frame);
	if(ret_val == NRF_SUCCESS)
	{
		switch (b_buffer_frame.comID)
		{
			case SEND_REQ_RESP_ID:
			case SEND_SET_TIME_RESP_ID:
			{	
				if (*br3s_protocol.p_expected_resp_type == b_buffer_frame.comID)
				{
					ret_val = update_br3s_params_from_responce(
										&b_buffer_frame.get_param);
					*br3s_protocol.p_expected_resp_type = NULL;
				}
			}	break;
			
			case SEND_SET_PARAM_RESP_ID:
			{	
				if (*br3s_protocol.p_expected_resp_type == b_buffer_frame.comID)
				{
					ret_val = update_br3s_params_from_responce_on_set(
										&b_buffer_frame.get_param);
					*br3s_protocol.p_expected_resp_type = NULL;
				}
			}	break;
			
			case SEND_SET_ALARM_RESP_ID:
			{	
				if (*br3s_protocol.p_expected_resp_type == b_buffer_frame.comID)
				{
					ret_val = update_br3s_alarms_from_responce(
										&b_buffer_frame.resp_alarm);
					*br3s_protocol.p_expected_resp_type = NULL;
				}
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
	static br3s_frame_t * p_buffer_frame;
	
	p_buffer_frame = (br3s_frame_t *)p_context;
	
	ret_val = nrf_queue_push(&br3s_raw_input, (const void *)p_buffer_frame);
	
	return ret_val;
}

static uint32_t output_raw_data_handler(void * p_context, uint32_t length)
{
	ret_code_t ret_val;
	static uint8_t b_local_raw_data_array[BR3S_RAW_OUTPUT_BUFFER_SIZE] = {0};
	br3s_data_t * p_local_raw_data;

	p_local_raw_data = (br3s_data_t *)p_context;
		
	ret_val = nrf_queue_peek(&br3s_output, (void *)&b_local_raw_data_array);
	if(ret_val != NRF_SUCCESS)
		return ret_val;
	
	if(ret_val == NRF_SUCCESS)
	{
		p_local_raw_data->data_size = BR3S_RAW_OUTPUT_BUFFER_SIZE;
		p_local_raw_data->p_raw_data = b_local_raw_data_array;
		ret_val = nrf_queue_pop(&br3s_output, (void *)b_local_raw_data_array);
	}
	else
	{
		p_local_raw_data->data_size = NULL;
		p_local_raw_data->p_raw_data = NULL;
	}
	
	return ret_val;
}

/*Public functions*/
ble_adv_data_parser_t br3s_get_adv_parser (void)
{
	return &adv_data_parser;
}
/******************************* END_OF_FILE **********************************/