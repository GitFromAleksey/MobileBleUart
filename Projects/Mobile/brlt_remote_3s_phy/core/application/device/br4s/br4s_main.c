/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "br4s_main.h"
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
const device_m_list_t	br4s_device_vptr = {
	.init 				= &init_device_instance,
	.deinit 			= &deinit_device_instance,
	.execute 			= &execute_device_instance,
	.event				= &device_event_instance,
};
/*Private variables*/
static br4s_main_t br4s_main = {.interface = 
						   {.vptr_dev = &br4s_device_vptr,
							.vptr_protocol = &br4s_protocol_vptr,
							.vptr_button_reaction = &br4s_button_reaction_vptr,
							.vptr_menu = &br4s_menu_vptr,
						   }};
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
		br4s_main.p_menu_state->sub_state_mode;
	
	switch (b_local_sub_state_mode)
	{
		case BR4S_ON_SET_HEATER_HOFF_M_ID:
		case BR4S_ON_SET_HEATER_TSET_M_ID:
		case BR4S_ON_SET_ALARM_ON_M_ID:
		case BR4S_ON_SET_ALARM_OFF_M_ID:
		{
			com_type_id = BR4S_SET_PARAMS_ID;
		}	break;

		case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
		{
			com_type_id = BR4S_REQ_ON_TIMER_ID;
		}	break;

		case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
		{
			com_type_id = BR4S_REQ_OFF_TIMER_ID;
		}	break;

		case BR4S_ON_SET_TIME_HOUR_M_ID:
		case BR4S_ON_SET_TIME_MIN_M_ID:
		{
			com_type_id = BR4S_SET_TIME_ID;
		}	break;	

		case BR4S_ON_SET_FILTER_TIME_M_ID:
		{
			com_type_id = BR4S_SET_FILTER_TIME_ID;
		}	break;

		default:
			break;
	}
	br4s_main.br4s_state_valid = 0;
	br4s_main.p_br4s_callback((void *)&br4s_main, 
				NULL, NULL, STOP_DEV_POLL);
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
	return ret_val;
} 

/*class methods section*/

static uint32_t init_device_instance(void * p_context, void * p_arg, 
					uint32_t length, void * p_dev_callback)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t comm_type;

	br4s_main.p_br4s_callback = (device_instance_callback_t)p_dev_callback;
	br4s_main.p_brc_main = (brc_main_t *)p_arg;
	br4s_main.p_menu_state = br4s_menu_get_mode();
	
	br4s_main.interface.vptr_protocol->init((void *)&br4s_main);
	br4s_main.interface.vptr_button_reaction->init((void *)&br4s_main);
	br4s_main.interface.vptr_menu->init((void *)&br4s_main, NULL, NULL);

	comm_type = BR4S_REQ_SYS_PARAMS_ID;
	br4s_main.interface.vptr_protocol->input_data((void *)&comm_type, NULL);

	br4s_main.br4s_state_valid = 0;
	br4s_main.br4s_state.waiting_responce = 0;
	br4s_main.p_br4s_callback((void *)&br4s_main, NULL, NULL, START_DEV_POLL);
	return ret_val;
}

static uint32_t deinit_device_instance(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	br4s_main.interface.vptr_menu->deinit((void *)&br4s_main.br4s_state);
	br4s_main.interface.vptr_protocol->deinit((void *)&br4s_main.br4s_state);
	br4s_main.interface.vptr_button_reaction->deinit((void *)&br4s_main.br4s_state);
	
	br4s_main.p_menu_state = NULL;
	br4s_main.p_br4s_callback = NULL;
	br4s_main.p_brc_main = NULL;
	br4s_main.br4s_state_valid = 0;
	br4s_main.prev_menu_sub_state_mode = 0;
	br4s_main.expected_resp_type = 0;
	br4s_main.first_resp_recieved = 0;
	memset(&br4s_main.br4s_state, 0x0, sizeof(br4s_main.br4s_state));

	return ret_val;
}

