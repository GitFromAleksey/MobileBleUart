#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "platform.h"
#include "boards.h"
#include "nordic_common.h"
#include "app_error.h"
#include "ble_db_discovery.h"
#include "app_timer.h"
#include "app_util.h"

#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "softdevice_handler.h"
#include "ble_advdata.h"

#include "fstorage.h"
#include "fds.h"
#include "peer_manager.h"
#include "ble_conn_state.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"

#include "timer_drv.h"
#include "main_common.h"
#include "br3s_main.h"
#include "br4s_main.h"
#include "brlt_main.h"
#include "remote_main.h"

#include "SEGGER_RTT.h"

/*Private defines*/
// clang-format off

// clang-format on

/*Private types*/
typedef struct _tag_event_data_t
{
	uint16_t	evt_type;
	uint16_t 	dev_type;
	uint16_t 	lenght;
	uint8_t 	* p_data;
}event_data_t;
/*Private functions prototypes*/
static uint32_t ble_rx_callback(ble_rx_t * p_rx_data);
static uint32_t dev_event_callback(void * p_context,
						void * p_arg, uint32_t length, uint32_t evt);
static void button_event_handler(uint32_t evt,	
					uint8_t * p_data, uint32_t data_len);

static void user_timeout_timer_id_handler (void * p_context);
static void app_sleep_timeout_timer_id_handler (void * p_context);
static void misc_timeout_timer_id_handler (void * p_context);
static void dev_poll_timer_id_handler (void * p_context);

static void sleep_mode_enter (uint32_t timeout);
static void switch_active_device(uint32_t dev_type, uint32_t timeout);

static void timer_drv_output_handler(timer_drv_output_evt_type_t event, 
				uint8_t * p_data, uint16_t data_len);
/*Exported constants*/

/*Private variables*/
static brc_main_t brc_main = {
	.p_core_ble = &ble_m_list,
	.p_lcd = &brc_lcd_indication_m_list,
	.expected_dev_type = NO_VALUE,
};

static timer_drv_input_evt_handler_t p_timer_drv_input_handler;
uint8_t t_button_id; 
uint8_t * p_t_button_id; 
uint32_t button_block = 1;
uint32_t test_mode = 0;
bool sleep_block = false;
/*Private macro*/
// clang-format off
#define CURRENT_BUTTON_EVT(_handler_, _p_context_)	\
do{ 									\
	brc_main.p_current_dev->interface->vptr_button_reaction->_handler_((void *)_p_context_, NULL);\
}while(0)
// clang-format on
/*Private constants*/ 

/*Private functions*/
static void timer_drv_output_handler(timer_drv_output_evt_type_t event, 
				uint8_t * p_data, uint16_t data_len)
{
	switch (event)
	{
		case USER_TIMEOUT_TIMER_OVF:
		{
			if(brc_main.p_current_dev != NULL)
			{
				brc_main.p_current_dev->interface->vptr_dev->event(
							(void *)brc_main.p_current_dev,
							NULL, NULL, USER_TIMEOUT_OVF_EVT);
			}
		}	break;

		case TIMESLOT_TIMER_OVF:
		{
			if(brc_main.p_current_dev != NULL)
				brc_main.p_current_dev->interface->vptr_dev->execute(
						(void *)&brc_main, NULL, NULL, NULL);
		}	break;
		
		case GET_STATUS_TIMER_OVF:
		{
			if(brc_main.p_current_dev != NULL)
			{
				brc_main.p_current_dev->interface->vptr_dev->event(
							(void *)&brc_main.p_current_dev,
							NULL, NULL, DEV_POLL_TIMER_RELOAD);
			}
		}	break;

		case AUTO_SLEEP_TIMER_OVF:
		{
			uint32_t err_code; 
			brc_main.p_lcd->deinit((void *)brc_main.p_lcd);
			nrf_gpio_cfg_output(0);
			nrf_gpio_pin_clear(0);
			nrf_delay_ms(100);
			// Go to system-off mode (this function will not return; wakeup will cause
			// a reset).
			err_code = sd_power_system_off();
			APP_ERROR_CHECK(err_code);
		}	break;

		case MISC_TIMER_OVF:
		{
			misc_timeout_timer_id_handler((void *)p_data);
		}	break;
		case CONT_PARAM_CHANGE_TIMER_OVF:
		{
			t_button_id = *p_data;
			p_t_button_id = p_data;
			switch (t_button_id)
			{
				case BTN_SPUP_ID:
				{
					CURRENT_BUTTON_EVT(fan_up, p_data);
				}	break;
				case BTN_SPDW_ID:
				{
					CURRENT_BUTTON_EVT(fan_down, p_data);
				}	break;
				case BTN_TUP_ID:
				{
					CURRENT_BUTTON_EVT(temp_up, p_data);
				}	break;
				case BTN_TDW_ID:
				{
					CURRENT_BUTTON_EVT(temp_down, p_data);
				}	break;
				default:
					break;
			}
			SET_AUTO_SLEEP_TIMER_INSTANCE(auto_sleep, NULL);
			p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&auto_sleep, TIMER_CONFIG_SIZE);
			brc_main.p_current_dev->interface->vptr_dev->event((void *)&brc_main,
							NULL, NULL, BUTTON_EVT);
		}	break;
		case BUTTON_TIMER_OVF:
		{
			if(p_data != NULL)
			{
				*p_data = 0;
			}
		}	break;
		default:
		{
		}	break;
	}
}


