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
// clang-format on
/*Private types*/

/*Private functions prototypes*/
static uint32_t menu_clear(void * p_arg, uint32_t length);
static uint32_t menu_init_erase(void * p_arg, uint32_t length);
static uint32_t menu_report_dev(void * p_arg, uint32_t length);

static uint32_t menu_on_default(void * p_arg, uint32_t length);
static uint32_t menu_on_set(void * p_arg, uint32_t length);
static uint32_t menu_on_error(void * p_arg, uint32_t length);
static uint32_t menu_on_kiv(void * p_arg, uint32_t length);
static uint32_t menu_on_kiv_set(void * p_arg, uint32_t length);

static uint32_t menu_off_default(void * p_arg, uint32_t length);
/*static uint32_t menu_off_error(void * p_arg, uint32_t length);*/

static uint32_t menu_set_default_on_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_kiv_on_state (brc_lcd_g_ram_t * p_g_ram);

static uint32_t init_device_instance(void * p_context, 
						void * p_arg, uint32_t length);
static uint32_t deinit_device_instance(void * p_context);

static uint32_t execute(void * p_context, 
						void * p_arg, uint32_t length);

/*static uint32_t menu_set_default_on_h_off_state (brc_lcd_g_ram_t * p_g_ram);*/
static uint32_t menu_set_default_on_t_set_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_h_inactive_state (brc_lcd_g_ram_t * p_g_ram);
/*static uint32_t menu_set_default_on_t_out_state (brc_lcd_g_ram_t * p_g_ram);*/
static uint32_t menu_set_default_on_dev_rep_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_default_on_dev_fw_rep_state (brc_lcd_g_ram_t * p_g_ram);

static uint32_t menu_set_kiv_on_dev_fw_rep_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_set_kiv_on_dev_rep_state (brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_kiv_start_screen (brc_lcd_g_ram_t * p_g_ram);

static uint32_t menu_on_set_autokiv_switch(brc_lcd_g_ram_t * p_g_ram);
static uint32_t menu_on_set_autokiv_temp_set(brc_lcd_g_ram_t * p_g_ram);

/*Exported constants*/
const menu_m_list_t brlt_menu_vptr = {
	.init = &init_device_instance,
	.deinit = &deinit_device_instance,
	.execute = &execute,
};
/*Private variables*/
static set_indication_t		brlt_menu_init_m[BRLT_INIT_M_NUM] = 
{
	menu_clear,
	menu_init_erase,
	menu_report_dev,
};

static set_indication_t		brlt_menu_on_m[BRLT_ON_M_NUM] = 
{
	menu_clear,
	menu_on_default,
	menu_on_set,
	menu_on_error,
	menu_on_kiv,
	menu_on_kiv_set,
};

static set_indication_t		brlt_menu_off_m[BRLT_OFF_M_NUM] = 
{
	menu_clear,
	menu_off_default,
	menu_on_error,
};

static brlt_menu_state_t brlt_menu_state = 
{
	.menu[BRLT_MENU_INIT_ID].size = BRLT_INIT_M_NUM,
	.menu[BRLT_MENU_INIT_ID].p_menu_sub_m_list = brlt_menu_init_m,
	.menu[BRLT_MENU_ON_ID].size = BRLT_ON_M_NUM,
	.menu[BRLT_MENU_ON_ID].p_menu_sub_m_list = brlt_menu_on_m,
	.menu[BRLT_MENU_OFF_ID].size = BRLT_OFF_M_NUM,
	.menu[BRLT_MENU_OFF_ID].p_menu_sub_m_list = brlt_menu_off_m,
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

/*static uint32_t menu_set_default_on_h_off_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t b_big_str[BIG_LINE_LEN+1];

	memcpy(b_big_str, BRLT_HEATER_OFF_STR, BIG_LINE_LEN);
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
}*/

static uint32_t menu_set_default_on_t_set_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	if (!brlt_menu_state.p_brlt_state->dev_info.flags.heater_mode)
	{
		ret_val = menu_set_default_on_h_inactive_state(p_g_ram);
		brlt_menu_state.sub_state_mode = BRLT_ON_DEFAULT_H_INACTIVE_M_ID;
	}
	else
	{
		snprintf(b_big_str, 6,"%3d%c", 
				brlt_menu_state.p_brlt_state->dev_info.user_set.target_temp, SEG_CELSIUM);
		p_g_ram->output_arrow_icon.state = SEG_ON;
		p_g_ram->output_arrow_icon.value = SEG_ON;
		p_g_ram->input_arrow_icon.state = SEG_OFF;
		p_g_ram->input_arrow_icon.value = SEG_OFF;
		p_g_ram->degree_icon.state = SEG_ON;
		p_g_ram->degree_icon.value = SEG_ON;
		p_g_ram->heater_pic[0].state = SEG_ON;
		p_g_ram->heater_pic[0].value = SEG_ON;
		p_g_ram->heater_pic[1].state = SEG_OFF;
		p_g_ram->heater_pic[1].value = SEG_OFF;
		for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
		{
			p_g_ram->big_line[cnt].value = b_big_str[cnt];
			p_g_ram->big_line[cnt].state = SEG_ON;
		}
	}
	return ret_val;
}

static uint32_t menu_set_default_on_h_inactive_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];
	
	if (brlt_menu_state.p_brlt_state->dev_info.flags.heater_mode)
	{
		ret_val = menu_set_default_on_t_set_state(p_g_ram);	
		brlt_menu_state.sub_state_mode = BRLT_ON_DEFAULT_TSET_M_ID;
	}
	else
	{
		snprintf(b_big_str, 6," 00  %c", SEG_CELSIUM);
		p_g_ram->output_arrow_icon.state = SEG_ON;
		p_g_ram->output_arrow_icon.value = SEG_ON;
		p_g_ram->input_arrow_icon.state = SEG_OFF;
		p_g_ram->input_arrow_icon.value = SEG_OFF;
		p_g_ram->degree_icon.state = SEG_OFF;
		p_g_ram->degree_icon.value = SEG_OFF;
		p_g_ram->heater_pic[0].state = SEG_BLINK;
		p_g_ram->heater_pic[0].value = SEG_ON;
		p_g_ram->heater_pic[1].state = SEG_OFF;
		p_g_ram->heater_pic[1].value = SEG_OFF;
		for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
		{
			p_g_ram->big_line[cnt].value = b_big_str[cnt];
			p_g_ram->big_line[cnt].state = SEG_ON;
		}
	}
	return ret_val;
}

