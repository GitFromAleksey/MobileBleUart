/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "brlt_main.h"
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
const device_m_list_t	brlt_device_vptr = {
	.init 				=  &init_device_instance,
	.deinit 			=  &deinit_device_instance,
	.execute 			=  &execute_device_instance,
	.event				=  &device_event_instance,
};
/*Private variables*/
static brlt_main_t brlt_main = {.interface = 
						   {.vptr_dev = &brlt_device_vptr,
							.vptr_protocol = &brlt_protocol_vptr,
							.vptr_button_reaction = &brlt_button_reaction_vptr,
							.vptr_menu = &brlt_menu_vptr,
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
	
	switch (brlt_main.p_menu_state->sub_state_mode)
	{
		case BRLT_ON_SET_HEATER_TSET_M_ID:
		case BRLT_ON_SET_HEATER_H_INACTIVE_M_ID:
		{
			com_type_id = BRLT_SET_PARAMS_ID;
		}	break;
		case BRLT_ON_SET_FILTER_TIME_M_ID:
		{
			com_type_id = BRLT_SET_FILTER_TIME_ID;
		}	break;
		case BRLT_ON_SET_HEATER_HOFF_M_ID:
		default:
			com_type_id = BRLT_SET_PARAMS_ID;
			break;
	}
	brlt_main.brlt_state_valid = 0;
	brlt_main.p_brlt_callback((void *)&brlt_main, 
				NULL, NULL, STOP_DEV_POLL);
	brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
	return ret_val;
} 
/*class methods section*/

static uint32_t init_device_instance(void * p_context, void * p_arg, 
					uint32_t length, void * p_dev_callback)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t comm_type;
	
	brlt_main.p_brlt_callback = (device_instance_callback_t)p_dev_callback;
	brlt_main.p_brc_main = (brc_main_t *)p_arg;
	brlt_main.p_menu_state = brlt_menu_get_mode();
	
	brlt_main.interface.vptr_protocol->init((void *)&brlt_main);
	brlt_main.interface.vptr_button_reaction->init((void *)&brlt_main);
	brlt_main.interface.vptr_menu->init((void *)&brlt_main, NULL, NULL);


	comm_type = BRLT_REQ_SYS_PARAMS_ID;
	brlt_main.interface.vptr_protocol->input_data((void *)&comm_type, NULL);

	/*comm_type = BRLT_GET_AUTOKIV_PARAMS_ID;
	brlt_main.interface.vptr_protocol->input_data((void *)&comm_type, NULL);*/

	brlt_main.brlt_state_valid = 0;
	brlt_main.p_brlt_callback((void *)&brlt_main, NULL, NULL, START_DEV_POLL);
	return ret_val;
}

static uint32_t deinit_device_instance(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	brlt_main.interface.vptr_menu->deinit((void *)&brlt_main.brlt_state);
	brlt_main.interface.vptr_protocol->deinit((void *)&brlt_main.brlt_state);
	brlt_main.interface.vptr_button_reaction->deinit((void *)&brlt_main.brlt_state);
	
	brlt_main.p_menu_state = NULL;
	brlt_main.p_brlt_callback = NULL;
	brlt_main.p_brc_main = NULL;
	brlt_main.brlt_state_valid = 0;
	brlt_main.prev_menu_sub_state_mode = 0;
	memset(&brlt_main.brlt_state, 0x0, sizeof(brlt_main.brlt_state));

	return ret_val;
}