static void misc_timeout_timer_id_handler (void * p_context)
{
	event_data_t b_local_evt_data;

	b_local_evt_data = *(event_data_t *)p_context;

	switch (b_local_evt_data.evt_type)
	{
		case DEV_SWITCH:
		{
			brc_main.p_current_dev->interface->vptr_dev->deinit(NULL);
			brc_main.p_current_dev = &brc_main.dev_list[b_local_evt_data.dev_type];
			if(b_local_evt_data.dev_type == DEFAULT_DEV)
			{
				brc_main.p_current_dev->interface->vptr_dev->init((void *)brc_main.p_current_dev
					, (void *)&brc_main, 1, (void *)&dev_event_callback);
				SET_GET_STATUS_TIMER_INSTANCE(get_state, NULL);
				p_timer_drv_input_handler(STOP_TIMER, 
							(uint8_t *)&get_state, TIMER_CONFIG_SIZE);
			}
			else
				brc_main.p_current_dev->interface->vptr_dev->init((void *)brc_main.p_current_dev
					, (void *)&brc_main, 0, (void *)&dev_event_callback);
			
				SET_AUTO_SLEEP_TIMER_INSTANCE(auto_sleep, NULL);
				p_timer_drv_input_handler(RELOAD_TIMER, 
							(uint8_t *)&auto_sleep, TIMER_CONFIG_SIZE);
				SET_USER_TIMEOUT_TIMER_INSTANCE(user_timeout, NULL);
				p_timer_drv_input_handler(RELOAD_TIMER, 
							(uint8_t *)&user_timeout, TIMER_CONFIG_SIZE);
		}	break;
		case INVALID_SPEED:
		{
			brc_main.p_current_dev->interface->vptr_dev->event((void *)&brc_main,
										 NULL, NULL, INVALID_SPEED_TIMEOUT_OVF_EVT);
		}	break;

		case REPORT_BRC_FW_TIMEOUT_OVF:
		{
			if (brc_main.p_current_dev != &brc_main.dev_list[DEFAULT_DEV])
			{
				sleep_mode_enter(0);
			}
			else
			{
				brc_main.p_current_dev->interface->vptr_dev->event((void *)&brc_main,
										 NULL, NULL, REPORT_BRC_FW_TIMEOUT_OVF);
			}
		}	break;

		case SCREEN_TEST_TIMEOUT_OVF:
		{
			if (brc_main.p_current_dev != &brc_main.dev_list[DEFAULT_DEV])
			{
				sleep_mode_enter(0);
			}
			else
			{
				brc_main.p_current_dev->interface->vptr_dev->event((void *)&brc_main,
										 NULL, NULL, SCREEN_TEST_TIMEOUT_OVF);
			}
		}	break;

		default:
			break;
	}
}

static void dev_poll_timer_id_handler (void * p_context)
{
	if(brc_main.p_current_dev != NULL)
	{
		brc_main.p_current_dev->interface->vptr_dev->event((void *)&brc_main,
										 NULL, NULL, DEV_POLL_TIMER_RELOAD);
	}
}

