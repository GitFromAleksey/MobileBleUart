/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "remote_main.h"
#include "brc_lcd_indication.h"
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
const device_m_list_t	remote_device_vptr = {
	.init 				= &init_device_instance,
	.deinit 			= &deinit_device_instance,
	.execute 			= &execute_device_instance,
	.event				= &device_event_instance,
};

/*Private variables*/
static remote_main_t remote_main = {.interface = 
						   {.vptr_dev = &remote_device_vptr,
							.vptr_protocol = &remote_protocol_vptr,
							.vptr_button_reaction = &remote_button_reaction_vptr,
							.vptr_menu = &remote_menu_vptr,},
							.remote_state.screen_test_state = 0,};
							
/*static device_instance_callback_t p_remote_callback;*/
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/ 

/*Private functions*/
static uint32_t ble_callback (void * p_data)
{
	uint32_t ret_val = NRF_SUCCESS;
	ble_rx_t * p_rx_data;

	p_rx_data = (ble_rx_t *)p_data;

	switch (p_rx_data->evt_type)
	{
		case RX_DATA:
		{

		}	break;
		case CONNECTION:
		{
			switch (p_rx_data->dev_type)
			{
				case BRLT_DEV:
				{
					uint32_t timeout = SWITCH_TO_DEV_TIME;
					remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
					remote_main.p_menu_state->sub_state = REMOTE_BLE_FOUND_BRLT_ID;
					remote_main.interface.vptr_menu->execute((void *)&remote_main,
								NULL, NULL);
					remote_main.p_remote_callback((void *)&remote_main, 
							(void *)&timeout, NULL, FOUND_BRLT);
				}	break;
				case BR3S_DEV:
				{
					uint32_t timeout = SWITCH_TO_DEV_TIME;
					remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
					remote_main.p_menu_state->sub_state = REMOTE_BLE_FOUND_BR3S_ID;
					remote_main.interface.vptr_menu->execute((void *)&remote_main,
								NULL, NULL);
					ret_val = remote_main.p_remote_callback((void *)&remote_main, 
							(void *)&timeout, NULL, FOUND_BR3S);
				}	break;
				case BR4S_DEV:
				{
					uint32_t timeout = SWITCH_TO_DEV_TIME;
					remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
					remote_main.p_menu_state->sub_state = REMOTE_BLE_FOUND_BR4S_ID;
					remote_main.interface.vptr_menu->execute((void *)&remote_main,
								NULL, NULL);
					ret_val = remote_main.p_remote_callback((void *)&remote_main, 
							(void *)&timeout, NULL, FOUND_BR4S);
				}	break;
				default:
					break;
			}
		}	break;
		case FOUND_CONNECTABLE_SUPPORTED_DEV:
		{
			remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
		
			switch (p_rx_data->dev_type)
			{
				case BRLT_DEV:
				{
					remote_main.p_menu_state->sub_state = REMOTE_BLE_FOUND_BRLT_ID;
				}	break;
				case BR3S_DEV:
				{
					remote_main.p_menu_state->sub_state = REMOTE_BLE_FOUND_BR3S_ID;
				}	break;
				case BR4S_DEV:
				{
					remote_main.p_menu_state->sub_state = REMOTE_BLE_FOUND_BR4S_ID;
				}	break;
				default:
					break;
			}
			remote_main.interface.vptr_menu->execute((void *)&remote_main,
								NULL, NULL);
		}	break;
		default:
			break;
	}
	return ret_val;
}


/*class methods section*/

static uint32_t init_device_instance(void * p_context, void * p_arg, 
					uint32_t length, void * p_dev_callback)
{
	uint32_t ret_val = NRF_SUCCESS;

	remote_main.p_remote_callback = (device_instance_callback_t)p_dev_callback;
	remote_main.p_brc_main = (brc_main_t *)p_arg;
	remote_main.remote_state.screen_test_state = remote_main.p_brc_main->test_mode;
	if (length == NULL)
	{
		static uint32_t timeout = 100;
		remote_main.p_remote_callback((void *)&remote_main, 
							(void *)&timeout, NULL, SLEEP_EVT);
		return ret_val;
	}	
	
	remote_main.p_menu_state = remote_menu_get_mode();

	remote_main.interface.vptr_protocol->init((void *)&remote_main);
	remote_main.interface.vptr_button_reaction->init((void *)&remote_main);
	remote_main.interface.vptr_menu->init((void *)&remote_main, NULL, NULL);
	
	remote_main.p_remote_callback((void *)&remote_main, NULL, NULL, 
				BUTTON_EVT_BLOCK_OVF);
	
	if (remote_main.remote_state.screen_test_state)
	{
		remote_main.p_menu_state->state = REMOTE_MENU_INIT_ID;
		remote_main.p_menu_state->sub_state = REMOTE_INIT_REPORT_BRC_FW_ID;
		remote_main.interface.vptr_menu->execute((void *)&remote_main,
							NULL, NULL);
		remote_main.p_remote_callback((void *)&remote_main, NULL, NULL, 
				REPORT_BRC_FW_TIMEOUT_RELOAD);
	}
	else
	{
		if (length != NULL)
		{
			bool is_wl = false;
			remote_main.p_brc_main->p_core_ble->sys.scan_permit((void *)&remote_main);
			remote_main.p_brc_main->p_core_ble->sys.connect((void *)&is_wl);
			

			if(is_wl)
			{
				remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
				remote_main.p_menu_state->sub_state = REMOTE_BLE_PAIR_ATT_M_ID;
				remote_main.interface.vptr_menu->execute((void *)&remote_main,
									NULL, NULL);
			}
			else
			{
				remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
				remote_main.p_menu_state->sub_state = REMOTE_BLE_CONN_ATT_M_ID;
				remote_main.interface.vptr_menu->execute((void *)&remote_main,
									NULL, NULL);
			}
			remote_main.p_remote_callback((void *)&remote_main, NULL, NULL,
					USER_TIMEOUT_RELOAD_EVT);
		}
		else
		{
			static uint32_t timeout = 100;
			remote_main.p_remote_callback((void *)&remote_main, 
								(void *)&timeout, NULL, SLEEP_EVT);
		}
	}
	
	return ret_val;
}

