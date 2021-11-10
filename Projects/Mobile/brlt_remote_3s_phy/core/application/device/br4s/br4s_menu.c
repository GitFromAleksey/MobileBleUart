/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "br4s_main.h"
#include "device_interface.h"
/*Private defines*/
// clang-format off
#define HEATER_ICON_TRESHOLD 	15

#define CONVERT_SEC_TO_DAYS(_target_, _source_)		\
	do{ if (_source_%(60*60*24))					\
		{_target_ = (_source_/(60*60*24)) + 1;}		\
		else										\
		{_target_ = _source_/(60*60*24);}}while(0)	
// clang-format on
/*Private types*/

/*Private functions prototypes*/
static uint32_t menu_clear(void * p_arg, uint32_t length);
static uint32_t menu_init_erase(void * p_arg, uint32_t length);
static uint32_t menu_report_dev(void * p_arg, uint32_t length);
static uint32_t menu_dummy_screen(void * p_arg, uint32_t length);

static uint32_t menu_on_default(void * p_arg, uint32_t length);
static uint32_t menu_on_set(void * p_arg, uint32_t length);
static uint32_t menu_on_error(void * p_arg, uint32_t length);

static uint32_t menu_off_default(void * p_arg, uint32_t length);
/*static uint32_t menu_off_error(void * p_arg, uint32_t length);*/

static uint32_t menu_set_default_on_state (brc_lcd_g_ram_t * p_g_ram);

static uint32_t init_device_instance(void * p_context, 
						void * p_arg, uint32_t length);
static uint32_t deinit_device_instance(void * p_context);

static uint32_t execute(void * p_context, 
						void * p_arg, uint32_t length);

static uint32_t menu_set_default_on_t_out_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_t_in_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_time_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_t_set_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_h_off_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_t_out_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_dev_rep_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_dev_fw_rep_state (brc_lcd_g_ram_t * p_g_ram);