/*static uint32_t menu_set_default_on_t_out_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"%3d%c", 
			brlt_menu_state.p_brlt_state->dev_info.sensors.temp_output, SEG_CELSIUM);
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
}*/

static uint32_t menu_set_kiv_on_dev_rep_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t b_big_str[BIG_LINE_LEN+1];

	memcpy(b_big_str, BRLT_DEV_REP_STR, BIG_LINE_LEN);
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
	return ret_val;
}

static uint32_t menu_set_kiv_on_dev_fw_rep_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"F%04X", 
			brlt_menu_state.p_brlt_state->sys_info.firmware_version);
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
	return ret_val;
}

static uint32_t menu_on_kiv_start_screen (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"%3d%c", 
				brlt_menu_state.p_brlt_state->dev_info.sensors.temp_output, SEG_CELSIUM);
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

	p_g_ram->breezer_icon.state = SEG_ON;
	p_g_ram->breezer_icon.value = SEG_ON;

	return ret_val;
}

static uint32_t menu_set_default_on_dev_rep_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t b_big_str[BIG_LINE_LEN+1];

	memcpy(b_big_str, BRLT_DEV_REP_STR, BIG_LINE_LEN);
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
	return ret_val;
}

static uint32_t menu_set_default_on_dev_fw_rep_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	snprintf(b_big_str, 6,"F%04X", 
			brlt_menu_state.p_brlt_state->sys_info.firmware_version);
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
	return ret_val;
}

static uint32_t menu_on_default_start_screen (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];

	if (brlt_menu_state.p_brlt_state->dev_info.flags.heater_present)
	{
		if (brlt_menu_state.p_brlt_state->dev_info.flags.heater_mode)
		{
			snprintf(b_big_str, 6,"%3d%c", 
				brlt_menu_state.p_brlt_state->dev_info.user_set.target_temp, 
				SEG_CELSIUM);
			p_g_ram->degree_icon.state = SEG_ON;
			p_g_ram->degree_icon.value = SEG_ON;
		}
		else
		{
			snprintf(b_big_str, 6," 00  ");
			p_g_ram->degree_icon.state = SEG_OFF;
			p_g_ram->degree_icon.value = SEG_OFF;
		}
	}
	else
	{
		snprintf(b_big_str, 6, " -- "/*"%3d%c"  
			brlt_menu_state.p_brlt_state->dev_info.sensors.temp_output, 
			SEG_CELSIUM*/);
		p_g_ram->degree_icon.state = SEG_OFF;
		p_g_ram->degree_icon.value = SEG_OFF;
	}

	p_g_ram->output_arrow_icon.state = SEG_ON;
	p_g_ram->output_arrow_icon.value = SEG_ON;

	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}	
	return ret_val;
}