static uint32_t ble_rx_callback(ble_rx_t * p_rx_data)
{
	uint32_t err_code = NRF_SUCCESS;

	switch (p_rx_data->evt_type)
	{
		case RX_DATA:
		{
			brc_main.p_current_dev->interface->vptr_protocol->input_raw_data(
							(void *)p_rx_data->p_rx_buff, p_rx_data->lenght);
			return err_code = NRF_SUCCESS;
		}	break;
		
		case CONNECTION:
		{
			brc_main.p_current_dev->interface->vptr_dev->event(
					(void *)brc_main.p_current_dev,	(void *)p_rx_data, 
					NULL, BLE_EVT);
			return err_code = NRF_SUCCESS;
		}	break;
		case CLEAR_W_LIST:
		{
			brc_main.expected_dev_type = NO_VALUE;
			ud_m_list.ud_deinit((void*)&brc_main);
			brc_main.p_current_dev->interface->vptr_dev->event(
					(void *)brc_main.p_current_dev,	(void *)p_rx_data, 
					NULL, BLE_BONDS_ERASED);
			brc_main.p_core_ble->sys.disconnect(NULL);
		}	break;
		
		case CLEAR_W_LIST_FAIL:
		{
			brc_main.p_core_ble->sys.erase_bonds(NULL);
		}	break;

		case FOUND_CONNECTABLE_SUPPORTED_DEV:
		{
			brc_main.p_current_dev->interface->vptr_dev->event(
					(void *)brc_main.p_current_dev,	(void *)p_rx_data, 
					NULL, BLE_EVT);
		}	break;
		
		case DISCONNECTION:
		{
			switch_active_device(DEFAULT_DEV, 500);
		}	break;
		default:
			break;
	}
	return err_code;
}