static uint32_t menu_on_set_filter_time (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_timer_off (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_timer_on (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_timer_off_time (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_timer_on_time (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_time (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_heater_off (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_heater_on_t_set (brc_lcd_g_ram_t * p_g_ram);

static uint32_t menu_on_set_time_blink_format(brc_lcd_g_ram_t * p_g_ram);
/*Exported constants*/
const menu_m_list_t br4s_menu_vptr = {
	.init = &init_device_instance,
	.deinit = &deinit_device_instance,
	.execute = &execute,
};
/*Private variables*/
static set_indication_t		br4s_menu_init_m[BR4S_INIT_M_NUM] = 
{
	menu_clear,
	menu_init_erase,
	menu_report_dev,
	menu_dummy_screen,
};

static set_indication_t		br4s_menu_on_m[BR4S_ON_M_NUM] = 
{
	menu_clear,
	menu_on_default,
	menu_on_set,
	menu_on_error,
};

static set_indication_t		br4s_menu_off_m[BR4S_OFF_M_NUM] = 
{
	menu_clear,
	menu_off_default,
	menu_on_error,
};

static br4s_menu_state_t br4s_menu_state = 
{
	.menu[BR4S_MENU_INIT_ID].size = BR4S_INIT_M_NUM,
	.menu[BR4S_MENU_INIT_ID].p_menu_sub_m_list = br4s_menu_init_m,
	.menu[BR4S_MENU_ON_ID].size = BR4S_ON_M_NUM,
	.menu[BR4S_MENU_ON_ID].p_menu_sub_m_list = br4s_menu_on_m,
	.menu[BR4S_MENU_OFF_ID].size = BR4S_OFF_M_NUM,
	.menu[BR4S_MENU_OFF_ID].p_menu_sub_m_list = br4s_menu_off_m,
};
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/ 

/*Private functions*/
static uint32_t menu_on_set_clear_screen (void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	if(length != sizeof(brc_lcd_g_ram_t))
	{
		ret_val = NRF_ERROR_INVALID_LENGTH;
		return ret_val;
	}
	brc_lcd_g_ram_t * p_local_g_ram;
	p_local_g_ram = (brc_lcd_g_ram_t *)p_arg;
	
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_local_g_ram->small_line[cnt].value = SEG_OFF;
		p_local_g_ram->small_line[cnt].state = SEG_OFF;
	}
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_local_g_ram->big_line[cnt].value = SEG_OFF;
		p_local_g_ram->big_line[cnt].state = SEG_OFF;
	}
	for(uint8_t cnt = 0; cnt < NUM_OF_SPEED_SEG; cnt++)
	{
		p_local_g_ram->speed_line[cnt].value = SEG_OFF;
		p_local_g_ram->speed_line[cnt].state = SEG_OFF;
	}
	for(uint8_t cnt = 0; cnt < HEATER_PIC_LEN; cnt++)
	{
		p_local_g_ram->heater_pic[cnt].value = SEG_OFF;
		p_local_g_ram->heater_pic[cnt].state = SEG_OFF;
	}
	for(uint8_t cnt = 0; cnt < HEATER_PIC_LEN; cnt++)
	{
		p_local_g_ram->gate_pic[cnt].value = SEG_OFF;
		p_local_g_ram->gate_pic[cnt].state = SEG_OFF;
	}
	p_local_g_ram->ma_icon.state = SEG_OFF;
	p_local_g_ram->ma_icon.value = SEG_OFF;
	p_local_g_ram->fan_icon.state = SEG_OFF;
	p_local_g_ram->fan_icon.value = SEG_OFF;
	p_local_g_ram->breezer_icon.state = SEG_OFF;
	p_local_g_ram->breezer_icon.value = SEG_OFF;
	p_local_g_ram->input_arrow_icon.state = SEG_OFF;
	p_local_g_ram->input_arrow_icon.value = SEG_OFF;
	p_local_g_ram->output_arrow_icon.state = SEG_OFF;
	p_local_g_ram->output_arrow_icon.value = SEG_OFF;
	p_local_g_ram->auto_icon.state = SEG_OFF;
	p_local_g_ram->auto_icon.value = SEG_OFF;
	p_local_g_ram->degree_icon.state = SEG_OFF;
	p_local_g_ram->degree_icon.value = SEG_OFF;
	p_local_g_ram->filter_icon.state = SEG_OFF;
	p_local_g_ram->filter_icon.value = SEG_OFF;
	p_local_g_ram->alarm_clock_icon.state = SEG_OFF;
	p_local_g_ram->alarm_clock_icon.value = SEG_OFF;	

	return ret_val;
}

static uint32_t menu_set_default_on_t_out_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"%3d%c", (br4s_menu_state.p_br4s_state->dev_info.flags.heater_present) ? 
                 br4s_menu_state.p_br4s_state->dev_info.sensors_state.out_temp : br4s_menu_state.p_br4s_state->dev_info.sensors_state.in_temp, SEG_CELSIUM);
	p_g_ram->output_arrow_icon.state = SEG_ON;
	p_g_ram->output_arrow_icon.value = SEG_ON;
	p_g_ram->input_arrow_icon.state = SEG_OFF;
	p_g_ram->input_arrow_icon.value = SEG_OFF;
	p_g_ram->degree_icon.state = SEG_ON;
	p_g_ram->degree_icon.value = SEG_ON;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	return ret_val;
}

static uint32_t menu_set_default_on_t_in_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"%3d%c", 
			br4s_menu_state.p_br4s_state->dev_info.sensors_state.in_temp, SEG_CELSIUM);
	p_g_ram->output_arrow_icon.state = SEG_OFF;
	p_g_ram->output_arrow_icon.value = SEG_OFF;
	p_g_ram->input_arrow_icon.state = SEG_ON;
	p_g_ram->input_arrow_icon.value = SEG_ON;
	p_g_ram->degree_icon.state = SEG_ON;
	p_g_ram->degree_icon.value = SEG_ON;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	return ret_val;
}

static uint32_t menu_set_default_on_time_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"%02d-%02d", 
			br4s_menu_state.p_br4s_state->dev_time.hour,
			br4s_menu_state.p_br4s_state->dev_time.min);
	p_g_ram->output_arrow_icon.state = SEG_OFF;
	p_g_ram->output_arrow_icon.value = SEG_OFF;
	p_g_ram->input_arrow_icon.state = SEG_OFF;
	p_g_ram->input_arrow_icon.value = SEG_OFF;
	p_g_ram->degree_icon.state = SEG_OFF;
	p_g_ram->degree_icon.value = SEG_OFF;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	
	p_g_ram->breezer_icon.state = SEG_OFF;
	p_g_ram->breezer_icon.value = SEG_OFF;
	return ret_val;
}
static uint32_t menu_set_default_on_t_set_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];
	uint8_t b_local_sub_state_mode;
	/*if (br4s_menu_state.sub_state_mode == 
		BR4S_ON_DEFAULT_INVALID_SPEED_M_ID)
	{
		b_local_sub_state_mode = 
			*br4s_menu_state.p_prev_menu_sub_state_mode;
	}
	else
	{*/
		b_local_sub_state_mode = br4s_menu_state.sub_state_mode;
	//}
	snprintf(b_big_str, 6,"%3d%c", 
			br4s_menu_state.p_br4s_state->dev_info.main_params.target_temp, SEG_CELSIUM);
	p_g_ram->output_arrow_icon.state = SEG_ON;
	p_g_ram->output_arrow_icon.value = SEG_ON;
	p_g_ram->input_arrow_icon.state = SEG_OFF;
	p_g_ram->input_arrow_icon.value = SEG_OFF;
	p_g_ram->degree_icon.state = SEG_BLINK;
	p_g_ram->degree_icon.value = SEG_ON;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		if (b_local_sub_state_mode == 
			BR4S_ON_DEFAULT_CONT_TSET_M_ID)
		{
			p_g_ram->big_line[cnt].state = SEG_ON;
		}
		else
		{
			p_g_ram->big_line[cnt].state = SEG_BLINK;
		}
	}
	if (b_local_sub_state_mode == BR4S_ON_DEFAULT_CONT_TSET_M_ID)
	{
		p_g_ram->degree_icon.state = SEG_ON;
	}
	return ret_val;
}
static uint32_t menu_set_default_on_h_off_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t b_big_str[BIG_LINE_LEN+1];

	memcpy(b_big_str, BR4S_HEATER_OFF_STR, BIG_LINE_LEN);
	p_g_ram->output_arrow_icon.state = SEG_OFF;
	p_g_ram->output_arrow_icon.value = SEG_OFF;
	p_g_ram->input_arrow_icon.state = SEG_OFF;
	p_g_ram->input_arrow_icon.value = SEG_OFF;
	p_g_ram->degree_icon.state = SEG_OFF;
	p_g_ram->degree_icon.value = SEG_OFF;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_BLINK;
	}
	return ret_val;
}
static uint32_t menu_set_default_on_dev_rep_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t b_big_str[BIG_LINE_LEN+1];

	memcpy(b_big_str, BR4S_DEV_REP_STR, BIG_LINE_LEN);
	p_g_ram->output_arrow_icon.state = SEG_OFF;
	p_g_ram->output_arrow_icon.value = SEG_OFF;
	p_g_ram->input_arrow_icon.state = SEG_OFF;
	p_g_ram->input_arrow_icon.value = SEG_OFF;
	p_g_ram->degree_icon.state = SEG_OFF;
	p_g_ram->degree_icon.value = SEG_OFF;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	p_g_ram->breezer_icon.state = SEG_OFF;
	p_g_ram->breezer_icon.value = SEG_OFF;
	return ret_val;
}
static uint32_t menu_set_default_on_dev_fw_rep_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"F%04X", br4s_menu_state.p_br4s_state->sys_info.firmware_version);
	p_g_ram->output_arrow_icon.state = SEG_OFF;
	p_g_ram->output_arrow_icon.value = SEG_OFF;
	p_g_ram->input_arrow_icon.state = SEG_OFF;
	p_g_ram->input_arrow_icon.value = SEG_OFF;
	p_g_ram->degree_icon.state = SEG_OFF;
	p_g_ram->degree_icon.value = SEG_OFF;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	p_g_ram->breezer_icon.state = SEG_OFF;
	p_g_ram->breezer_icon.value = SEG_OFF;
	return ret_val;
}