static uint32_t menu_on_default_t_set_screen (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_big_str[BIG_LINE_LEN+1];
	uint8_t b_seg_state = SEG_OFF;

	if ((*brlt_menu_state.p_prev_menu_sub_state_mode == 
		BRLT_ON_DEFAULT_CONT_TSET_M_ID) ||
		(brlt_menu_state.sub_state_mode  == 
		BRLT_ON_DEFAULT_CONT_TSET_M_ID))
	{
		b_seg_state = SEG_ON;
	}
	else
	{
		b_seg_state = SEG_BLINK;
	}

	menu_on_default_start_screen(p_g_ram);
	if (!brlt_menu_state.p_brlt_state->dev_info.flags.heater_present)
	{
		snprintf(b_big_str, 6," --  ");
		for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
		{
			p_g_ram->big_line[cnt].value = b_big_str[cnt];
			p_g_ram->big_line[cnt].state = SEG_BLINK;
		}
		if (p_g_ram->degree_icon.state != SEG_OFF)
		{
			p_g_ram->degree_icon.state = SEG_OFF;
		}
	}
	else
	{
		for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
		{
			p_g_ram->big_line[cnt].state = b_seg_state;
		}
		if (p_g_ram->degree_icon.state != SEG_OFF)
		{
			p_g_ram->degree_icon.state = b_seg_state;
		}
	}

	return ret_val;
}

static uint32_t menu_set_kiv_on_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_small_str[SMALL_LINE_LEN+1];
	uint8_t b_local_sub_state_mode;

	p_g_ram->blink.state = SEG_ON;
	p_g_ram->blink.pattern = DEFAULT_BLINK_PATTERN;

/*Fan Speed section*/
	for(uint8_t s_cnt = 0; 
		s_cnt < brlt_menu_state.p_brlt_state->dev_info.user_set.fan_speed; 
		s_cnt++) 
	{
		p_g_ram->speed_line[s_cnt].state = SEG_OFF;
		p_g_ram->speed_line[s_cnt].value = SEG_OFF;
	}

	if (brlt_menu_state.sub_state_mode == BRLT_ON_KIV_VSET_M_ID)
	{
		p_g_ram->fan_icon.state = SEG_BLINK;
		p_g_ram->fan_icon.value = SEG_ON;
	}
	else
	{
		p_g_ram->fan_icon.state = SEG_OFF;
		p_g_ram->fan_icon.value = SEG_OFF;
	}
/*Big Line section*/




/*substate modes*/
	b_local_sub_state_mode = brlt_menu_state.sub_state_mode;

	switch (b_local_sub_state_mode)
	{
		case BRLT_ON_KIV_TOUT_M_ID:
			menu_on_kiv_start_screen(p_g_ram);
			break;
		case BRLT_ON_KIV_DEV_REPORT_M_ID:
			menu_set_kiv_on_dev_rep_state(p_g_ram);
			break;
		case BRLT_ON_KIV_DEV_FW_REPORT_M_ID:
			menu_set_kiv_on_dev_fw_rep_state(p_g_ram);
			break;		
		case BRLT_ON_KIV_TSET_M_ID:
			menu_on_kiv_start_screen(p_g_ram);
			break;
		case BRLT_ON_KIV_VSET_M_ID:
			menu_on_kiv_start_screen(p_g_ram);
			break;
	}
/*MISC section*/


/*Heater section*/
	if (brlt_menu_state.sub_state_mode == BRLT_ON_KIV_TSET_M_ID)	
	{		
		p_g_ram->heater_pic[0].value = SEG_ON;		
		p_g_ram->heater_pic[0].state = SEG_BLINK;
		p_g_ram->heater_pic[1].value = SEG_ON;		
		p_g_ram->heater_pic[1].state = SEG_BLINK;	
	}else
	{
		p_g_ram->heater_pic[0].value = SEG_OFF;		
		p_g_ram->heater_pic[0].state = SEG_OFF;
		p_g_ram->heater_pic[1].value = SEG_OFF;		
		p_g_ram->heater_pic[1].state = SEG_OFF;			
	}

	
/*MA section*/
	p_g_ram->auto_icon.value = SEG_OFF;
	p_g_ram->auto_icon.state = SEG_OFF;
	p_g_ram->alarm_clock_icon.value = SEG_OFF;
	p_g_ram->alarm_clock_icon.state	= SEG_OFF;
/*Small line section*/
	
	uint32_t b_days;
	
	/*Calculate days*/
	b_days = brlt_menu_state.p_brlt_state->dev_info.counters.filter_time/86400;
	/*Check if current day is not gone yet*/
	if(brlt_menu_state.p_brlt_state->dev_info.counters.filter_time%86400 != 0)
	{
		b_days++;
	}
	snprintf(b_small_str, 4,"%03d",(uint16_t)(b_days));
	
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}
	if ((brlt_menu_state.p_brlt_state->dev_info.counters.filter_time/86400) <= 30)
	{
		for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
		{
			p_g_ram->small_line[cnt].state = SEG_BLINK;
		}
		
		p_g_ram->filter_icon.value = SEG_ON;
		p_g_ram->filter_icon.state = SEG_BLINK;
		if (brlt_menu_state.p_brlt_state->dev_info.flags.filter_wornout)
		{
			
		}
	}
	