static uint32_t execute_device_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	uint32_t ret_val = NRF_SUCCESS;
	static data_t b_local_data = {0};
	ret_val = brlt_main.interface.vptr_protocol->output_raw_data((void *)&b_local_data, NULL);
	if(ret_val == NRF_SUCCESS)
	{
		ret_val = brlt_main.p_brc_main->p_core_ble->write((void *)&b_local_data);
	}
	ret_val = brlt_main.interface.vptr_protocol->output_data((void *)&brlt_main, NULL);
	if(ret_val == NRF_SUCCESS)
	{
		
	}
	brlt_main.interface.vptr_menu->execute((void *)&brlt_main, NULL, NULL);

	if(brlt_main.brlt_state.brlt_invalid_speed)
	{
		brlt_main.p_brlt_callback((void *)&brlt_main, NULL, NULL, INVALID_SPEED);
		if ((brlt_main.p_menu_state->sub_state == BRLT_ON_DEFAULT_M_ID) && 
			(brlt_main.p_menu_state->state == BRLT_MENU_ON_ID))
		{
			if (brlt_main.p_menu_state->sub_state_mode != 
				BRLT_ON_DEFAULT_INVALID_SPEED_M_ID)
			{
				brlt_main.prev_menu_sub_state_mode = 
					brlt_main.p_menu_state->sub_state_mode;
				brlt_main.p_menu_state->sub_state_mode = 
					BRLT_ON_DEFAULT_INVALID_SPEED_M_ID;
			}
		}
		brlt_main.brlt_state.brlt_invalid_speed = 0;
	}

	if(brlt_main.brlt_state.dev_info.errors.flag_register != 0)
	{
		if(brlt_main.p_menu_state->state == BRLT_MENU_ON_ID) 
		{
			brlt_main.p_menu_state->sub_state = BRLT_ON_ERROR_M_ID;
			brlt_main.p_menu_state->sub_state_mode = 0;
		}
		else
		{
			brlt_main.p_menu_state->sub_state = BRLT_OFF_ERROR_M_ID;
			brlt_main.p_menu_state->sub_state_mode = 0;
		}
	}else 
	{
		if (brlt_main.p_menu_state->state == BRLT_MENU_ON_ID &&
			brlt_main.p_menu_state->sub_state == BRLT_ON_ERROR_M_ID)
		{
			if (brlt_main.brlt_state.dev_info.flags.kiv_active)
				brlt_main.p_menu_state->sub_state = BRLT_ON_KIV_M_ID;
			else if (brlt_main.brlt_state.dev_info.flags.power_mode)
				brlt_main.p_menu_state->sub_state = BRLT_ON_DEFAULT_M_ID;
			else 
				brlt_main.p_menu_state->state = BRLT_MENU_OFF_ID;
		} else if (brlt_main.p_menu_state->state == BRLT_MENU_OFF_ID &&
					brlt_main.p_menu_state->sub_state == BRLT_OFF_ERROR_M_ID)
		{
			brlt_main.p_menu_state->sub_state = BRLT_OFF_DEFAULT_M_ID;
		}
	}	
	return ret_val;
}
static uint32_t device_event_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	/*static uint8_t comm_wdt = 0;*/
	uint32_t ret_val = NRF_SUCCESS;
	if(brlt_main.p_brc_main != NULL)
	{
		switch(evt_type)
		{
			case DEFAULT_EVT:
			{

			}	break;

			case USER_TIMEOUT_OVF_EVT:
			{
				if (brlt_main.p_menu_state->sub_state ==
				 	BRLT_ON_SET_M_ID)
				{
					send_current_param_value_to_breezer();
					brlt_main.p_menu_state->sub_state = 
						BRLT_ON_DEFAULT_M_ID;
					brlt_main.p_menu_state->sub_state_mode =
						BRLT_ON_DEFAULT_H_INACTIVE_M_ID;
				}
				else if (brlt_main.p_menu_state->sub_state ==
				 		BRLT_ON_DEFAULT_M_ID)
				{
					brlt_main.p_menu_state->sub_state_mode =
						BRLT_ON_DEFAULT_H_INACTIVE_M_ID;
				} else if (brlt_main.p_menu_state->sub_state ==
				 		BRLT_ON_KIV_M_ID)
				{
					brlt_main.p_menu_state->sub_state_mode =
						BRLT_ON_KIV_TOUT_M_ID;
				}

			}	break;

			case DEV_POLL_TIMER_RELOAD:
			{
				if(brlt_main.brlt_state_valid == 0)
					break;
				uint8_t comm_type = BRLT_REQ_DEV_PARAMS_ID;
				brlt_main.interface.vptr_protocol->input_data(
													(void *)&comm_type, NULL);
				comm_type = BRLT_GET_AUTOKIV_PARAMS_ID;
				brlt_main.interface.vptr_protocol->input_data((void *)&comm_type, NULL);
				brlt_main.brlt_state_valid = 0;
			}	break;

			case BLE_BONDS_ERASED:
			{
				brlt_main.p_brlt_callback((void *)&brlt_main, 
										  NULL, NULL, BLE_DISCONNECT);
			}	break;

			case INVALID_SPEED_TIMEOUT_OVF_EVT:
			{
				if (brlt_main.p_menu_state->sub_state_mode ==
				 	BRLT_ON_DEFAULT_INVALID_SPEED_M_ID)
				{
					brlt_main.p_menu_state->sub_state_mode =
					brlt_main.prev_menu_sub_state_mode;
				}
			}	break;

			case BUTTON_EVT:
			{
				if (brlt_main.p_menu_state->sub_state !=
					BRLT_ON_SET_M_ID)
				{
					brlt_main.p_brlt_callback((void *)&brlt_main, NULL, NULL, 
							USER_TIMEOUT_RELOAD_EVT);
				}
				else
				{
					brlt_main.p_brlt_callback((void *)&brlt_main, NULL, NULL, 
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
device_t * brlt_get_interface(void)
{
	return &brlt_main.interface;
}
ble_adv_data_parser_t brlt_get_p_adv_data(void)
{
	return brlt_get_adv_parser();
}	
/******************************* END_OF_FILE **********************************/