static uint32_t menu_set_default_on_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_small_str[SMALL_LINE_LEN+1];


	p_g_ram->blink.state = SEG_ON;
	p_g_ram->blink.pattern = DEFAULT_BLINK_PATTERN;
/*Fan Speed section*/
	if(br4s_menu_state.p_br4s_state->dev_info.main_params.fan_speed != 0)
	{
		for(uint8_t s_cnt = 0; s_cnt < br4s_menu_state.p_br4s_state->dev_info.main_params.fan_speed; s_cnt++) 
		{
			p_g_ram->speed_line[s_cnt].state = SEG_ON;
			p_g_ram->speed_line[s_cnt].value = SEG_ON;
		}
	}

	uint8_t b_local_sub_state_mode;
	if (br4s_menu_state.sub_state_mode == 
		BR4S_ON_DEFAULT_INVALID_SPEED_M_ID)
	{
		b_local_sub_state_mode = 
			*br4s_menu_state.p_prev_menu_sub_state_mode;

		if(br4s_menu_state.p_br4s_state->dev_info.main_params.fan_speed != 0)
		{
			for(uint8_t s_cnt = 0; s_cnt < br4s_menu_state.p_br4s_state->dev_info.main_params.fan_speed; s_cnt++) 
			{
				p_g_ram->speed_line[s_cnt].state = SEG_BLINK;
				p_g_ram->speed_line[s_cnt].value = SEG_ON;
			}
		}
	}
	else
	{
		b_local_sub_state_mode = br4s_menu_state.sub_state_mode;
	}