/*Dumper section*/	
	if (brlt_menu_state.sub_state_mode == BRLT_ON_KIV_VSET_M_ID)
	{
		p_g_ram->gate_pic[1].value = SEG_ON;
		p_g_ram->gate_pic[1].state	= SEG_BLINK;
		p_g_ram->gate_pic[0].value = SEG_OFF;
		p_g_ram->gate_pic[0].state	= SEG_OFF;
	}else
	{
		p_g_ram->gate_pic[1].value = SEG_ON;
		p_g_ram->gate_pic[1].state	= SEG_ON;
		p_g_ram->gate_pic[0].value = SEG_OFF;
		p_g_ram->gate_pic[0].state	= SEG_OFF;
	}
	
	return ret_val;
}

static uint32_t menu_set_default_on_state (brc_lcd_g_ram_t * p_g_ram)
{
	uint32_t ret_val = NRF_SUCCESS;
	char b_small_str[SMALL_LINE_LEN+1];
	uint8_t b_local_sub_state_mode;

	p_g_ram->blink.state = SEG_ON;
	p_g_ram->blink.pattern = DEFAULT_BLINK_PATTERN;
/*Fan Speed section*/
	if(brlt_menu_state.p_brlt_state->dev_info.user_set.fan_speed > 0)
	{
		for(uint8_t s_cnt = 0; 
			s_cnt < brlt_menu_state.p_brlt_state->dev_info.user_set.fan_speed; 
			s_cnt++) 
		{
			p_g_ram->speed_line[s_cnt].state = SEG_ON;
			p_g_ram->speed_line[s_cnt].value = SEG_ON;
		}
	}
	
	if (brlt_menu_state.sub_state_mode == 
		BRLT_ON_DEFAULT_INVALID_SPEED_M_ID)
	{
		b_local_sub_state_mode = 
			*brlt_menu_state.p_prev_menu_sub_state_mode;

		if(brlt_menu_state.p_brlt_state->dev_info.user_set.fan_speed != 0)
		{
			for(uint8_t s_cnt = 0; 
				s_cnt < brlt_menu_state.p_brlt_state->dev_info.user_set.fan_speed; 
				s_cnt++) 
			{
				p_g_ram->speed_line[s_cnt].state = SEG_BLINK;
				p_g_ram->speed_line[s_cnt].value = SEG_ON;
			}
		}
	}
	else
	{
		b_local_sub_state_mode = brlt_menu_state.sub_state_mode;
	}
/*Big Line section*/

/*substate modes*/
	switch (b_local_sub_state_mode)
	{
		case BRLT_ON_DEFAULT_TSET_M_ID:
		case BRLT_ON_DEFAULT_CONT_TSET_M_ID:
		{
			menu_on_default_t_set_screen(p_g_ram);
			//menu_set_default_on_t_set_state(p_g_ram);
		}	break;
		case BRLT_ON_DEFAULT_H_INACTIVE_M_ID:
		{
			menu_on_default_start_screen(p_g_ram);
			//menu_set_default_on_h_inactive_state(p_g_ram);
		}	break;
		case BRLT_ON_DEFAULT_HOFF_M_ID:
		{
			menu_on_default_start_screen(p_g_ram);
			//menu_set_default_on_h_off_state(p_g_ram);
		}	break;
		case BRLT_ON_DEFAULT_TOUT_M_ID:
		{
			menu_on_default_start_screen(p_g_ram);
			//menu_set_default_on_t_out_state(p_g_ram);
		}	break;
		case BRLT_ON_DEFAULT_DEV_REPORT_M_ID:
		{
			menu_set_default_on_dev_rep_state(p_g_ram);
		}	break;
		case BRLT_ON_DEFAULT_DEV_FW_REPORT_M_ID:
		{
			menu_set_default_on_dev_fw_rep_state(p_g_ram);
		}	break;
		default:
			break;
	}
/*MISC section*/
	p_g_ram->breezer_icon.state = SEG_ON;
	p_g_ram->breezer_icon.value = SEG_ON;
	p_g_ram->fan_icon.state = SEG_ON;
	p_g_ram->fan_icon.value = SEG_ON;
/*Heater section*/
	if (brlt_menu_state.p_brlt_state->dev_info.flags.heater_present)	
	{		
		p_g_ram->heater_pic[0].value = SEG_ON;		
		if (!brlt_menu_state.p_brlt_state->dev_info.flags.heater_mode)		
		{			
			p_g_ram->heater_pic[0].state = SEG_BLINK;		
		}		
		else		
		{			
			p_g_ram->heater_pic[0].state = SEG_ON;		
		}		
		p_g_ram->heater_pic[1].value = SEG_OFF;		
		p_g_ram->heater_pic[1].state = SEG_OFF;	
	}	
	else	
	{		
		p_g_ram->heater_pic[0].value = SEG_ON;		
		p_g_ram->heater_pic[0].state = SEG_ON;		
		p_g_ram->heater_pic[1].value = SEG_ON;
		p_g_ram->heater_pic[1].state = SEG_ON;	
	}
	
/*MA section*/
	p_g_ram->auto_icon.value = SEG_OFF;
	p_g_ram->auto_icon.state = SEG_OFF;
	p_g_ram->alarm_clock_icon.value = SEG_OFF;
	p_g_ram->alarm_clock_icon.state	= SEG_OFF;
	p_g_ram->auto_icon.state = SEG_OFF;
	p_g_ram->auto_icon.value = SEG_OFF;
/*Small line section*/
	uint32_t b_days;
	
	/*Calculate days*/
	b_days = brlt_menu_state.p_brlt_state->dev_info.counters.filter_time/86400;
	/*Check if current day is not gone yet*/
	if(brlt_menu_state.p_brlt_state->dev_info.counters.filter_time%86400 != 0)
	{
		b_days++;
	}
	
	snprintf(b_small_str, 4,"%03d",(uint16_t)(b_days));
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}
	if ((brlt_menu_state.p_brlt_state->dev_info.counters.filter_time/86400) <= 30)
	{
		for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
		{
			p_g_ram->small_line[cnt].state = SEG_BLINK;
		}
		
		p_g_ram->filter_icon.value = SEG_ON;
		p_g_ram->filter_icon.state = SEG_BLINK;
		if (brlt_menu_state.p_brlt_state->dev_info.flags.filter_wornout)
		{
			
		}
	}
	
