/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "remote_main.h"
/*Private defines*/
// clang-format off
// clang-format on
/*Private types*/

/*Private functions prototypes*/
static uint32_t menu_clear(void * p_arg, uint32_t length);

static uint32_t menu_ble_conn_attempt(void * p_arg, uint32_t length);
static uint32_t menu_ble_conn_fail(void * p_arg, uint32_t length);
static uint32_t menu_ble_pair_attempt(void * p_arg, uint32_t length);
static uint32_t menu_ble_pair_fail(void * p_arg, uint32_t length);
static uint32_t menu_ble_erase_bonds(void * p_arg, uint32_t length);
static uint32_t menu_ble_found_brlt(void * p_arg, uint32_t length);
static uint32_t menu_ble_found_br3s(void * p_arg, uint32_t length);
static uint32_t menu_ble_found_br4s(void * p_arg, uint32_t length);
static uint32_t menu_ble_brc_fw_report(void * p_arg, uint32_t length);
static uint32_t menu_ble_screen_test(void * p_arg, uint32_t length);

static uint32_t init_device_instance(void * p_context, 
						void * p_arg, uint32_t length);
static uint32_t deinit_device_instance(void * p_context);

static uint32_t execute(void * p_context, 
						void * p_arg, uint32_t length);

/*Exported constants*/
const menu_m_list_t remote_menu_vptr = {
	.init = &init_device_instance,
	.deinit = &deinit_device_instance,
	.execute = &execute,
};
/*Private variables*/
static set_indication_t		remote_menu_ble_m[REMOTE_BLE_M_NUM] = 
{
	menu_clear,
	menu_ble_conn_attempt,
	menu_ble_conn_fail,
	menu_ble_pair_attempt,
	menu_ble_pair_fail,
	menu_ble_erase_bonds,
	menu_ble_found_brlt,
	menu_ble_found_br3s,
	menu_ble_found_br4s,
};

static set_indication_t		remote_menu_init_m[REMOTE_INIT_M_NUM] = 
{
	menu_clear,
	menu_ble_brc_fw_report,
	menu_ble_screen_test,
};
static set_indication_t		remote_menu_off_m[REMOTE_OFF_M_NUM] = 
{
	menu_clear,
};


static remote_menu_state_t remote_menu_state = 
{
	.menu[REMOTE_MENU_INIT_ID].size = REMOTE_INIT_M_NUM,
	.menu[REMOTE_MENU_INIT_ID].p_menu_sub_m_list = remote_menu_init_m,
	.menu[REMOTE_MENU_BLE_ID].size = REMOTE_BLE_M_NUM,
	.menu[REMOTE_MENU_BLE_ID].p_menu_sub_m_list = remote_menu_ble_m,
	.menu[REMOTE_MENU_OFF_ID].size = REMOTE_OFF_M_NUM,
	.menu[REMOTE_MENU_OFF_ID].p_menu_sub_m_list = remote_menu_off_m,
};
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/ 

/*Private functions*/
static uint32_t menu_clear(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	brc_lcd_g_ram_t b_local_g_ram = {0};
	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_ble_conn_attempt(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	switch (remote_menu_state.p_brc_main->expected_dev_type)
	{
		case BRLT_DEV:
		{
			p_str = (uint8_t *)(REMOTE_FOUND_BRLT_STR);
		}	break;
		case BR3S_DEV:
		{
			p_str = (uint8_t *)(REMOTE_FOUND_BR3S_STR);
		}	break;
		case BR4S_DEV:
		{
			p_str = (uint8_t *)(REMOTE_FOUND_BR4S_STR);
		}	break;
		case NO_VALUE:
		default:
		{
			p_str = (uint8_t *)(REMOTE_CONN_STR);
		}	break;
	}
	
	b_local_g_ram.blink.state = SEG_ON;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_BLINK;
	}
	
	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));

	return ret_val;
}