/*MISC section*/

	p_g_ram->breezer_icon.state = SEG_ON;
	p_g_ram->breezer_icon.value = SEG_ON;

	p_g_ram->fan_icon.state = SEG_ON;
	p_g_ram->fan_icon.value = SEG_ON;
/*Heater section*/
	if((br4s_menu_state.p_br4s_state->dev_info.flags.heater_state &&
		br4s_menu_state.p_br4s_state->dev_info.heater_var > HEATER_ICON_TRESHOLD))
	{
		p_g_ram->heater_pic[0].value = SEG_ON;
		p_g_ram->heater_pic[0].state = SEG_ON;
		p_g_ram->heater_pic[1].value = SEG_OFF;
		p_g_ram->heater_pic[1].state = SEG_OFF;
	}
	else if((br4s_menu_state.p_br4s_state->dev_info.flags.heater_state &&
		br4s_menu_state.p_br4s_state->dev_info.heater_var < HEATER_ICON_TRESHOLD))
	{
            if (!br4s_menu_state.p_br4s_state->dev_info.flags.heater_mode)//if heating heater mode
            {
		p_g_ram->heater_pic[0].value = SEG_OFF;
		p_g_ram->heater_pic[0].state = SEG_OFF;
		p_g_ram->heater_pic[1].value = SEG_OFF;
		p_g_ram->heater_pic[1].state = SEG_OFF;
            } 
            else //if maintenance
            {
                p_g_ram->heater_pic[0].value = SEG_ON;
		p_g_ram->heater_pic[0].state = SEG_ON;
		p_g_ram->heater_pic[1].value = SEG_ON;
		p_g_ram->heater_pic[1].state = SEG_ON;
            }
         }
        else
        {
                p_g_ram->heater_pic[0].value = SEG_ON;
		p_g_ram->heater_pic[0].state = SEG_ON;
		p_g_ram->heater_pic[1].value = SEG_ON;
		p_g_ram->heater_pic[1].state = SEG_ON;
        }
/*MA section*/
	p_g_ram->ma_icon.value = SEG_OFF;
	p_g_ram->ma_icon.state	= SEG_OFF;
	p_g_ram->auto_icon.value = SEG_OFF;
	p_g_ram->auto_icon.state = SEG_OFF;

/*Small line section*/
	uint32_t b_filter_time_in_days;
	CONVERT_SEC_TO_DAYS(b_filter_time_in_days, 
						br4s_menu_state.p_br4s_state->dev_info.counters.filter_time);
	snprintf(b_small_str, 4,"%03d", b_filter_time_in_days);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}
	if (b_filter_time_in_days < 30)
	{
		for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
		{
			p_g_ram->small_line[cnt].state = SEG_BLINK;
		}
		p_g_ram->filter_icon.value = SEG_ON;
		p_g_ram->filter_icon.state = SEG_BLINK;
	}
	