/*Dumper section*/	
	p_g_ram->gate_pic[0].value = SEG_OFF;
	p_g_ram->gate_pic[0].state	= SEG_OFF;
	p_g_ram->gate_pic[1].value = SEG_OFF;
	p_g_ram->gate_pic[1].state	= SEG_OFF;
	
	return ret_val;
}

static uint32_t menu_clear(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	while(NRF_SUCCESS != brlt_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_init_erase(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(BRLT_ERASE_BONDS_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != brlt_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_report_dev(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(BRLT_DEV_REP_STR);
	b_local_g_ram.blink.state = SEG_OFF;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_ON;
	}

	while(NRF_SUCCESS != brlt_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t)));
	return ret_val;
}

static uint32_t menu_on_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	menu_set_default_on_state(&b_local_g_ram);

	ret_val = brlt_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));

	return ret_val;
}

static uint32_t menu_on_set_heater_on_t_set (void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t * p_local_g_ram;

	if(length != sizeof(brc_lcd_g_ram_t))
	{
		ret_val = NRF_ERROR_INVALID_LENGTH;
		return ret_val;
	}
	p_local_g_ram = (brc_lcd_g_ram_t *)p_arg;
	
	char b_big_str[BIG_LINE_LEN+1];
	snprintf(b_big_str, 6,"%3d%c", 
		brlt_menu_state.p_brlt_state->dev_info.user_set.target_temp, SEG_CELSIUM);
	p_local_g_ram->degree_icon.state = SEG_BLINK;
	p_local_g_ram->degree_icon.value = SEG_ON;
	p_local_g_ram->heater_pic[0].state = SEG_ON;
	p_local_g_ram->heater_pic[0].value = SEG_ON;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_local_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_local_g_ram->big_line[cnt].state = SEG_BLINK;
	}
	/*uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BRLT_ON_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_local_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_local_g_ram->small_line[cnt].state = SEG_ON;
	}*/
	
	return ret_val;
}

static uint32_t menu_on_set_heater_inactive (void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t * p_local_g_ram;

	if(length != sizeof(brc_lcd_g_ram_t))
	{
		ret_val = NRF_ERROR_INVALID_LENGTH;
		return ret_val;
	}
	p_local_g_ram = (brc_lcd_g_ram_t *)p_arg;

	uint8_t b_big_str[BIG_LINE_LEN+1];
	memcpy(b_big_str, " 00  ", BIG_LINE_LEN);
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_local_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_local_g_ram->big_line[cnt].state = SEG_ON;
	}
	p_local_g_ram->degree_icon.value = SEG_OFF;
	p_local_g_ram->degree_icon.state = SEG_OFF;
	p_local_g_ram->heater_pic[0].value = SEG_ON;
	p_local_g_ram->heater_pic[0].state = SEG_BLINK;
	p_local_g_ram->heater_pic[1].value = SEG_OFF;
	p_local_g_ram->heater_pic[1].state = SEG_OFF;

	/*uint8_t b_small_str[SMALL_LINE_LEN+1];
	memcpy(b_small_str, BRLT_OFF_STR, SMALL_LINE_LEN);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_local_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_local_g_ram->small_line[cnt].state = SEG_ON;
	}*/

	return ret_val;
}

