/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "br3s_main.h"
#include "brc_lcd_indication.h"
#include "nrf_delay.h"
/*Private defines*/
// clang-format off
// clang-format on
/*Private types*/

/*Private functions prototypes*/
static uint32_t init_device_instance(void * p_context, void * p_arg, 
					uint32_t length, void * p_dev_callback);
static uint32_t deinit_device_instance(void * p_context);
static uint32_t execute_device_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type);
static uint32_t device_event_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type);

/*Exported constants*/
const device_m_list_t	br3s_device_vptr = {
	.init 				= &init_device_instance,
	.deinit 			= &deinit_device_instance,
	.execute 			= &execute_device_instance,
	.event				= &device_event_instance,
};
/*Private variables*/
static br3s_main_t br3s_main = {.interface = 
						   {.vptr_dev = &br3s_device_vptr,
							.vptr_protocol = &br3s_protocol_vptr,
							.vptr_button_reaction = &br3s_button_reaction_vptr,
							.vptr_menu = &br3s_menu_vptr,}};
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/ 

/*Private functions*/
static uint32_t send_current_param_value_to_breezer(void)
{
	static uint8_t com_type_id;
	uint32_t ret_val = NRF_SUCCESS;
	uint32_t b_local_sub_state_mode = 
		br3s_main.p_menu_state->sub_state_mode;
	
	switch (b_local_sub_state_mode)
	{
		case BR3S_ON_SET_HEATER_HOFF_M_ID:
		case BR3S_ON_SET_HEATER_TSET_M_ID:
		case BR3S_ON_SET_ALARM_ON_M_ID:
		case BR3S_ON_SET_ALARM_OFF_M_ID:
		{
			com_type_id = BR3S_SET_PARAMS_ID;
		}	break;

		case BR3S_ON_SET_ALARM_ON_HOUR_M_ID:
		case BR3S_ON_SET_ALARM_ON_MIN_M_ID:
		{
			com_type_id = BR3S_SET_ON_ALARM_ID;
		}	break;

		case BR3S_ON_SET_ALARM_OFF_HOUR_M_ID:
		case BR3S_ON_SET_ALARM_OFF_MIN_M_ID:
		{
			com_type_id = BR3S_SET_OFF_ALARM_ID;
		}	break;

		case BR3S_ON_SET_TIME_HOUR_M_ID:
		case BR3S_ON_SET_TIME_MIN_M_ID:
		{
			com_type_id = BR3S_SET_TIME_ID;
		}	break;	

		case BR3S_ON_SET_FILTER_TIME_M_ID:
		{
			com_type_id = BR3S_SET_FILTER_TIME_ID;
		}	break;

		default:
			break;
	}
	br3s_main.br3s_state_valid = 0;
	br3s_main.p_br3s_callback((void *)&br3s_main, 
				NULL, NULL, STOP_DEV_POLL);
	br3s_protocol_vptr.input_data((void *)&com_type_id, NULL);
	return ret_val;
} 

static uint32_t force_send_data_to_ble (data_t * p_send_data)
{
	uint32_t ret_val = NRF_SUCCESS;
	static data_t b_local_data = {0};
	br3s_output_com_t * p_send_com;
	
	br3s_main.br3s_state.waiting_responce = 1;
	p_send_com = (br3s_output_com_t *)p_send_data->p_data;	
	b_local_data.p_data = (uint8_t *)&p_send_com->br3s_frame;
	b_local_data.data_len = p_send_data->data_len -
		(BR3S_RAW_OUTPUT_BUFFER_SIZE - BR3S_RAW_BUFFER_SIZE);
	ret_val = br3s_main.p_brc_main->p_core_ble->write((void *)&b_local_data);
	br3s_main.expected_resp_type = p_send_com->resp_type;
	
	return ret_val;
}

/*class methods section*/

static uint32_t init_device_instance(void * p_context, void * p_arg, 
					uint32_t length, void * p_dev_callback)
{
	uint32_t ret_val = NRF_SUCCESS;

	br3s_main.p_br3s_callback = (device_instance_callback_t)p_dev_callback;
	br3s_main.p_brc_main = (brc_main_t *)p_arg;
	br3s_main.p_menu_state = br3s_menu_get_mode();
	
	br3s_main.interface.vptr_protocol->init((void *)&br3s_main);
	br3s_main.interface.vptr_button_reaction->init((void *)&br3s_main);
	br3s_main.interface.vptr_menu->init((void *)&br3s_main, NULL, NULL);

	br3s_main.br3s_state_valid = 0;
	br3s_main.br3s_state.waiting_responce = 0;
	br3s_main.p_br3s_callback((void *)&br3s_main, NULL, NULL, START_DEV_POLL);
	return ret_val;
}