/*Dumper section*/	
	switch(br4s_menu_state.p_br4s_state->dev_info.main_params.substate)
	{
		case BR4S_RECYCLE_SUBSTATE:
		{
			p_g_ram->gate_pic[0].value = SEG_ON;
			p_g_ram->gate_pic[0].state	= SEG_ON;
			p_g_ram->gate_pic[1].value = SEG_OFF;
			p_g_ram->gate_pic[1].state	= SEG_OFF;
		}	break;

		case BR4S_INFLOW_SUBSTATE:
		{
			p_g_ram->gate_pic[0].value = SEG_OFF;
			p_g_ram->gate_pic[0].state	= SEG_OFF;
			p_g_ram->gate_pic[1].value = SEG_ON;
			p_g_ram->gate_pic[1].state	= SEG_ON;
			
		}	break;

		default:
		{
			p_g_ram->gate_pic[0].value = SEG_OFF;
			p_g_ram->gate_pic[0].state	= SEG_OFF;
			p_g_ram->gate_pic[1].value = SEG_OFF;
			p_g_ram->gate_pic[1].state	= SEG_OFF;
		}	break;
	} 	
	/*Big Line section*/


	if(br4s_menu_state.p_br4s_state->timers[BR4S_ON_TIMER_REM_IND].timer_flags.timer_state &&
		br4s_menu_state.p_br4s_state->timers[BR4S_OFF_TIMER_REM_IND].timer_flags.timer_state)
		{
			p_g_ram->alarm_clock_icon.value = SEG_ON;
			p_g_ram->alarm_clock_icon.state	= SEG_ON;
		}
		else
		{
			p_g_ram->alarm_clock_icon.value = SEG_OFF;
			p_g_ram->alarm_clock_icon.state	= SEG_OFF;
		}

	switch (b_local_sub_state_mode)
	{
		case BR4S_ON_DEFAULT_TOUT_M_ID:
		{
			menu_set_default_on_t_out_state(p_g_ram);
		}	break;
		case BR4S_ON_DEFAULT_TIN_M_ID:
		{
			menu_set_default_on_t_in_state(p_g_ram);
		}	break;
		case BR4S_ON_DEFAULT_TIME_M_ID:
		{
			menu_set_default_on_time_state(p_g_ram);
		}	break;
		case BR4S_ON_DEFAULT_CONT_TSET_M_ID:
		case BR4S_ON_DEFAULT_TSET_M_ID:
		{
			menu_set_default_on_t_set_state(p_g_ram);
		}	break;
		case BR4S_ON_DEFAULT_HOFF_M_ID:
		{
			menu_set_default_on_h_off_state(p_g_ram);
		}	break;
		case BR4S_ON_DEFAULT_DEV_REPORT_M_ID:
		{
			menu_set_default_on_dev_rep_state(p_g_ram);
		}	break;
		case BR4S_ON_DEFAULT_DEV_FW_REPORT_M_ID:
		{
			menu_set_default_on_dev_fw_rep_state(p_g_ram);
		}	break;
		default:
			break;
	}
	return ret_val;
}

static uint32_t menu_clear(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	//brc_lcd_g_ram_t b_local_g_ram = {0};
	//menu_dummy_screen(p_arg, length);
	menu_report_dev(p_arg, length);
	/*while(NRF_SUCCESS != br4s_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));*/
	return ret_val;
}

static uint32_t menu_init_erase(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(BR4S_ERASE_BONDS_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != br4s_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_report_dev(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(BR4S_DEV_REP_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != br4s_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_dummy_screen(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(BR4S_STBY_STR);
	b_local_g_ram.blink.state = SEG_ON;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_BLINK;
	}

	while(NRF_SUCCESS != br4s_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;	
}

static uint32_t menu_on_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	menu_set_default_on_state(&b_local_g_ram);

	ret_val = br4s_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));

	return ret_val;
}

static uint32_t menu_on_set_time_blink_format(brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	switch (br4s_menu_state.sub_state_mode)
	{
		case BR4S_ON_SET_CONT_TIME_HOUR_M_ID:
		case BR4S_ON_SET_CONT_ALARM_ON_HOUR_M_ID:
		case BR4S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID:
		{
			p_g_ram->big_line[0].state = SEG_ON;
			p_g_ram->big_line[1].state = SEG_ON;
		}	break;
		case BR4S_ON_SET_CONT_TIME_MIN_M_ID:
		case BR4S_ON_SET_CONT_ALARM_ON_MIN_M_ID:
		case BR4S_ON_SET_CONT_ALARM_OFF_MIN_M_ID:
		{
			p_g_ram->big_line[3].state = SEG_ON;
			p_g_ram->big_line[4].state = SEG_ON;
		}	break;
		case BR4S_ON_SET_TIME_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
		{
			p_g_ram->big_line[0].state = SEG_BLINK;
			p_g_ram->big_line[1].state = SEG_BLINK;
		}	break;
		case BR4S_ON_SET_TIME_MIN_M_ID:
		case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
		case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
		{
			p_g_ram->big_line[3].state = SEG_BLINK;
			p_g_ram->big_line[4].state = SEG_BLINK;
		}	break;
		default:
			break;
	}
	return ret_val;
}

static uint32_t menu_on_set_heater_on_t_set (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"%3d%c", 
			br4s_menu_state.p_br4s_state->dev_info.main_params.target_temp, SEG_CELSIUM);
	p_g_ram->degree_icon.state = SEG_BLINK;
	p_g_ram->degree_icon.value = SEG_ON;
	p_g_ram->heater_pic[0].state = SEG_ON;
	p_g_ram->heater_pic[0].value = SEG_ON;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_BLINK;
	}
	/*uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BR4S_ON_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}*/
	return ret_val;
}