static uint32_t menu_on_set_heater_off (void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t * p_local_g_ram;

	if(length != sizeof(brc_lcd_g_ram_t))
	{
		ret_val = NRF_ERROR_INVALID_LENGTH;
		return ret_val;
	}
	p_local_g_ram = (brc_lcd_g_ram_t *)p_arg;

	uint8_t b_big_str[BIG_LINE_LEN+1];
	memcpy(b_big_str, " --  ", BIG_LINE_LEN);
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_local_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_local_g_ram->big_line[cnt].state = SEG_BLINK;
	}
	p_local_g_ram->degree_icon.value = SEG_OFF;
	p_local_g_ram->degree_icon.state = SEG_OFF;
	p_local_g_ram->heater_pic[0].value = SEG_ON;
	p_local_g_ram->heater_pic[0].state = SEG_ON;
	p_local_g_ram->heater_pic[1].value = SEG_ON;
	p_local_g_ram->heater_pic[1].state = SEG_ON;

	return ret_val;
}

static uint32_t menu_on_set_filter_time (void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	brc_lcd_g_ram_t * p_local_g_ram;
	uint32_t b_days;
		
	if(length != sizeof(brc_lcd_g_ram_t))
	{
		ret_val = NRF_ERROR_INVALID_LENGTH;
		return ret_val;
	}
	
	/*Calculate days*/
	b_days = brlt_menu_state.p_brlt_state->dev_info.counters.filter_time/86400;
	/*Check if current day is not gone yet*/
	if(brlt_menu_state.p_brlt_state->dev_info.counters.filter_time%86400 != 0)
	{
		b_days++;
	}
	
	p_local_g_ram = (brc_lcd_g_ram_t *)p_arg;
	p_local_g_ram->filter_icon.value = SEG_ON;
	p_local_g_ram->filter_icon.state = SEG_ON;
	char b_small_str[SMALL_LINE_LEN+1];
	snprintf(b_small_str, 4,"%03d",b_days);
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_local_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_local_g_ram->small_line[cnt].state = SEG_BLINK;
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

	switch (brlt_menu_state.sub_state_mode)
	{
		case BRLT_ON_SET_HEATER_HOFF_M_ID:
		{
			ret_val = menu_on_set_heater_off((void *)&b_local_g_ram, 
									sizeof(brc_lcd_g_ram_t));
		}	break;
		case BRLT_ON_SET_HEATER_TSET_M_ID:
		{
			ret_val = menu_on_set_heater_on_t_set((void *)&b_local_g_ram, 
									sizeof(brc_lcd_g_ram_t));
		}	break;
		case BRLT_ON_SET_HEATER_H_INACTIVE_M_ID:
		{
			ret_val = menu_on_set_heater_inactive((void *)&b_local_g_ram, 
									sizeof(brc_lcd_g_ram_t));
		}	break;
		case BRLT_ON_SET_FILTER_TIME_M_ID:
		{	
			ret_val = menu_on_set_filter_time((void *)&b_local_g_ram, 
									sizeof(brc_lcd_g_ram_t));
		}	break;
		default:
			break;
	}
	
	if(ret_val != NRF_SUCCESS)
		return ret_val;
	ret_val = brlt_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));

	return ret_val;
}

 static uint32_t menu_on_set_autokiv_switch(brc_lcd_g_ram_t *p_g_ram)
 {
	uint32_t ret_val = NRF_SUCCESS;
	
	p_g_ram->filter_icon.value = SEG_ON;
	p_g_ram->filter_icon.state = SEG_BLINK;
	char b_small_str[SMALL_LINE_LEN+1];
	if (brlt_menu_state.p_brlt_state->dev_info.autokiv_info.autokiv_state)
		snprintf(b_small_str, 4, BRLT_ON_STR );
	else
		snprintf(b_small_str, 4, BRLT_OFF_STR );
	
	for(uint8_t cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		p_g_ram->small_line[cnt].value = b_small_str[cnt];
		p_g_ram->small_line[cnt].state = SEG_ON;
	}

	/*MA section*/
	p_g_ram->auto_icon.value = SEG_ON;
	p_g_ram->auto_icon.state = SEG_ON;
	p_g_ram->alarm_clock_icon.value = SEG_OFF;
	p_g_ram->alarm_clock_icon.state	= SEG_OFF;


	p_g_ram->gate_pic[1].value = SEG_ON;
	p_g_ram->gate_pic[1].state	= SEG_ON;
	p_g_ram->gate_pic[0].value = SEG_OFF;
	p_g_ram->gate_pic[0].state	= SEG_OFF;

	return ret_val;
 }