static uint32_t deinit_device_instance(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	remote_main.interface.vptr_menu->deinit((void *)&remote_main);
	remote_main.interface.vptr_protocol->deinit((void *)&remote_main);
	remote_main.interface.vptr_button_reaction->deinit((void *)&remote_main);
	
	remote_main.p_menu_state = NULL;
	//p_remote_callback = NULL;
	remote_main.p_brc_main = NULL;
	remote_main.remote_state.screen_test_state = 0;

	return ret_val;
}

static uint32_t execute_device_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	uint32_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t device_event_instance(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type)
{
	uint32_t ret_val = NRF_SUCCESS;
	if ((remote_main.p_brc_main != NULL) || 
		(evt_type == START_SCREEN_TEST))
	{
		switch(evt_type)
		{
			case DEFAULT_EVT:
				break;

			case USER_TIMEOUT_OVF_EVT:
			{
				uint32_t timeout = 2000;
				ble_read_out_t b_ble_read_out = {
				.read_type = SYS_READ,
				.p_ble_sys = NULL,
				};
				remote_main.p_brc_main->p_core_ble->read((void*)&b_ble_read_out);
				if(b_ble_read_out.p_ble_sys->m_scan_mode == BLE_WHITELIST_SCAN)
				{
					remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
					remote_main.p_menu_state->sub_state = REMOTE_BLE_CONN_FAIL_M_ID;
					remote_main.interface.vptr_menu->execute((void *)&remote_main,
									NULL, NULL);
				}
				else
				{
					remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
					remote_main.p_menu_state->sub_state = REMOTE_BLE_PAIR_FAIL_M_ID;
					remote_main.interface.vptr_menu->execute((void *)&remote_main,
									NULL, NULL);
				}
				remote_main.p_remote_callback((void *)&remote_main, 
						(void *)&timeout, NULL, SLEEP_EVT);
			}	break;
			
			case REPORT_BRC_FW_TIMEOUT_OVF:
			{
				remote_main.p_menu_state->state = REMOTE_MENU_INIT_ID;
				remote_main.p_menu_state->sub_state = REMOTE_INIT_SCREEN_TEST_ID;
				remote_main.interface.vptr_menu->execute((void *)&remote_main,
							NULL, NULL);
				remote_main.p_remote_callback((void *)&remote_main, 
						NULL, NULL, SCREEN_TEST_TIMEOUT_RELOAD);
			}	break;
			
			case SCREEN_TEST_TIMEOUT_OVF:
			{
				remote_main.p_menu_state->state = REMOTE_MENU_INIT_ID;
				remote_main.p_menu_state->sub_state = REMOTE_INIT_SCREEN_TEST_ID;
				remote_main.interface.vptr_menu->execute((void *)&remote_main,
							NULL, NULL);
				remote_main.p_remote_callback((void *)&remote_main, 
						NULL, NULL, SCREEN_TEST_TIMEOUT_RELOAD);
			}	break;

			case START_SCREEN_TEST:
			{
				remote_main.remote_state.screen_test_state = 1;
				remote_main.p_menu_state->state = REMOTE_MENU_INIT_ID;
				remote_main.p_menu_state->sub_state = REMOTE_INIT_REPORT_BRC_FW_ID;
				remote_main.interface.vptr_menu->execute((void *)&remote_main,
									NULL, NULL);
				remote_main.p_remote_callback((void *)&remote_main, NULL, NULL, 
						REPORT_BRC_FW_TIMEOUT_RELOAD);
			}	break;

			case BLE_EVT:
			{
				ble_callback(p_arg);
			}	break;

			case BLE_BONDS_ERASED:
			{
				remote_main.p_remote_callback((void *)&remote_main, NULL, NULL, 
						USER_TIMEOUT_RELOAD_EVT);
				remote_main.p_menu_state->state = REMOTE_MENU_BLE_ID;
				remote_main.p_menu_state->sub_state = REMOTE_BLE_PAIR_ATT_M_ID;
				remote_main.interface.vptr_menu->execute((void *)&remote_main,
								NULL, NULL);
				remote_main.p_remote_callback((void *)&remote_main, NULL, NULL, 
						BLE_START_PAIRING);
			}	break;

			case BUTTON_EVT:
			{
				remote_main.p_remote_callback((void *)&remote_main, NULL, NULL, 
						USER_TIMEOUT_RELOAD_EVT);
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
device_t * remote_get_interface(void)
{
	return &remote_main.interface;
}

ble_adv_data_parser_t remote_get_p_adv_data(void)
{
	return remote_get_adv_parser();
}
/************************* END_OF_FILE **********************************/