static uint32_t menu_on_set_heater_off (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t b_big_str[BIG_LINE_LEN+1];

	memcpy(b_big_str, BR4S_HEATER_OFF_STR, BIG_LINE_LEN);
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_BLINK;
	}
	p_g_ram->heater_pic[0].value = SEG_ON;
	p_g_ram->heater_pic[0].state = SEG_ON;
	p_g_ram->heater_pic[1].value = SEG_ON;
	p_g_ram->heater_pic[1].state = SEG_ON;

	/*uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BR4S_OFF_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}*/
	return ret_val;
}

static uint32_t menu_on_set_time (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"%02d-%02d", 
			br4s_menu_state.p_br4s_state->dev_time.hour,
			br4s_menu_state.p_br4s_state->dev_time.min);
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	
	ret_val = menu_on_set_time_blink_format(p_g_ram);
	
	/*if(!br4s_menu_state.p_br4s_state->dev_info.flags.ma_connect)
	{
		ret_val = menu_on_set_time_blink_format(p_g_ram);
	}*/
	return ret_val;
}

static uint32_t menu_on_set_timer_on_time (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;

	char b_big_str[BIG_LINE_LEN+1];
	snprintf(b_big_str, 6,"%02d-%02d", 
			br4s_menu_state.p_br4s_state->timers[BR4S_ON_TIMER_REM_IND].timer_time.hours,
			br4s_menu_state.p_br4s_state->timers[BR4S_ON_TIMER_REM_IND].timer_time.minutes);
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	ret_val = menu_on_set_time_blink_format(p_g_ram);

	uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BR4S_ON_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}
	p_g_ram->alarm_clock_icon.state = SEG_ON;
	p_g_ram->alarm_clock_icon.value = SEG_ON;
	return ret_val;
}

static uint32_t menu_on_set_timer_off_time (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;

	char b_big_str[BIG_LINE_LEN+1];
	snprintf(b_big_str, 6,"%02d-%02d", 
			br4s_menu_state.p_br4s_state->timers[BR4S_OFF_TIMER_REM_IND].timer_time.hours,
			br4s_menu_state.p_br4s_state->timers[BR4S_OFF_TIMER_REM_IND].timer_time.minutes);
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}
	ret_val = menu_on_set_time_blink_format(p_g_ram);

	uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BR4S_OFF_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}

	p_g_ram->alarm_clock_icon.state = SEG_ON;
	p_g_ram->alarm_clock_icon.value = SEG_ON;
	return ret_val;
}

static uint32_t menu_on_set_timer_on (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;

	p_g_ram->alarm_clock_icon.state = SEG_BLINK;
	p_g_ram->alarm_clock_icon.value = SEG_ON;

	uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BR4S_ON_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_BLINK;
	}

	return ret_val;
}

static uint32_t menu_on_set_timer_off (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;

	p_g_ram->alarm_clock_icon.state = SEG_BLINK;
	p_g_ram->alarm_clock_icon.value = SEG_ON;

	uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BR4S_OFF_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_BLINK;
	}
	
	return ret_val;
}

static uint32_t menu_on_set_filter_time (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;

	p_g_ram->filter_icon.value = SEG_ON;
	p_g_ram->filter_icon.state = SEG_ON;
	char b_small_str[SMALL_LINE_LEN+1];
	uint32_t b_filter_time_in_days;
	CONVERT_SEC_TO_DAYS(b_filter_time_in_days, br4s_menu_state.p_br4s_state->dev_info.counters.filter_time);
	snprintf(b_small_str, 4,"%03d", b_filter_time_in_days);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_BLINK;
	}

	return ret_val;
}