static uint32_t menu_on_set_autokiv_temp_set(brc_lcd_g_ram_t *p_g_ram)
 {
	 	uint32_t ret_val = NRF_SUCCESS;
	
	char b_big_str[BIG_LINE_LEN+1];
	snprintf(b_big_str, 6,"%3d%c", 
		brlt_menu_state.p_brlt_state->dev_info.autokiv_info.autokiv_temp, SEG_CELSIUM);

	p_g_ram->degree_icon.state = SEG_ON;
	p_g_ram->degree_icon.value = SEG_ON;
	p_g_ram->heater_pic[0].state = SEG_OFF;
	p_g_ram->heater_pic[0].value = SEG_OFF;
	for(uint8_t cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		p_g_ram->big_line[cnt].value = b_big_str[cnt];
		p_g_ram->big_line[cnt].state = SEG_ON;
	}


	p_g_ram->gate_pic[1].value = SEG_ON;
	p_g_ram->gate_pic[1].state	= SEG_ON;
	p_g_ram->gate_pic[0].value = SEG_OFF;
	p_g_ram->gate_pic[0].state	= SEG_OFF;
	return ret_val;
 }

static uint32_t menu_on_kiv_set(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};

	ret_val = menu_on_set_clear_screen((void *)&b_local_g_ram, 
									sizeof(brc_lcd_g_ram_t));
	if(ret_val != NRF_SUCCESS)
		return ret_val;

	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	b_local_g_ram.blink.state = SEG_ON;

/* substate modes */
	switch (brlt_menu_state.sub_state_mode)
	{
		case BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID:
		{
			ret_val = menu_on_set_autokiv_switch(&b_local_g_ram);
		}	break;
		case BRLT_ON_KIV_SET_AUTOKIV_TEMP_M_ID:
		{
			ret_val = menu_on_set_autokiv_temp_set(&b_local_g_ram);
		}	break;
		default:
			break;
	}

	if(ret_val != NRF_SUCCESS)
		return ret_val;
	ret_val = brlt_menu_state.p_brc_main->p_lcd->write(
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
		if ((brlt_menu_state.p_brlt_state->dev_info.errors.flag_register >> er_cnt) &
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
		b_err_reg = brlt_menu_state.p_brlt_state->dev_info.errors.flag_register >> 23;
		
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

	ret_val = brlt_menu_state.p_brc_main->p_lcd->write(
							(void*)&b_local_g_ram, 
							sizeof(brc_lcd_g_ram_t));
	return ret_val;
}

static uint32_t menu_on_kiv(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	menu_set_kiv_on_state(&b_local_g_ram);

	ret_val = brlt_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));

	return ret_val;
}

static uint32_t menu_off_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brc_lcd_g_ram_t b_local_g_ram = {0};
	uint8_t * p_str;
	p_str = (uint8_t *)(BRLT_STBY_STR);
	b_local_g_ram.blink.state = SEG_ON;
	b_local_g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	for(uint8_t cnt = 0; cnt <= BIG_LINE_LEN; cnt++)
	{
		b_local_g_ram.big_line[cnt].value = *(p_str + cnt);
		b_local_g_ram.big_line[cnt].state = SEG_BLINK;
	}
	
	ret_val = brlt_menu_state.p_brc_main->p_lcd->write(
											(void*)&b_local_g_ram, 
											sizeof(brc_lcd_g_ram_t));

	return ret_val;
}
/*class methods section*/
static uint32_t init_device_instance(void * p_context, 
						void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	brlt_main_t * p_local_brlt_main;
	
	p_local_brlt_main = (brlt_main_t *)p_context;
	brlt_menu_state.p_brc_main = p_local_brlt_main->p_brc_main;
	brlt_menu_state.p_brlt_state = 
		&p_local_brlt_main->brlt_state;
	brlt_menu_state.p_prev_menu_sub_state_mode = 
		&p_local_brlt_main->prev_menu_sub_state_mode;
	menu_clear(NULL, NULL);
	brlt_menu_state.state = BRLT_MENU_INIT_ID;
	brlt_menu_state.sub_state = BRLT_INIT_REPORT_DEV_M_ID;

	return ret_val;
}