static uint32_t deinit_device_instance(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	br3s_main.interface.vptr_menu->deinit((void *)&br3s_main.br3s_state);
	br3s_main.interface.vptr_protocol->deinit((void *)&br3s_main.br3s_state);
	br3s_main.interface.vptr_button_reaction->deinit((void *)&br3s_main.br3s_state);
	
	br3s_main.p_menu_state = NULL;
	br3s_main.p_br3s_callback = NULL;
	br3s_main.p_brc_main = NULL;
	br3s_main.br3s_state_valid = 0;
	br3s_main.prev_menu_sub_state_mode = 0;
	br3s_main.expected_resp_type = 0;
	br3s_main.first_resp_recieved = 0;
	memset(&br3s_main.br3s_state, 0x0, sizeof(br3s_main.br3s_state));

	return ret_val;
}

static uint32_t execute_device_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	uint32_t ret_val = NRF_SUCCESS;
	static data_t b_local_data = {0};

	if(br3s_main.interface.vptr_protocol != NULL)
	{
		if (!br3s_main.br3s_state.waiting_responce)
		{
			ret_val = br3s_main.interface.vptr_protocol->output_raw_data((void *)&b_local_data, NULL);
			if (ret_val == NRF_SUCCESS)
			{
				ret_val = force_send_data_to_ble(&b_local_data);
			}	
		}
		else
		{
			if (br3s_main.expected_resp_type == NULL)
			{
				br3s_main.br3s_state.waiting_responce = 0;
			}
		}
		ret_val = br3s_main.interface.vptr_protocol->output_data((void *)&br3s_main, NULL);
		if (ret_val == NRF_SUCCESS)
		{
			
		}
	}
	br3s_main.interface.vptr_menu->execute((void *)&br3s_main, NULL, NULL);
	if (br3s_main.br3s_state.br3s_invalid_speed == 1)
	{
		br3s_main.p_br3s_callback((void *)&br3s_main, NULL, NULL, INVALID_SPEED);
		if ((br3s_main.p_menu_state->sub_state == BR3S_ON_DEFAULT_M_ID) && 
			(br3s_main.p_menu_state->state == BR3S_MENU_ON_ID))
		{
			if (br3s_main.p_menu_state->sub_state_mode != 
				BR3S_ON_DEFAULT_INVALID_SPEED_M_ID)
			{
				br3s_main.prev_menu_sub_state_mode = 
				br3s_main.p_menu_state->sub_state_mode;
				br3s_main.p_menu_state->sub_state_mode = 
				BR3S_ON_DEFAULT_INVALID_SPEED_M_ID;
			}
		}
		br3s_main.br3s_state.br3s_invalid_speed = 0;
	}
	else if (br3s_main.br3s_state.br3s_invalid_speed == 2)
	{ 
		if ((br3s_main.p_menu_state->sub_state == BR3S_ON_DEFAULT_M_ID) && 
			(br3s_main.p_menu_state->state == BR3S_MENU_ON_ID))
		{
			if (br3s_main.p_menu_state->sub_state_mode == 
				BR3S_ON_DEFAULT_INVALID_SPEED_M_ID)
			{
				br3s_main.p_menu_state->sub_state_mode = 
					br3s_main.prev_menu_sub_state_mode;
			}
			br3s_main.br3s_state.br3s_invalid_speed = 0;
		}
	}
	if(br3s_main.br3s_state.dev_info.er_last_error_num != 0)
	{
		if(br3s_main.p_menu_state->state == BR3S_MENU_ON_ID) 
		{
			br3s_main.p_menu_state->sub_state = BR3S_ON_ERROR_M_ID;
			br3s_main.p_menu_state->sub_state_mode = 0;
		}
		else
		{
			br3s_main.p_menu_state->sub_state = BR3S_OFF_ERROR_M_ID;
			br3s_main.p_menu_state->sub_state_mode = 0;
		}
	}
	else
	{
		if	((br3s_main.p_menu_state->state == BR3S_MENU_ON_ID) &&
			 (br3s_main.p_menu_state->sub_state == BR3S_ON_ERROR_M_ID))
		{
			br3s_main.p_menu_state->sub_state = BR3S_ON_DEFAULT_M_ID;
			br3s_main.p_menu_state->sub_state_mode = 0;
		}
		else if ((br3s_main.p_menu_state->state == BR3S_MENU_OFF_ID) &&
				 (br3s_main.p_menu_state->sub_state == BR3S_OFF_ERROR_M_ID))
		{
			br3s_main.p_menu_state->sub_state = BR3S_OFF_DEFAULT_M_ID;
			br3s_main.p_menu_state->sub_state_mode = 0;
		}
	}
	return ret_val;
}
static uint32_t device_event_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	static uint8_t comm_type = BR3S_REQ_PARAMS_ID;
	static uint8_t comm_wdt = 0;
	uint32_t ret_val = NRF_SUCCESS;
	if(br3s_main.p_brc_main != NULL)
	{
		switch(evt_type)
		{
			case DEFAULT_EVT:
			{

			}	break;
			case USER_TIMEOUT_OVF_EVT:
			{
				if (br3s_main.p_menu_state->sub_state ==
				 	BR3S_ON_SET_M_ID)
				{
					send_current_param_value_to_breezer();
					br3s_main.p_menu_state->sub_state = 
						BR3S_ON_DEFAULT_M_ID;
					br3s_main.p_menu_state->sub_state_mode =
						BR3S_ON_DEFAULT_TOUT_M_ID;
				}
				else if (br3s_main.p_menu_state->sub_state ==
						 BR3S_ON_DEFAULT_M_ID)
				{
					br3s_main.p_menu_state->sub_state_mode =
						BR3S_ON_DEFAULT_TOUT_M_ID;
				}
			}	break;
			case DEV_POLL_TIMER_RELOAD:
			{
				if ((br3s_main.br3s_state.waiting_responce) ||
					(br3s_main.expected_resp_type != NULL))
				{
					if (comm_wdt++ > 5)
					{
						br3s_main.br3s_state.waiting_responce = 0;
						br3s_main.expected_resp_type = NULL;
						comm_wdt = 0;
					}
					break;
				}
				if (br3s_main.br3s_state_valid == 0)
				{
					if(br3s_main.first_resp_recieved)
						break;					
				}
				comm_type = BR3S_REQ_PARAMS_ID;
				br3s_main.br3s_state_valid = 0;
				br3s_main.interface.vptr_protocol->input_data(
													(void *)&comm_type, NULL);
				comm_wdt = 0;
				
			}	break;
			case BLE_BONDS_ERASED:
			{
			  	br3s_main.p_br3s_callback((void *)&br3s_main,
						NULL, NULL, BLE_DISCONNECT);
			}	break;
			case BLE_FORCE_SEND_DATA:
			{
				data_t b_local_data;
				if (length != BR3S_RAW_OUTPUT_BUFFER_SIZE)
				{
					ret_val = NRF_ERROR_NOT_FOUND;
				}
				else
				{
					b_local_data.p_data = (uint8_t *)p_arg;
					b_local_data.data_len = length;
					ret_val = force_send_data_to_ble(&b_local_data);
				}
				
			}	break;
			case INVALID_SPEED_TIMEOUT_OVF_EVT:
			{
				if (br3s_main.p_menu_state->sub_state_mode ==
				 	BR3S_ON_DEFAULT_INVALID_SPEED_M_ID)
				{
					br3s_main.p_menu_state->sub_state_mode =
					br3s_main.prev_menu_sub_state_mode;
				}
			}	break;

			case BUTTON_EVT:
			{
				if (br3s_main.p_menu_state->sub_state !=
					BR3S_ON_SET_M_ID)
				{
					br3s_main.p_br3s_callback((void *)&br3s_main, NULL, NULL, 
							USER_TIMEOUT_RELOAD_EVT);
				}
				else
				{
					br3s_main.p_br3s_callback((void *)&br3s_main, NULL, NULL, 
							USER_MENU_TIMEOUT_RELOAD_EVT);
				}
			}	break;
			default:
				break;
		}
	}
	else
		ret_val = NRF_ERROR_NOT_FOUND;
	return ret_val;
}

/*Public functions*/
device_t * br3s_get_interface(void)
{
	return &br3s_main.interface;
}
ble_adv_data_parser_t br3s_get_p_adv_data(void)
{
	return br3s_get_adv_parser();
}	
/******************************* END_OF_FILE **********************************/