static uint32_t menu_on_set(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};

	ret_val = menu_on_set_clear_screen((void *)&b_local_g_ram, 
									sizeof(brc_lcd_g_ram_t));
	if(ret_val != NRF_SUCCESS)
		return ret_val;

	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	b_local_g_ram.blink.state = SEG_ON;

	switch (br4s_menu_state.sub_state_mode)
	{
		case BR4S_ON_SET_HEATER_HOFF_M_ID:
		{
			ret_val = menu_on_set_heater_off(&b_local_g_ram);
		}	break;
		case BR4S_ON_SET_HEATER_TSET_M_ID:
		{
			ret_val = menu_on_set_heater_on_t_set(&b_local_g_ram);
		}	break;
		case BR4S_ON_SET_CONT_TIME_HOUR_M_ID:
		case BR4S_ON_SET_CONT_TIME_MIN_M_ID:
		case BR4S_ON_SET_TIME_HOUR_M_ID:
		case BR4S_ON_SET_TIME_MIN_M_ID:
		{
			ret_val = menu_on_set_time(&b_local_g_ram);
		}	break;
		case BR4S_ON_SET_ALARM_ON_M_ID:
		{
			ret_val = menu_on_set_timer_on(&b_local_g_ram);
		}	break;
		case BR4S_ON_SET_CONT_ALARM_ON_HOUR_M_ID:
		case BR4S_ON_SET_CONT_ALARM_ON_MIN_M_ID:
		case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
		{
			ret_val = menu_on_set_timer_on_time(&b_local_g_ram);
		}	break;
		case BR4S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID:
		case BR4S_ON_SET_CONT_ALARM_OFF_MIN_M_ID:
		case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
		{
			ret_val = menu_on_set_timer_off_time(&b_local_g_ram);
		}	break;
		case BR4S_ON_SET_ALARM_OFF_M_ID:
		{
			ret_val = menu_on_set_timer_off(&b_local_g_ram);
		}	break;
		case BR4S_ON_SET_FILTER_TIME_M_ID:
		{	
			ret_val = menu_on_set_filter_time(&b_local_g_ram);
		}	break;
		default:
			break;
	}
	
	if(ret_val != NRF_SUCCESS)
		return ret_val;
	ret_val = br4s_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));
	
	return ret_val;
}

static uint32_t menu_on_error(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	brc_lcd_g_ram_t b_local_g_ram = {0};
    uint8_t error_number = 0;
	char b_big_str[BIG_LINE_LEN+1];
	ret_val = menu_on_set_clear_screen((void *)&b_local_g_ram, 
									sizeof(brc_lcd_g_ram_t));
	if(ret_val != NRF_SUCCESS)
		return ret_val;
	
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	b_local_g_ram.blink.state = SEG_ON;

	for (uint8_t er_cnt = 0; er_cnt < 11; er_cnt++)
	{
		if ((br4s_menu_state.p_br4s_state->dev_info.errors >> er_cnt) &
			0x01)
		{
			error_number = er_cnt+1;
			snprintf(b_big_str, 6,"EC %02d", error_number);
			break;
		}
	}
	if (error_number == 0)
	{
		uint32_t b_err_reg;
		b_err_reg = br4s_menu_state.p_br4s_state->dev_info.errors >> 23;
		
		for (uint8_t wr_cnt = 0; wr_cnt < 6; wr_cnt++)
		{
			if ((b_err_reg >> wr_cnt) & 
				0x01)
			{
				error_number = wr_cnt++;
				snprintf(b_big_str, 6,"CC %02d", error_number);
				break;
			}
		}
	}

	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = b_big_str[cnt];
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	ret_val = br4s_menu_state.p_brc_main->p_lcd->write(
							(void*)&b_local_g_ram, 
							sizeof(brc_lcd_g_ram_t));
	return ret_val;
}

static uint32_t menu_off_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(BR4S_STBY_STR);
	b_local_g_ram.blink.state = SEG_ON;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_BLINK;
	}

	ret_val = br4s_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));

	return ret_val;
}
/*class methods section*/
static uint32_t init_device_instance(void * p_context, 
						void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	br4s_main_t * p_local_br4s_main;
	
	p_local_br4s_main = (br4s_main_t *)p_context;
	br4s_menu_state.p_brc_main = p_local_br4s_main->p_brc_main;
	br4s_menu_state.p_br4s_state = 
		&p_local_br4s_main->br4s_state;
	br4s_menu_state.p_prev_menu_sub_state_mode = 
		&p_local_br4s_main->prev_menu_sub_state_mode;
	br4s_menu_state.p_br4s_callback = p_local_br4s_main->p_br4s_callback;
	menu_clear(NULL, NULL);
	br4s_menu_state.state = BR4S_MENU_INIT_ID;
	br4s_menu_state.sub_state = BR4S_INIT_CLEAR_M_ID;
	
	return ret_val;
}