static uint32_t execute_device_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	uint32_t ret_val = NRF_SUCCESS;
	static br4s_data_t b_local_data = {0};

	if(br4s_main.interface.vptr_protocol != NULL)
	{
		if (!br4s_main.br4s_state.waiting_responce)
		{
			ret_val = br4s_main.interface.vptr_protocol->output_raw_data((void *)&b_local_data, NULL);
			if (ret_val == NRF_SUCCESS)
			{
					ret_val = br4s_main.p_brc_main->p_core_ble->write((void *)&b_local_data);
			}	
		}
		else
		{
			if (br4s_main.expected_resp_type == NULL)
			{
				br4s_main.br4s_state.waiting_responce = 0;
			}
		}
		ret_val = br4s_main.interface.vptr_protocol->output_data((void *)&br4s_main, NULL);
		if (ret_val == NRF_SUCCESS)
		{
			
		}
	}
	br4s_main.interface.vptr_menu->execute((void *)&br4s_main, NULL, NULL);
	
	if(br4s_main.br4s_state.br4s_invalid_speed)
	{
		br4s_main.p_br4s_callback((void *)&br4s_main, NULL, NULL, INVALID_SPEED);
		if ((br4s_main.p_menu_state->sub_state == BR4S_ON_DEFAULT_M_ID) && 
			(br4s_main.p_menu_state->state == BR4S_MENU_ON_ID))
		{
			if (br4s_main.p_menu_state->sub_state_mode != 
				BR4S_ON_DEFAULT_INVALID_SPEED_M_ID)
			{
				br4s_main.prev_menu_sub_state_mode = 
					br4s_main.p_menu_state->sub_state_mode;
				br4s_main.p_menu_state->sub_state_mode = 
					BR4S_ON_DEFAULT_INVALID_SPEED_M_ID;
			}
		}
		br4s_main.br4s_state.br4s_invalid_speed = 0;
	}

	if(br4s_main.br4s_state.dev_info.errors != 0)
	{
		if(br4s_main.p_menu_state->state == BR4S_MENU_ON_ID) 
		{
			br4s_main.p_menu_state->sub_state = BR4S_ON_ERROR_M_ID;
			br4s_main.p_menu_state->sub_state_mode = 0;
		}
		else
		{
			br4s_main.p_menu_state->sub_state = BR4S_OFF_ERROR_M_ID;
			br4s_main.p_menu_state->sub_state_mode = 0;
		}
	}
	else
	{
		if	((br4s_main.p_menu_state->state == BR4S_MENU_ON_ID) &&
			 (br4s_main.p_menu_state->sub_state == BR4S_ON_ERROR_M_ID))
		{
			br4s_main.p_menu_state->sub_state = BR4S_ON_DEFAULT_M_ID;
			br4s_main.p_menu_state->sub_state_mode = 0;
		}
		else if ((br4s_main.p_menu_state->state == BR4S_MENU_OFF_ID) &&
				 (br4s_main.p_menu_state->sub_state == BR4S_OFF_ERROR_M_ID))
		{
			br4s_main.p_menu_state->sub_state = BR4S_OFF_DEFAULT_M_ID;
			br4s_main.p_menu_state->sub_state_mode = 0;
		}
	}
	return ret_val;
}
static uint32_t device_event_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	static uint8_t comm_type = BR4S_REQ_DEV_PARAMS_ID;
	static uint8_t comm_wdt = 0;
	uint32_t ret_val = NRF_SUCCESS;
	if(br4s_main.p_brc_main != NULL)
	{
		switch(evt_type)
		{
			case DEFAULT_EVT:
			{

			}	break;
			case USER_TIMEOUT_OVF_EVT:
			{
				if (br4s_main.p_menu_state->sub_state ==
				 	BR4S_ON_SET_M_ID)
				{
					send_current_param_value_to_breezer();
					br4s_main.p_menu_state->sub_state = 
						BR4S_ON_DEFAULT_M_ID;
					br4s_main.p_menu_state->sub_state_mode =
						BR4S_ON_DEFAULT_TOUT_M_ID;
				}
				else if (br4s_main.p_menu_state->sub_state ==
						 BR4S_ON_DEFAULT_M_ID)
				{
					br4s_main.p_menu_state->sub_state_mode =
						BR4S_ON_DEFAULT_TOUT_M_ID;
				}
			}	break;
			case DEV_POLL_TIMER_RELOAD:
			{
				if ((br4s_main.br4s_state.waiting_responce) ||
					(br4s_main.expected_resp_type != NULL))
				{
					if (comm_wdt++ > 5)
					{
						br4s_main.br4s_state.waiting_responce = 0;
						br4s_main.expected_resp_type = NULL;
						comm_wdt = 0;
					}
					break;
				}
				if (br4s_main.br4s_state_valid == 0)
				{
					if(br4s_main.first_resp_recieved)
						break;					
				}
				comm_type = BR4S_REQ_DEV_PARAMS_ID;
				br4s_main.interface.vptr_protocol->input_data(
													(void *)&comm_type, NULL);
				
				comm_type = BR4S_REQ_ON_TIMER_ID;
				br4s_main.interface.vptr_protocol->input_data(
													(void *)&comm_type, NULL);

				comm_type = BR4S_REQ_OFF_TIMER_ID;
				br4s_main.interface.vptr_protocol->input_data(
													(void *)&comm_type, NULL);
                                
				comm_type = BR4S_REQ_TIME_ID;
				br4s_main.interface.vptr_protocol->input_data(
													(void *)&comm_type, NULL);

                br4s_main.br4s_state_valid = 0;
				
				comm_wdt = 0;
				
			}	break;
			case BLE_BONDS_ERASED:
			{
			  	br4s_main.p_br4s_callback((void *)&br4s_main,
						NULL, NULL, BLE_DISCONNECT);
			}	break;
			case BLE_FORCE_SEND_DATA:
			{
				br4s_data_t b_local_data;
				if (length != BR4S_RAW_OUTPUT_BUFFER_SIZE)
				{
					ret_val = NRF_ERROR_NOT_FOUND;
				}
				else
				{
					b_local_data.p_data = (uint8_t *)p_arg;
					b_local_data.data_len = length;
						ret_val = br4s_main.p_brc_main->p_core_ble->write((void *)&b_local_data);
				}
				
			}	break;

			case INVALID_SPEED_TIMEOUT_OVF_EVT:
			{
				if (br4s_main.p_menu_state->sub_state_mode ==
				 	BR4S_ON_DEFAULT_INVALID_SPEED_M_ID)
				{
					br4s_main.p_menu_state->sub_state_mode =
					br4s_main.prev_menu_sub_state_mode;
				}
			}	break;			
			case BUTTON_EVT:
			{
				if (br4s_main.p_menu_state->sub_state !=
					BR4S_ON_SET_M_ID)
				{
					br4s_main.p_br4s_callback((void *)&br4s_main, NULL, NULL, 
							USER_TIMEOUT_RELOAD_EVT);
				}
				else
				{
					br4s_main.p_br4s_callback((void *)&br4s_main, NULL, NULL, 
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
device_t * br4s_get_interface(void)
{
	return &br4s_main.interface;
}
ble_adv_data_parser_t br4s_get_p_adv_data(void)
{
	return br4s_get_adv_parser();
}	
/******************************* END_OF_FILE **********************************/