static uint32_t dev_event_callback(void * p_context,
						void * p_arg, uint32_t length, uint32_t evt)
{
	uint32_t err_code = NRF_SUCCESS;
	static event_data_t b_local_evt_data = {0};
	switch (evt)
	{
		case USER_TIMEOUT_RELOAD_EVT:
		{
			SET_USER_TIMEOUT_TIMER_INSTANCE(user_timeout, p_context);
			p_timer_drv_input_handler(RELOAD_TIMER, 
								(uint8_t *)&user_timeout, TIMER_CONFIG_SIZE);
		}	break;
		case USER_MENU_TIMEOUT_RELOAD_EVT:
		{
			SET_USER_MENU_TIMEOUT_TIMER_INSTANCE(user_timeout, p_context);
			p_timer_drv_input_handler(RELOAD_TIMER, 
								(uint8_t *)&user_timeout, TIMER_CONFIG_SIZE);
		}	break;
		case SLEEP_EVT:
		{
			sleep_block = true;
			button_block = true;
			sleep_mode_enter(*(uint32_t *)p_arg);
		}	break;
		case FOUND_BRLT:
		{
			if (b_local_evt_data.dev_type == DEFAULT_DEV ||
				b_local_evt_data.dev_type == NO_VALUE)
			{
			switch_active_device(BRLT_DEV, *(uint32_t *)p_arg);
			if(brc_main.expected_dev_type == NO_VALUE)
			{
				brc_main.expected_dev_type = BRLT_DEV;
				ud_format_t b_local_ud;
				b_local_ud.dev_id = BRLT_DEV;
				ud_m_list.ud_write((void*)&brc_main, &b_local_ud);
			}
			}
		}	break;
		case FOUND_BR3S:
		{
			if (b_local_evt_data.dev_type == DEFAULT_DEV ||
				b_local_evt_data.dev_type == NO_VALUE)
			{
			switch_active_device(BR3S_DEV, *(uint32_t *)p_arg);
			if(brc_main.expected_dev_type == NO_VALUE)
			{
				brc_main.expected_dev_type = BR3S_DEV;
				ud_format_t b_local_ud;
				b_local_ud.dev_id = BR3S_DEV;
				ud_m_list.ud_write((void*)&brc_main, &b_local_ud);				
			}
			}
		}	break;
		case FOUND_BR4S:
		{
			if (b_local_evt_data.dev_type == DEFAULT_DEV ||
				b_local_evt_data.dev_type == NO_VALUE)
			{
			switch_active_device(BR4S_DEV, *(uint32_t *)p_arg);
			if(brc_main.expected_dev_type == NO_VALUE)
			{
				brc_main.expected_dev_type = BR4S_DEV;
				ud_format_t b_local_ud;
				b_local_ud.dev_id = BR4S_DEV;
				ud_m_list.ud_write((void*)&brc_main, &b_local_ud);				
			}
			}
		}	break;
		case BLE_DISCONNECT:
		{
			brc_main.p_core_ble->sys.disconnect(NULL);
		}	break;
		case BLE_ERASE_BONDS:
		{
			//brc_main.p_core_ble->sys.disconnect(NULL);
			brc_main.p_core_ble->sys.erase_bonds(NULL);
		}	break;
		case BLE_START_PAIRING:
		{
			brc_main.p_core_ble->sys.pair(NULL);
		}	break;
		case BLE_SEND_DATA:
		{
			data_t b_local_data;
			b_local_data.p_data = (uint8_t *)p_arg;
			b_local_data.data_len = (uint16_t)length;
			brc_main.p_core_ble->write((void *)&b_local_data);
		}	break;
		case START_DEV_POLL:
		{
			SET_GET_STATUS_TIMER_INSTANCE(get_status, NULL);
			p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&get_status, TIMER_CONFIG_SIZE);
		}	break;
		case STOP_DEV_POLL:
		{
			SET_GET_STATUS_TIMER_INSTANCE(get_status, NULL);
			p_timer_drv_input_handler(STOP_TIMER, 
						(uint8_t *)&get_status, TIMER_CONFIG_SIZE);
		}	break;
		case START_CONT_PARAM_CHANGE_TIMER:
		{	
			SET_CONT_PARAM_CHANGE_TIMER_INSTANCE(cont_param_change, p_arg);
			p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&cont_param_change, TIMER_CONFIG_SIZE);
		}	break;
		case STOP_CONT_PARAM_CHANGE_TIMER:
		{	
			SET_CONT_PARAM_CHANGE_TIMER_INSTANCE(cont_param_change,  p_arg);
			p_timer_drv_input_handler(STOP_TIMER, 
						(uint8_t *)&cont_param_change, TIMER_CONFIG_SIZE);
		}	break;
		case INVALID_SPEED:
		{	
			b_local_evt_data.dev_type = 0;
			b_local_evt_data.evt_type = INVALID_SPEED;

			SET_INVALID_SPEED_TIMER_INSTANCE(invalid_speed, &b_local_evt_data);
			p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&invalid_speed, TIMER_CONFIG_SIZE);
		}	break;

		case START_SCREEN_TEST:
		{
			brc_main.p_core_ble->sys.scan_forbid((void *)&brc_main);
			brc_main.p_core_ble->sys.disconnect((void *)&brc_main);
			if (brc_main.p_current_dev != &brc_main.dev_list[DEFAULT_DEV])
			{
				switch_active_device(DEFAULT_DEV, 1000);
				brc_main.test_mode = 1;
			}
			else
			{
				brc_main.dev_list[DEFAULT_DEV].interface->vptr_dev->event((void *)&brc_main,
						NULL, NULL, START_SCREEN_TEST);
			}
		}	break;

		case REPORT_BRC_FW_TIMEOUT_RELOAD:
		{
			b_local_evt_data.dev_type = 0;
			b_local_evt_data.evt_type = REPORT_BRC_FW_TIMEOUT_OVF;

			SET_BRC_REPORT_FW_TIMER_INSTANCE(brc_report_fw, &b_local_evt_data);
			p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&brc_report_fw, TIMER_CONFIG_SIZE);
		}	break;

		case SCREEN_TEST_TIMEOUT_RELOAD:
		{
			b_local_evt_data.dev_type = 0;
			b_local_evt_data.evt_type = SCREEN_TEST_TIMEOUT_OVF;

			SET_SCREEN_TEST_TIMER_INSTANCE(screen_test, &b_local_evt_data);
			p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&screen_test, TIMER_CONFIG_SIZE);
		}	break;
		case BUTTON_EVT_BLOCK:
		{
			SET_BUTTON_TIMER_INSTANCE(button_block_tmr,p_arg);
			p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&button_block, TIMER_CONFIG_SIZE);
		}	break;
		case BUTTON_EVT_BLOCK_OVF:
		{
			button_block = 0;
		}	break;
		default:
			break;
	}
	return err_code;
}