static uint32_t deinit_device_instance(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;

	menu_clear(NULL, NULL);
	br4s_menu_state.p_brc_main = NULL;
	br4s_menu_state.p_br4s_state = NULL;
	br4s_menu_state.p_prev_menu_sub_state_mode = NULL;
	br4s_menu_state.p_br4s_callback = NULL;
	br4s_menu_state.state = BR4S_MENU_INIT_ID;
	br4s_menu_state.sub_state = BR4S_INIT_CLEAR_M_ID;
	br4s_menu_state.sub_state_mode = BR4S_INIT_CLEAR_M_ID;

	return ret_val;
}

static uint32_t execute(void * p_context, 
						void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint32_t b_state_idx, b_sub_state_idx;

	if (!br4s_menu_state.p_br4s_state->br4s_first_responce_dev_received)
	{
		br4s_menu_state.state = BR4S_MENU_INIT_ID;
		br4s_menu_state.sub_state = BR4S_INIT_CLEAR_M_ID;
		br4s_menu_state.sub_state_mode = BR4S_INIT_CLEAR_M_ID;
	}
	else
	{
		if ((br4s_menu_state.state == BR4S_MENU_INIT_ID) &&
			(br4s_menu_state.p_br4s_state->br4s_first_responce_dev_received))
		{
			if (br4s_menu_state.p_br4s_callback != NULL)
			{
				br4s_menu_state.p_br4s_callback((void *)&br4s_menu_state, NULL, 
									NULL, USER_TIMEOUT_RELOAD_EVT);
			}
		}
		if ((br4s_menu_state.p_br4s_state->dev_info.flags.power_state)&& 
			(br4s_menu_state.state != BR4S_MENU_ON_ID))
		{
			br4s_menu_state.state = BR4S_MENU_ON_ID;
			/*if ((br4s_menu_state.sub_state >= BR4S_ON_M_NUM) ||
				(br4s_menu_state.sub_state == BR4S_ON_CLEAR_M_ID))
				
			{*/
				br4s_menu_state.sub_state = BR4S_ON_DEFAULT_M_ID;
				br4s_menu_state.sub_state_mode = BR4S_ON_DEFAULT_TOUT_M_ID;
			//}
		}
		else if((!br4s_menu_state.p_br4s_state->dev_info.flags.power_state)&& 
			(br4s_menu_state.state != BR4S_MENU_OFF_ID))
		{
			br4s_menu_state.state = BR4S_MENU_OFF_ID;
			if(br4s_menu_state.sub_state >= BR4S_OFF_M_NUM)
				br4s_menu_state.sub_state = BR4S_OFF_DEFAULT_M_ID;
		}

		if((br4s_menu_state.sub_state == BR4S_ON_CLEAR_M_ID) &&
		   (br4s_menu_state.state == BR4S_MENU_ON_ID))
		{
			br4s_menu_state.sub_state = BR4S_ON_DEFAULT_M_ID;
		}
		
		if((br4s_menu_state.sub_state == BR4S_OFF_CLEAR_M_ID) &&
		   (br4s_menu_state.state == BR4S_MENU_OFF_ID))
		{
			br4s_menu_state.sub_state = BR4S_OFF_DEFAULT_M_ID;
		}
	}
	
	b_state_idx = br4s_menu_state.state;
	b_sub_state_idx = br4s_menu_state.sub_state;
	if ((b_state_idx < BR4S_MENU_SIZE) &&
		(b_sub_state_idx < br4s_menu_state.menu[b_state_idx].size))
	{
		br4s_menu_state.menu[b_state_idx].p_menu_sub_m_list[b_sub_state_idx](NULL,NULL);
	}
	else
	{
		ret_val = NRF_ERROR_INVALID_STATE;
		menu_clear(NULL, NULL);
		br4s_menu_state.state = BR4S_MENU_INIT_ID;
		br4s_menu_state.sub_state = BR4S_INIT_CLEAR_M_ID;
	}
	return ret_val;
}
/*Public functions*/
br4s_menu_state_t * br4s_menu_get_mode(void)
{
	return &br4s_menu_state;
}
/******************************* END_OF_FILE **********************************/