static uint32_t deinit_device_instance(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;

	menu_clear(NULL, NULL);
	brlt_menu_state.p_brc_main = NULL;
	brlt_menu_state.p_brlt_state = NULL;
	brlt_menu_state.p_prev_menu_sub_state_mode = NULL;
	brlt_menu_state.state = BRLT_MENU_INIT_ID;
	brlt_menu_state.sub_state = BRLT_INIT_CLEAR_M_ID;
	brlt_menu_state.sub_state_mode = BRLT_INIT_CLEAR_M_ID;
	
	return ret_val;
}

static uint32_t execute(void * p_context, 
						void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint32_t b_state_idx, b_sub_state_idx;

	if ((!brlt_menu_state.p_brlt_state->brlt_first_responce_sys_received) ||
		(!brlt_menu_state.p_brlt_state->brlt_first_responce_dev_received))
			return ret_val;
	if (brlt_menu_state.state == BRLT_MENU_INIT_ID) 
	{
		if (brlt_menu_state.p_brlt_callback != NULL)
		{
			brlt_menu_state.p_brlt_callback((void *)&brlt_menu_state, NULL, 
							NULL, USER_TIMEOUT_RELOAD_EVT);
		}
	}
	
	if ((brlt_menu_state.p_brlt_state->dev_info.flags.power_mode)&& 
		(brlt_menu_state.state != BRLT_MENU_ON_ID) &&
		!brlt_menu_state.p_brlt_state->dev_info.flags.kiv_active)
	{
		brlt_menu_state.state = BRLT_MENU_ON_ID;
		brlt_menu_state.sub_state = BRLT_ON_DEFAULT_M_ID;
		brlt_menu_state.sub_state_mode = 
						BRLT_ON_DEFAULT_H_INACTIVE_M_ID;

		
		/*if (brlt_menu_state.p_brlt_state->dev_info.flags.heater_present)
		{
			if (brlt_menu_state.p_brlt_state->dev_info.flags.heater_mode)
			{
				brlt_menu_state.sub_state_mode = 
					BRLT_ON_DEFAULT_TSET_M_ID;
			} 
			else 
				brlt_menu_state.sub_state_mode = 
					BRLT_ON_DEFAULT_H_INACTIVE_M_ID;
		}
		else
			brlt_menu_state.sub_state_mode = 
				BRLT_ON_DEFAULT_DEV_REPORT_M_ID;*/	
	}
	else if((!brlt_menu_state.p_brlt_state->dev_info.flags.power_mode)&& 
		(brlt_menu_state.state != BRLT_MENU_OFF_ID) &&
		!brlt_menu_state.p_brlt_state->dev_info.flags.kiv_active)
	{
			brlt_menu_state.state = BRLT_MENU_OFF_ID;
			brlt_menu_state.sub_state = BRLT_OFF_DEFAULT_M_ID;
			brlt_menu_state.sub_state_mode = 0;
	}

	if (brlt_menu_state.p_brlt_state->dev_info.flags.kiv_active &&
		brlt_menu_state.sub_state != BRLT_ON_KIV_SET_M_ID &&
		brlt_menu_state.sub_state != BRLT_ON_ERROR_M_ID)
	{
		brlt_menu_state.state = BRLT_MENU_ON_ID;
		brlt_menu_state.sub_state = BRLT_ON_KIV_M_ID;
	}

	if((brlt_menu_state.sub_state == BRLT_ON_CLEAR_M_ID) &&
	   (brlt_menu_state.state == BRLT_MENU_ON_ID))
	{
		brlt_menu_state.sub_state = BRLT_ON_DEFAULT_M_ID;
	}

	if((brlt_menu_state.sub_state == BRLT_OFF_CLEAR_M_ID) &&
	   (brlt_menu_state.state == BRLT_MENU_OFF_ID))
	{
		brlt_menu_state.sub_state = BRLT_OFF_DEFAULT_M_ID;
	}

	b_state_idx = brlt_menu_state.state;
	b_sub_state_idx = brlt_menu_state.sub_state;
	if ((b_state_idx < BRLT_MENU_SIZE) &&
		(b_sub_state_idx < brlt_menu_state.menu[b_state_idx].size))
	{
		brlt_menu_state.menu[b_state_idx].p_menu_sub_m_list[b_sub_state_idx](NULL,NULL);
	}
	else
	{
		ret_val = NRF_ERROR_INVALID_STATE;
		menu_clear(NULL, NULL);
		brlt_menu_state.state = BRLT_MENU_INIT_ID;
		brlt_menu_state.sub_state = BRLT_INIT_CLEAR_M_ID;
	}
	return ret_val;
}
/*Public functions*/
brlt_menu_state_t * brlt_menu_get_mode(void)
{
	return &brlt_menu_state;
}
/******************************* END_OF_FILE **********************************/