static uint32_t menu_ble_conn_fail(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(REMOTE_NO_CONN_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	
	return ret_val;
}

static uint32_t menu_ble_pair_attempt(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(REMOTE_PAIR_STR);
	b_local_g_ram.blink.state = SEG_ON;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_BLINK;
	}

	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_ble_pair_fail(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(REMOTE_NO_PAIR_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_ble_erase_bonds(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(REMOTE_ERASE_BONDS_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_ble_found_brlt(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(REMOTE_FOUND_BRLT_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_ble_found_br3s(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(REMOTE_FOUND_BR3S_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_ble_found_br4s(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(REMOTE_FOUND_BR4S_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_ble_brc_fw_report(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};

	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	char b_big_str[BIG_LINE_LEN+1];
	snprintf(b_big_str, 6,"F%04X", SW_VERSION);
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = b_big_str[cnt];
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}
	ret_val = remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));
	return ret_val;
}

static uint32_t menu_ble_screen_test(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint32_t timeout;
	static uint32_t	entry_cnt = 0; 
	static uint8_t 	test_type = SMALL_LINE_TEST_ID;
	
	static brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t print_char;

	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;

	/*uint8_t b_big_str[BIG_LINE_LEN+1];
	uint8_t b_small_str[SMALL_LINE_LEN+1];*/
	switch (test_type)
	{
		case SMALL_LINE_TEST_ID:
		{
			if (entry_cnt < (SEG_G - SEG_CELSIUM))
			{
				print_char = SEG_A + entry_cnt;
				for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
				{
					b_local_g_ram.small_line[cnt].value = print_char;
					b_local_g_ram.small_line[cnt].state = SEG_ON;
				}
			}
			else
			{
				test_type++;
				entry_cnt = 0;
			}
		}	break;

		case BIG_LINE_TEST_ID:
		{
			if (entry_cnt < (SEG_G - SEG_CELSIUM))
			{
				print_char = SEG_A + entry_cnt;
				for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
				{
					b_local_g_ram.big_line[cnt].value = print_char;
					b_local_g_ram.big_line[cnt].state = SEG_ON;
				}
			}
			else
			{
				test_type++;
				entry_cnt = 0;
			}
		}	break;

		case SPEED_LINE_TEST_ID:
		{
			if (entry_cnt < NUM_OF_SPEED_SEG)
			{
				for(uint8_t cnt = 0; cnt <= entry_cnt; cnt++)
				{
					b_local_g_ram.speed_line[cnt].state = SEG_ON;
					b_local_g_ram.speed_line[cnt].value = SEG_ON;
				}
			}
			else
			{
				test_type++;
				entry_cnt = 0;
			}
		}	break;

		case HEATER_PIC_TEST_ID:
		{
			if (entry_cnt < NUM_OF_HEAT_SEG)
			{
				for(uint8_t cnt = 0; cnt <= entry_cnt; cnt++)
				{
					b_local_g_ram.heater_pic[cnt].state = SEG_ON;
					b_local_g_ram.heater_pic[cnt].value = SEG_ON;
				}
			}
			else
			{
				test_type++;
				entry_cnt = 0;
			}
		}	break;

		case GATE_PIC_TEST_ID:
		{
			if (entry_cnt < NUM_OF_GATE_SEG)
			{
				for(uint8_t cnt = 0; cnt <= entry_cnt; cnt++)
				{
					b_local_g_ram.gate_pic[cnt].state = SEG_ON;
					b_local_g_ram.gate_pic[cnt].value = SEG_ON;
				}
			}
			else
			{
				test_type++;
				entry_cnt = 0;
			}
		}	break;

		case MA_ICON_TEST_ID:
		{
			b_local_g_ram.ma_icon.state = SEG_ON;
			b_local_g_ram.ma_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case FAN_ICON_TEST_ID:
		{
			b_local_g_ram.fan_icon.state = SEG_ON;
			b_local_g_ram.fan_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case BREEZER_ICON_TEST_ID:
		{
			b_local_g_ram.breezer_icon.state = SEG_ON;
			b_local_g_ram.breezer_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case INPUT_ARROW_ICON_TEST_ID:
		{
			b_local_g_ram.input_arrow_icon.state = SEG_ON;
			b_local_g_ram.input_arrow_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case OUTPUT_ARROW_ICON_TEST_ID:
		{
			b_local_g_ram.output_arrow_icon.state = SEG_ON;
			b_local_g_ram.output_arrow_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case AUTO_ICON_TEST_ID:
		{
			b_local_g_ram.auto_icon.state = SEG_ON;
			b_local_g_ram.auto_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case DEGREE_ICON_TEST_ID:
		{
			b_local_g_ram.degree_icon.state = SEG_ON;
			b_local_g_ram.degree_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case FILTER_ICON_TEST_ID:
		{
			b_local_g_ram.filter_icon.state = SEG_ON;
			b_local_g_ram.filter_icon.value = SEG_ON;
			test_type++;
			entry_cnt = 0;
		}	break;

		case ALARM_CLOCK_ICON_TEST_ID:
		{
			b_local_g_ram.alarm_clock_icon.state = SEG_ON;
			b_local_g_ram.alarm_clock_icon.value = SEG_ON;
			test_type = END_OF_SCREEN_TEST;
			entry_cnt = 0;
		}	break;

		case END_OF_SCREEN_TEST:
		{
			if(entry_cnt >= 20)
			{
				timeout = 0;
				remote_menu_state.p_remote_callback((void *)&remote_menu_state, 
						(void *)&timeout, NULL, SLEEP_EVT);
			}
		}	break;
		default:
			break;
	}

	ret_val = remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));
	if(ret_val == NRF_SUCCESS)
	{
		entry_cnt++;
	}
	/*while(NRF_SUCCESS != remote_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));*/
	return ret_val;
}

/*class methods section*/
static uint32_t init_device_instance(void * p_context, 
						void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	remote_main_t * p_local_remote_main;
	p_local_remote_main = (remote_main_t *)p_context;
	remote_menu_state.p_brc_main = p_local_remote_main->p_brc_main;
	remote_menu_state.p_remote_callback	= p_local_remote_main->p_remote_callback;
	menu_clear(NULL, NULL);
	remote_menu_state.state = REMOTE_MENU_INIT_ID;
	remote_menu_state.sub_state = REMOTE_INIT_CLEAR_M_ID;
	
	return ret_val;
}

static uint32_t deinit_device_instance(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;

	menu_clear(NULL, NULL);
	remote_menu_state.state = REMOTE_MENU_INIT_ID;
	remote_menu_state.sub_state = REMOTE_INIT_CLEAR_M_ID;
	remote_menu_state.p_remote_callback = NULL;
	remote_menu_state.p_brc_main = NULL;
	return ret_val;
}

static uint32_t execute(void * p_context, 
						void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint32_t b_state_idx, b_sub_state_idx;

	b_state_idx = remote_menu_state.state;
	b_sub_state_idx = remote_menu_state.sub_state;
	if ((b_state_idx < REMOTE_MENU_SIZE) &&
		(b_sub_state_idx < remote_menu_state.menu[b_state_idx].size))
	{
		remote_menu_state.menu[b_state_idx].p_menu_sub_m_list[b_sub_state_idx](NULL,NULL);
	}
	else
	{
		ret_val = NRF_ERROR_INVALID_STATE;
		menu_clear(NULL, NULL);
		remote_menu_state.state = REMOTE_MENU_INIT_ID;
		remote_menu_state.sub_state = REMOTE_INIT_CLEAR_M_ID;
	}

	return ret_val;
}
/*Public functions*/
remote_menu_state_t * remote_menu_get_mode(void)
{
	return &remote_menu_state;
}

/******************************* END_OF_FILE **********************************/