static void switch_active_device(uint32_t dev_type, uint32_t timeout)
{
	static event_data_t b_local_evt_data = {0};
	
	button_block = true;
	b_local_evt_data.dev_type = (uint16_t)dev_type;
	b_local_evt_data.evt_type = DEV_SWITCH;
	SET_MISC_TIMER_INSTANCE(switch_dev, timeout, 
						(void *)&b_local_evt_data);
	p_timer_drv_input_handler(RELOAD_TIMER, 
				(uint8_t *)&switch_dev, TIMER_CONFIG_SIZE);
}

static void sleep_mode_enter(uint32_t timeout)
{
	if (timeout == 0)
	{
		uint32_t err_code;
		ble_read_out_t b_ble_read_out;
		
		nrf_delay_ms(1000);
		b_ble_read_out.read_type = SYS_READ;
		brc_main.p_core_ble->read((void *)&b_ble_read_out);
		if (b_ble_read_out.p_ble_sys->m_active_dev_type != DEFAULT_DEV)
			brc_main.p_core_ble->sys.disconnect((void *)&brc_main);
		else
			brc_main.p_core_ble->sys.scan_forbid((void *)&brc_main);
		
		brc_main.p_lcd->deinit((void *)brc_main.p_lcd);
		nrf_gpio_cfg_output(0);
		nrf_gpio_pin_clear(0);
		nrf_delay_ms(100);
		// Go to system-off mode (this function will not return; wakeup will cause
		// a reset).
		err_code = sd_power_system_off();
		APP_ERROR_CHECK(err_code);
	}
	brc_main.p_core_ble->sys.disconnect(NULL);
	SET_SLEEP_TIMER_INSTANCE(sleep_mode, timeout, NULL);
	p_timer_drv_input_handler(RELOAD_TIMER, 
				(uint8_t *)&sleep_mode, TIMER_CONFIG_SIZE);
}

static void power_manage(void)
{
	uint32_t err_code = sd_app_evt_wait();

	APP_ERROR_CHECK(err_code);
}

static void button_event_handler(uint32_t evt,	uint8_t * p_data, uint32_t data_len)
{
	if (button_block)
	{
		return;
	}
	
	switch (evt)
	{
		case POWER_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(power, NULL);
			break;

		case STATE_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(state, NULL);
			break;

		case SET_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(set, NULL);
			break;

		case GATE_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(gate, NULL);
			break;

		case TEMP_UP_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(temp_up, NULL);
			break;

		case TEMP_DOWN_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(temp_down, NULL);
			break;

		case FAN_UP_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(fan_up, NULL);
			break;

		case FAN_DOWN_BUTTON_RELEASE:
			CURRENT_BUTTON_EVT(fan_down, NULL);
			break;

		case FAN_UP_BUTTON_LONG_PUSH:
			CURRENT_BUTTON_EVT(long_fan_up, NULL);
			break;

		case FAN_DOWN_BUTTON_LONG_PUSH:
			CURRENT_BUTTON_EVT(long_fan_down, NULL);
			break;

		case TEMP_UP_BUTTON_LONG_PUSH:
			CURRENT_BUTTON_EVT(long_temp_up, NULL);
			break;

		case TEMP_DOWN_BUTTON_LONG_PUSH:
			CURRENT_BUTTON_EVT(long_temp_down, NULL);
			break;

		case SMLT_LONG_PUSH_STATE_SET_GATE:
			CURRENT_BUTTON_EVT(long_state_set_gate, NULL);
			break;

		case SMLT_LONG_PUSH_STATE_SET_FAN_UP_FAN_DOWN:
			CURRENT_BUTTON_EVT(long_state_set_fan_up_fan_down, NULL);
			break;

		case SMLT_LONG_PUSH_STATE_SET:
			CURRENT_BUTTON_EVT(long_state_set, NULL);
			break;

		case SMLT_LONG_PUSH_TEMP_DOWN_TEMP_UP_SET:
			CURRENT_BUTTON_EVT(long_temp_up_temp_down, NULL);
			break;

		case SMLT_PUSH_TEMP_DOWN_TEMP_UP:
			CURRENT_BUTTON_EVT(state, NULL);
			break;

		case SMLT_LONG_PUSH_GATE_SET_TEMP_DOWN_SPEED_UP:
			CURRENT_BUTTON_EVT(long_gate_set_temp_down_fan_up, NULL);
			break;

		case SMLT_LONG_PUSH_STATUS_POWER:
			CURRENT_BUTTON_EVT(long_state_power, NULL);
			break;
			
		default:
			break;
	}
	
	/*Next statement must be after execute of button events, 
	cause if before - it First set Timers, and after go 
	to currect menu. This leads to https://youtrack.tiondev.ru/issue/URC-73*/
	if(evt != 0) 
	{
		if(!sleep_block)
		{
			SET_AUTO_SLEEP_TIMER_INSTANCE(auto_sleep, NULL);
			p_timer_drv_input_handler(RELOAD_TIMER, (uint8_t *)&auto_sleep,  
									  	TIMER_CONFIG_SIZE);
			brc_main.p_current_dev->interface->vptr_dev->event((void *)&brc_main,
										NULL, NULL, BUTTON_EVT);
		}
	}
}

int main(void)  
{  
	bool check_button = 0;
	nrf_gpio_cfg_output(0);
	nrf_gpio_pin_set(0);
	nrf_delay_ms(5);
	
	brc_main.dev_list[DEFAULT_DEV].interface = remote_get_interface();
	brc_main.dev_list[DEFAULT_DEV].p_adv_data_parser = remote_get_p_adv_data();
	brc_main.dev_list[BRLT_DEV].interface = brlt_get_interface();
	brc_main.dev_list[BRLT_DEV].p_adv_data_parser = brlt_get_p_adv_data();
	brc_main.dev_list[BR3S_DEV].interface = br3s_get_interface();
	brc_main.dev_list[BR3S_DEV].p_adv_data_parser = br3s_get_p_adv_data();
	brc_main.dev_list[BR4S_DEV].interface = br4s_get_interface();
	brc_main.dev_list[BR4S_DEV].p_adv_data_parser = br4s_get_p_adv_data();

	brc_main.test_mode = 0;
	
	brc_main.p_core_ble->init((void *)&brc_main, &ble_rx_callback);
	
	p_timer_drv_input_handler = timer_drv_init(&timer_drv_output_handler);
	
	check_button = check_all_buttons();
	
	my_button_init(button_event_handler);

	brc_main.p_lcd_g_ram = brc_lcd_indication_get_g_ram();

	brc_main.p_lcd->init((void *)brc_main.p_lcd, NULL, NULL);	

	if (ud_m_list.ud_init((void*)&brc_main) != FDS_SUCCESS)
	{
		brc_main.expected_dev_type = NO_VALUE;
	}
	else 
	{
		ud_format_t b_local_ud;
		ret_code_t local_ret = ud_m_list.ud_read((void*)&brc_main, &b_local_ud);
		if (local_ret != FDS_SUCCESS)
		{
			brc_main.expected_dev_type = NO_VALUE;
			ud_format_t b_local_ud;
			b_local_ud.dev_id = NO_VALUE;
			if (local_ret == FDS_ERR_NOT_FOUND)
			{
				ud_m_list.ud_create((void*)&brc_main, &b_local_ud);
			}
			else
			{
				ud_m_list.ud_write((void*)&brc_main, &b_local_ud);
			}
		}
		else
		{
			brc_main.expected_dev_type = b_local_ud.dev_id;
		}
	}
	
	brc_main.p_current_dev = &brc_main.dev_list[DEFAULT_DEV];

	brc_main.p_current_dev->interface->vptr_dev->init((void *)brc_main.p_current_dev,
				(void *)&brc_main, check_button, (void *)&dev_event_callback);
	SET_TIMESLOT_TIMER_INSTANCE(time_slot, NULL);
	p_timer_drv_input_handler(RELOAD_TIMER, 
						(uint8_t *)&time_slot, TIMER_CONFIG_SIZE);
	for (;;)
	{
		power_manage();
	}
}
