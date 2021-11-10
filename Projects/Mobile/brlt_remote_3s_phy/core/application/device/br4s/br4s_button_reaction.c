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
#define NEXT_MODE	(1)
#define PREV_MODE	(-1)
// clang-format on
/*Private types*/
typedef struct _tag_br4s_button_reaction_menu_t
{
	button_event_t			power;
	button_event_t			state;
	button_event_t			set;
	button_event_t			gate;
	button_event_t			fan_up;
	button_event_t			fan_down;
	button_event_t			temp_up;
	button_event_t			temp_down;
	
	button_event_t			long_fan_up;
	button_event_t			long_fan_down;
	button_event_t			long_temp_up;
	button_event_t			long_temp_down;
	
	button_event_t			long_state_set_gate;
	button_event_t			long_state_set;
	button_event_t			long_state_set_fan_up_fan_down;
	button_event_t			long_set_temp_up_temp_down;
	button_event_t			long_temp_up_temp_down;
	button_event_t			long_temp_down_power;
	button_event_t			long_state_power;
	button_event_t			long_gate_set_temp_down_fan_up;
} br4s_button_reaction_menu_t;

/*Private functions prototypes*/
static uint32_t init_button_reaction_instance(void * p_context);
static uint32_t deinit_button_reaction_instance(void * p_context);

static uint32_t evt_power_button(void * p_arg, uint32_t length);
static uint32_t evt_state_button(void * p_arg, uint32_t length);
static uint32_t evt_set_button(void * p_arg, uint32_t length);
static uint32_t evt_gate_button(void * p_arg, uint32_t length);
static uint32_t evt_fan_up_button(void * p_arg, uint32_t length);
static uint32_t evt_fan_down_button(void * p_arg, uint32_t length);
static uint32_t evt_temp_up_button(void * p_arg, uint32_t length);
static uint32_t evt_temp_down_button(void * p_arg, uint32_t length);

static uint32_t evt_long_fan_up_button(void * p_arg, uint32_t length);
static uint32_t evt_long_fan_down_button(void * p_arg, uint32_t length);
static uint32_t evt_long_temp_up_button(void * p_arg, uint32_t length);
static uint32_t evt_long_temp_down_button(void * p_arg, uint32_t length);

static uint32_t evt_long_state_set_gate_button(void * p_arg, uint32_t length);
static uint32_t evt_long_state_set_button(void * p_arg, uint32_t length);
static uint32_t evt_long_state_set_fan_up_fan_down_button(void * p_arg, uint32_t length);
static uint32_t evt_long_set_temp_up_temp_down_button(void * p_arg, uint32_t length);
static uint32_t evt_long_temp_up_temp_down_button(void * p_arg, uint32_t length);
static uint32_t evt_long_temp_down_power_button(void * p_arg, uint32_t length);
static uint32_t evt_long_state_power_button(void * p_arg, uint32_t length);
static uint32_t evt_long_gate_set_temp_down_fan_up_button(void * p_arg, uint32_t length);

static uint32_t evt_br_menu_void(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_set_srv_temp(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_bond_erase(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_power(void * p_arg, uint32_t length);

static uint32_t evt_br_menu_single_param_change(void * p_arg, uint32_t length);

static uint32_t evt_br_menu_continues_param_change(void * p_arg, uint32_t length);

static uint32_t evt_br_menu_switch_snd_state(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_start_ma_pairing(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_reset_userset_to_default(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_remote_test(void * p_arg, uint32_t length);

static uint32_t evt_br_menu_on_default_switch_state(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_switch_default_to_set(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_switch_set_to_default(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_default_switch_gate(void * p_arg, uint32_t length);
/*Exported constants*/
const button_reaction_m_list_t	br4s_button_reaction_vptr = {
	.init 				= &init_button_reaction_instance,
	.deinit 			= &deinit_button_reaction_instance,
	
	.power				= &evt_power_button,
	.state				= &evt_state_button,
	.set				= &evt_set_button,
	.gate				= &evt_gate_button,
	.fan_up				= &evt_fan_up_button,
	.fan_down			= &evt_fan_down_button,
	.temp_up			= &evt_temp_up_button,
	.temp_down			= &evt_temp_down_button,
	

	.long_fan_up		= &evt_long_fan_up_button,
	.long_fan_down		= &evt_long_fan_down_button,
	.long_temp_up		= &evt_long_temp_up_button,
	.long_temp_down		= &evt_long_temp_down_button,
	
	.long_state_set_gate			= &evt_long_state_set_gate_button,
	.long_state_set					= &evt_long_state_set_button,
	.long_state_set_fan_up_fan_down	= &evt_long_state_set_fan_up_fan_down_button,
	.long_set_temp_up_temp_down		= &evt_long_set_temp_up_temp_down_button,
	.long_temp_up_temp_down			= &evt_long_temp_up_temp_down_button,
	.long_temp_down_power			= &evt_long_temp_down_power_button,
	.long_state_power				= &evt_long_state_power_button,
	.long_gate_set_temp_down_fan_up	= &evt_long_gate_set_temp_down_fan_up_button,
};

/*Private variables*/
static br4s_button_reaction_t br4s_button_reaction;

static const br4s_button_reaction_menu_t br4s_menu_init_clear_button_reaction = {0};
static const br4s_button_reaction_menu_t br4s_menu_init_erase_bonds_button_reaction = {0};

static const br4s_button_reaction_menu_t br4s_menu_on_clear_button_reaction = {0};
static const br4s_button_reaction_menu_t br4s_menu_on_default_button_reaction =
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_on_default_switch_state,
	.set	= &evt_br_menu_on_switch_default_to_set,
	.gate	= &evt_br_menu_on_default_switch_gate,
	.fan_up	= &evt_br_menu_single_param_change,
	.fan_down	= &evt_br_menu_single_param_change,
	.temp_up	= &evt_br_menu_single_param_change,
	.temp_down	= &evt_br_menu_single_param_change,
	
	.long_fan_up 	= &evt_br_menu_continues_param_change,
	.long_fan_down	= &evt_br_menu_continues_param_change,
	.long_temp_up	= &evt_br_menu_continues_param_change,
	.long_temp_down	= &evt_br_menu_continues_param_change,
		
	.long_state_set_gate 			= &evt_br_menu_switch_snd_state,
	.long_state_set					= &evt_br_menu_start_ma_pairing,
	.long_state_set_fan_up_fan_down	= &evt_br_menu_reset_userset_to_default,
	.long_set_temp_up_temp_down		= &evt_br_menu_set_srv_temp,
	.long_temp_up_temp_down			= &evt_br_menu_void,
	.long_temp_down_power			= &evt_br_menu_void,
	.long_state_power				= &evt_br_menu_bond_erase,
	.long_gate_set_temp_down_fan_up	= &evt_br_menu_remote_test,
};
static const br4s_button_reaction_menu_t br4s_menu_on_set_button_reaction =
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_void,
	.set	= &evt_br_menu_on_switch_set_to_default,
	.gate	= &evt_br_menu_void,
	.fan_up	= &evt_br_menu_single_param_change,
	.fan_down	= &evt_br_menu_single_param_change,
	.temp_up	= &evt_br_menu_single_param_change,
	.temp_down	= &evt_br_menu_single_param_change,
	
	.long_fan_up 	= &evt_br_menu_continues_param_change,
	.long_fan_down	= &evt_br_menu_continues_param_change,
	.long_temp_up	= &evt_br_menu_continues_param_change,
	.long_temp_down	= &evt_br_menu_continues_param_change,
		
	.long_state_set_gate 			= &evt_br_menu_switch_snd_state,
	.long_state_set					= &evt_br_menu_void,
	.long_state_set_fan_up_fan_down	= &evt_br_menu_void,
	.long_set_temp_up_temp_down		= &evt_br_menu_set_srv_temp,
	.long_temp_up_temp_down			= &evt_br_menu_void,
	.long_temp_down_power			= &evt_br_menu_void,
	.long_state_power				= &evt_br_menu_bond_erase,
	.long_gate_set_temp_down_fan_up	= &evt_br_menu_remote_test,
};
static const br4s_button_reaction_menu_t br4s_menu_on_error_button_reaction = 
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_void,
	.set	= &evt_br_menu_void,
	.gate	= &evt_br_menu_void,
	.fan_up	= &evt_br_menu_void,
	.fan_down	= &evt_br_menu_void,
	.temp_up	= &evt_br_menu_void,
	.temp_down	= &evt_br_menu_void,
	
	.long_fan_up 	= &evt_br_menu_void,
	.long_fan_down	= &evt_br_menu_void,
	.long_temp_up	= &evt_br_menu_void,
	.long_temp_down	= &evt_br_menu_void,
		
	.long_state_set_gate 			= &evt_br_menu_void,
	.long_state_set					= &evt_br_menu_void,
	.long_state_set_fan_up_fan_down	= &evt_br_menu_void,
	.long_set_temp_up_temp_down		= &evt_br_menu_void,
	.long_temp_up_temp_down			= &evt_br_menu_void,
	.long_temp_down_power			= &evt_br_menu_void,
	.long_state_power				= &evt_br_menu_bond_erase,
	.long_gate_set_temp_down_fan_up	= &evt_br_menu_remote_test,
};

static const br4s_button_reaction_menu_t br4s_menu_off_clear_button_reaction = {0};
static const br4s_button_reaction_menu_t br4s_menu_off_default_button_reaction = 
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_void,
	.set	= &evt_br_menu_void,
	.gate	= &evt_br_menu_void,
	.fan_up	= &evt_br_menu_void,
	.fan_down	= &evt_br_menu_void,
	.temp_up	= &evt_br_menu_void,
	.temp_down	= &evt_br_menu_void,
	
	.long_fan_up 	= &evt_br_menu_void,
	.long_fan_down	= &evt_br_menu_void,
	.long_temp_up	= &evt_br_menu_void,
	.long_temp_down	= &evt_br_menu_void,
		
	.long_state_set_gate 			= &evt_br_menu_switch_snd_state,
	.long_state_set					= &evt_br_menu_start_ma_pairing,
	.long_state_set_fan_up_fan_down	= &evt_br_menu_reset_userset_to_default,
	.long_set_temp_up_temp_down		= &evt_br_menu_void,
	.long_temp_up_temp_down			= &evt_br_menu_void,
	.long_temp_down_power			= &evt_br_menu_void,
	.long_state_power				= &evt_br_menu_bond_erase,
	.long_gate_set_temp_down_fan_up	= &evt_br_menu_remote_test,
};
static const br4s_button_reaction_menu_t br4s_menu_off_error_button_reaction = 
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_void,
	.set	= &evt_br_menu_void,
	.gate	= &evt_br_menu_void,
	.fan_up	= &evt_br_menu_void,
	.fan_down	= &evt_br_menu_void,
	.temp_up	= &evt_br_menu_void,
	.temp_down	= &evt_br_menu_void,
	
	.long_fan_up 	= &evt_br_menu_void,
	.long_fan_down	= &evt_br_menu_void,
	.long_temp_up	= &evt_br_menu_void,
	.long_temp_down	= &evt_br_menu_void,
		
	.long_state_set_gate 			= &evt_br_menu_void,
	.long_state_set					= &evt_br_menu_void,
	.long_state_set_fan_up_fan_down	= &evt_br_menu_void,
	.long_set_temp_up_temp_down		= &evt_br_menu_void,
	.long_temp_up_temp_down			= &evt_br_menu_void,
	.long_temp_down_power			= &evt_br_menu_void,
	.long_state_power				= &evt_br_menu_bond_erase,
	.long_gate_set_temp_down_fan_up	= &evt_br_menu_remote_test,
};

static const br4s_button_reaction_menu_t * br4s_menu_init_button_reaction[BR4S_INIT_M_NUM] = 
{
	&br4s_menu_init_clear_button_reaction,
	&br4s_menu_init_erase_bonds_button_reaction,
};
static const br4s_button_reaction_menu_t * br4s_menu_on_button_reaction[BR4S_ON_M_NUM] = 
{
	&br4s_menu_on_clear_button_reaction,
	&br4s_menu_on_default_button_reaction,
	&br4s_menu_on_set_button_reaction,
	&br4s_menu_on_error_button_reaction,
};
static const br4s_button_reaction_menu_t * br4s_menu_off_button_reaction[BR4S_OFF_M_NUM] = 
{
	&br4s_menu_off_clear_button_reaction,
	&br4s_menu_off_default_button_reaction,
	&br4s_menu_off_error_button_reaction,
};

static const br4s_button_reaction_menu_t ** br4s_menu_button_reaction[BR4S_MENU_SIZE] = 
{
	br4s_menu_init_button_reaction,
	br4s_menu_on_button_reaction,
	br4s_menu_off_button_reaction,
};

/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/

/*Private functions*/
static uint32_t switch_param_change_indication(void)
{
	uint32_t ret_val = NRF_SUCCESS; 
	uint32_t b_local_state = br4s_button_reaction.p_menu_state->sub_state;
	uint32_t b_local_sub_state = br4s_button_reaction.p_menu_state->sub_state_mode;

	if (b_local_state == BR4S_ON_SET_M_ID)
	{
		if (br4s_button_reaction.cont_param_change_on == 1)
		{
			switch (b_local_sub_state)
			{
				case BR4S_ON_SET_TIME_HOUR_M_ID:
				case BR4S_ON_SET_TIME_MIN_M_ID:
				case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
				case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
				case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
				case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
				{
					br4s_button_reaction.p_menu_state->sub_state_mode += 
						BR4S_ON_SET_SINGLE_CONT_OFFSET;
				}	break;
				default:
					break;
			}
		}
		else
		{
			switch (b_local_sub_state)
			{
				case BR4S_ON_SET_CONT_TIME_HOUR_M_ID:
				case BR4S_ON_SET_CONT_TIME_MIN_M_ID:
				case BR4S_ON_SET_CONT_ALARM_ON_HOUR_M_ID:
				case BR4S_ON_SET_CONT_ALARM_ON_MIN_M_ID:
				case BR4S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID:
				case BR4S_ON_SET_CONT_ALARM_OFF_MIN_M_ID:
				{
					br4s_button_reaction.p_menu_state->sub_state_mode -= 
						BR4S_ON_SET_SINGLE_CONT_OFFSET;
				}	break;
				default:
					break;
			}
		}
	}
	else if (b_local_state == BR4S_ON_DEFAULT_M_ID)
	{
		uint8_t *p_local_sub_state_mode;
		if (br4s_button_reaction.p_menu_state->sub_state_mode == 
			BR4S_ON_DEFAULT_INVALID_SPEED_M_ID)
		{
			p_local_sub_state_mode = 
				br4s_button_reaction.p_menu_state->p_prev_menu_sub_state_mode;
		}
		else
		{
			p_local_sub_state_mode = 
				(uint8_t *)&br4s_button_reaction.p_menu_state->sub_state_mode;
		}
		
		if (br4s_button_reaction.cont_param_change_on == 1)
		{
			switch (b_local_sub_state)
			{
				case BR4S_ON_DEFAULT_TSET_M_ID:
				{
					*p_local_sub_state_mode = BR4S_ON_DEFAULT_CONT_TSET_M_ID;
				}	break;
				default:
					break;
			}
		}
		else
		{
			switch (b_local_sub_state)
			{
				case BR4S_ON_DEFAULT_CONT_TSET_M_ID:
				{
					*p_local_sub_state_mode = BR4S_ON_DEFAULT_TSET_M_ID;
				}	break;
				default:
					break;
			}
		}
	}
	return ret_val;
}

static uint32_t br4s_evt_switch_to_next_menu_set_sub_state_mode(int8_t switch_direction)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	if (switch_direction == NEXT_MODE)
	{	 
		switch (br4s_button_reaction.p_menu_state->sub_state_mode)
		{
			case BR4S_ON_SET_HEATER_HOFF_M_ID:
			case BR4S_ON_SET_HEATER_TSET_M_ID:
			{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_TIME_HOUR_M_ID;
			}	break;
			case BR4S_ON_SET_TIME_MIN_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[0].timer_flags.timer_state &&
					br4s_button_reaction.p_br4s_params->timers[1].timer_flags.timer_state)
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_ALARM_ON_M_ID;
				}
				else
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_ALARM_OFF_M_ID;
				}
			}	break;
			case BR4S_ON_SET_TIME_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_ON_M_ID:
			case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
			case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_OFF_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode++;
			}	break;

			case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_FILTER_TIME_M_ID;
			}	break;
			case BR4S_ON_SET_FILTER_TIME_M_ID:
			{
				if(br4s_button_reaction.p_br4s_params->dev_info.flags.heater_state &&
				   br4s_button_reaction.p_br4s_params->dev_info.heater_var)
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_HEATER_TSET_M_ID;
				}
				else
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_HEATER_HOFF_M_ID;
				}
			}	break;
			case BR4S_ON_SET_CONT_TIME_HOUR_M_ID:
			case BR4S_ON_SET_CONT_TIME_MIN_M_ID:
			case BR4S_ON_SET_CONT_ALARM_ON_HOUR_M_ID:
			case BR4S_ON_SET_CONT_ALARM_ON_MIN_M_ID:
			case BR4S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID:
			case BR4S_ON_SET_CONT_ALARM_OFF_MIN_M_ID:
			{
			
			}	break;
			default:
			{
				if (br4s_button_reaction.p_menu_state->sub_state_mode == 
					(BR4S_ON_SET_M_NUM - 1))
				{
					if(br4s_button_reaction.p_br4s_params->dev_info.flags.heater_state &&
				   		br4s_button_reaction.p_br4s_params->dev_info.heater_var)
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_SET_HEATER_TSET_M_ID;
					}
					else
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_SET_HEATER_HOFF_M_ID;
					}
				}
			}	break;
		}
	}
	else if (switch_direction == PREV_MODE)
	{
		switch (br4s_button_reaction.p_menu_state->sub_state_mode)
		{
			case BR4S_ON_SET_HEATER_HOFF_M_ID:
			case BR4S_ON_SET_HEATER_TSET_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_FILTER_TIME_M_ID;
			}	break;
			case BR4S_ON_SET_TIME_HOUR_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->dev_info.flags.heater_state &&
				   br4s_button_reaction.p_br4s_params->dev_info.heater_var)
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_HEATER_TSET_M_ID;
				}
				else
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_HEATER_HOFF_M_ID;
				}
			}	break;
			case BR4S_ON_SET_ALARM_ON_M_ID:
			case BR4S_ON_SET_ALARM_OFF_M_ID:
			{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_TIME_MIN_M_ID;
			}	break;
			
			case BR4S_ON_SET_TIME_MIN_M_ID:
			case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
			case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode--;
			}	break;

			case BR4S_ON_SET_FILTER_TIME_M_ID:
			{
				if(br4s_button_reaction.p_br4s_params->timers[0].timer_flags.timer_state &&
				br4s_button_reaction.p_br4s_params->timers[1].timer_flags.timer_state)
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_ALARM_ON_M_ID;
				}
				else
				{
					br4s_button_reaction.p_menu_state->sub_state_mode = 
						BR4S_ON_SET_ALARM_OFF_M_ID;
				}
			}	break;
			case BR4S_ON_SET_CONT_TIME_HOUR_M_ID:
			case BR4S_ON_SET_CONT_TIME_MIN_M_ID:
			case BR4S_ON_SET_CONT_ALARM_ON_HOUR_M_ID:
			case BR4S_ON_SET_CONT_ALARM_ON_MIN_M_ID:
			case BR4S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID:
			case BR4S_ON_SET_CONT_ALARM_OFF_MIN_M_ID:
			{
			
			}	break;
			default:
			{
				if (br4s_button_reaction.p_menu_state->sub_state_mode == 
					(BR4S_ON_SET_M_NUM - 1))
				{
					if(br4s_button_reaction.p_br4s_params->dev_info.flags.heater_state &&
				   		br4s_button_reaction.p_br4s_params->dev_info.heater_var)
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_SET_HEATER_TSET_M_ID;
					}
					else
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_SET_HEATER_HOFF_M_ID;
					}
				}
			}	break;
		}
	}
	else
	{
		ret_val = NRF_ERROR_INVALID_PARAM;
	}
	return ret_val;
}

static uint32_t execute_button_evt(uint8_t * p_button_id, uint8_t evt_id)
{
	uint32_t ret_val = NRF_SUCCESS; 
	static uint32_t b_local_state;
	static uint32_t b_local_sub_state;
	
	b_local_state = br4s_button_reaction.p_menu_state->state;
	b_local_sub_state = br4s_button_reaction.p_menu_state->sub_state;
	
	if (br4s_button_reaction.button_in_process != 0)
		return ret_val = NRF_ERROR_BUSY;
	br4s_button_reaction.button_in_process = 1;
			
	switch (evt_id)
	{
		case BUTTON_INT_EVT_POWER_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->power !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->power((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_STATE_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->state !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->state((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_SET_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->set !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->set((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_GATE_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->gate !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->gate((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_FAN_UP_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->fan_up !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->fan_up((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_FAN_DOWN_ID: 
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->fan_down !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->fan_down((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_TEMP_UP_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->temp_up !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->temp_up((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_TEMP_DOWN_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->temp_down !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->temp_down((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_FAN_UP_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_up !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_up((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_FAN_DOWN_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_down !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_down((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_TEMP_UP_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_up !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_up((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_TEMP_DOWN_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_SET_GATE_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_gate !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_gate((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_SET_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_SET_FAN_UP_FAN_DOWN_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_fan_up_fan_down !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_fan_up_fan_down((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_SET_TEMP_UP_TEMP_DOWN_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_set_temp_up_temp_down !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_set_temp_up_temp_down((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_TEMP_DOWN_POWER_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down_power !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down_power((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}		
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_POWER_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_power !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_power((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_GATE_SETTEMP_DOWN_FAN_UP_ID:
		{
			if (br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_gate_set_temp_down_fan_up !=0)
			{
				br4s_menu_button_reaction[b_local_state][b_local_sub_state]->long_gate_set_temp_down_fan_up((void *)p_button_id, NULL);
				br4s_button_reaction.button_in_process = 0;
				return ret_val;
			}
		}	break;
		default:
			break;
	}
	evt_br_menu_void((void *)p_button_id, NULL);
	return ret_val;
}

static uint32_t change_current_menu_set_param(bool state)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint32_t b_sub_state_mode = 
		br4s_button_reaction.p_menu_state->sub_state_mode;
	
	if (state)
	{
		switch (b_sub_state_mode)
		{
			case BR4S_ON_SET_HEATER_HOFF_M_ID:
			{
                            if (br4s_button_reaction.p_br4s_params->dev_info.flags.heater_present)
                            {
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_HEATER_TSET_M_ID;
				br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode = 0;
                            }
                            
			}	break;
			case BR4S_ON_SET_HEATER_TSET_M_ID:
			{
                            if (br4s_button_reaction.p_br4s_params->dev_info.flags.heater_present)
                            {
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_HEATER_HOFF_M_ID;
				br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode = 1;
                            }
			}	break;
			case BR4S_ON_SET_CONT_TIME_HOUR_M_ID:
			case BR4S_ON_SET_TIME_HOUR_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->dev_time.hour < 23)
					br4s_button_reaction.p_br4s_params->dev_time.hour++;
				else
					br4s_button_reaction.p_br4s_params->dev_time.hour = 0;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_TIME_MIN_M_ID:
			case BR4S_ON_SET_TIME_MIN_M_ID:
			{
				/*if (!br4s_button_reaction.p_br4s_params->dev_info.flags.ma_connect)
				{*/
					if (br4s_button_reaction.p_br4s_params->dev_time.min < 59)
						br4s_button_reaction.p_br4s_params->dev_time.min++;
					else
						br4s_button_reaction.p_br4s_params->dev_time.min = 0;
					switch_param_change_indication();
				//}
			}	break;
			case BR4S_ON_SET_ALARM_ON_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_ALARM_OFF_M_ID;
				br4s_button_reaction.p_br4s_params->timers[0].timer_flags.timer_state = 0;
                br4s_button_reaction.p_br4s_params->timers[1].timer_flags.timer_state = 0;
			}	break;
			case BR4S_ON_SET_CONT_ALARM_ON_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.hours < 23)
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.hours++;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.hours = 0;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_ALARM_ON_MIN_M_ID:
			case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.minutes < 59)
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.minutes++;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.minutes = 0;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.hours < 23)
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.hours++;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.hours = 0;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_ALARM_OFF_MIN_M_ID:
			case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.minutes < 59)
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.minutes++;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.minutes = 0;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_ALARM_OFF_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_ALARM_ON_M_ID;
				br4s_button_reaction.p_br4s_params->timers[0].timer_flags.timer_state = 1;
                br4s_button_reaction.p_br4s_params->timers[1].timer_flags.timer_state = 1;
			}	break;
			case BR4S_ON_SET_FILTER_TIME_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time < (150 * 86400))
					br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time = 
					br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time + 30 * 86400;
				else
					br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time = 180 * 86400;
			}	break;
			default:
				break;
		}
	}
	else
	{
		switch (b_sub_state_mode)
		{
			case BR4S_ON_SET_HEATER_HOFF_M_ID:
			{
                            if (br4s_button_reaction.p_br4s_params->dev_info.flags.heater_present)
                            {
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_HEATER_TSET_M_ID;
				br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode = 0;
                            }
			}	break;
			case BR4S_ON_SET_HEATER_TSET_M_ID:
			{
                            if (br4s_button_reaction.p_br4s_params->dev_info.flags.heater_present)
                            {
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_HEATER_HOFF_M_ID;
				br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode = 1;
                            }
			}	break;
			case BR4S_ON_SET_CONT_TIME_HOUR_M_ID:
			case BR4S_ON_SET_TIME_HOUR_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->dev_time.hour > 0)
					br4s_button_reaction.p_br4s_params->dev_time.hour--;
				else
					br4s_button_reaction.p_br4s_params->dev_time.hour = 23;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_TIME_MIN_M_ID:
			case BR4S_ON_SET_TIME_MIN_M_ID:
			{
				/*if (!br4s_button_reaction.p_br4s_params->dev_info.flags.ma_connect)
				{*/
					if (br4s_button_reaction.p_br4s_params->dev_time.min > 0)
						br4s_button_reaction.p_br4s_params->dev_time.min--;
					else
						br4s_button_reaction.p_br4s_params->dev_time.min = 59;
					switch_param_change_indication();
				//}
			}	break;
			case BR4S_ON_SET_ALARM_ON_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_ALARM_OFF_M_ID;
				br4s_button_reaction.p_br4s_params->timers[0].timer_flags.timer_state = 0;
                br4s_button_reaction.p_br4s_params->timers[1].timer_flags.timer_state = 0;
			}	break;
			case BR4S_ON_SET_CONT_ALARM_ON_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.hours > 0)
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.hours--;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.hours = 23;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_ALARM_ON_MIN_M_ID:
			case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.minutes > 0)
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.minutes--;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_ON_TIMER_REM_IND].timer_time.minutes = 59;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_ALARM_OFF_HOUR_M_ID:
			case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.hours > 0)
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.hours--;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.hours = 23;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_CONT_ALARM_OFF_MIN_M_ID:
			case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.minutes > 0)
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.minutes--;
				else
					br4s_button_reaction.p_br4s_params->timers[BR4S_OFF_TIMER_REM_IND].timer_time.minutes = 59;
				switch_param_change_indication();
			}	break;
			case BR4S_ON_SET_ALARM_OFF_M_ID:
			{
				br4s_button_reaction.p_menu_state->sub_state_mode = 
					BR4S_ON_SET_ALARM_ON_M_ID;
				br4s_button_reaction.p_br4s_params->timers[0].timer_flags.timer_state = 1;
                br4s_button_reaction.p_br4s_params->timers[1].timer_flags.timer_state = 1;
			}	break;
			case BR4S_ON_SET_FILTER_TIME_M_ID:
			{
				if (br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time > 30 * 86400)
					br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time = 
					br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time - 30 * 86400;
				else
					br4s_button_reaction.p_br4s_params->dev_info.counters.filter_time = 30 * 86400;
			}	break;
			default:
				break;
		}
	}
	
	return ret_val;
}

static bool check_save_param_condition (uint32_t button_id)
{
	bool ret_val = false;
	uint32_t b_local_sub_state_mode = 
		br4s_button_reaction.p_menu_state->sub_state_mode;
	switch (button_id)
	{
		case BTN_SPUP_ID:
		{
			if ((b_local_sub_state_mode == BR4S_ON_SET_HEATER_HOFF_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_HEATER_TSET_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_TIME_MIN_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_ON_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_ON_MIN_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_OFF_MIN_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_OFF_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_FILTER_TIME_M_ID))
			{
				ret_val = true;
			}
		}	break;
		case BTN_SPDW_ID:
		{
			if ((b_local_sub_state_mode == BR4S_ON_SET_HEATER_HOFF_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_HEATER_TSET_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_TIME_HOUR_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_ON_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_ON_HOUR_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_OFF_HOUR_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_ALARM_OFF_M_ID) ||
				(b_local_sub_state_mode == BR4S_ON_SET_FILTER_TIME_M_ID))
			{
				ret_val = true;
			}
		}	break;
		default:
			break;
	}
	return ret_val;
}

static uint32_t send_current_param_value_to_breezer(void)
{
	static uint8_t com_type_id;
	uint32_t ret_val = NRF_SUCCESS;
	uint32_t b_local_sub_state_mode = 
		br4s_button_reaction.p_menu_state->sub_state_mode;

	switch (b_local_sub_state_mode)
	{
		case BR4S_ON_SET_HEATER_HOFF_M_ID:
		case BR4S_ON_SET_HEATER_TSET_M_ID:
		{
			com_type_id = BR4S_SET_PARAMS_ID;
			
		}	break;
		case BR4S_ON_SET_ALARM_ON_M_ID:
		case BR4S_ON_SET_ALARM_OFF_M_ID:
		{
			*br4s_button_reaction.p_br4s_state_valid = 0;
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
								NULL, NULL, STOP_DEV_POLL);
			com_type_id = BR4S_SET_OFF_TIMER_OFF_ID;
			br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
			com_type_id = BR4S_SET_ON_TIMER_OFF_ID;
			br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
			return ret_val;
		}	break;
		case BR4S_ON_SET_ALARM_ON_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_ON_MIN_M_ID:
		{
			com_type_id = BR4S_SET_ON_TIMER_ID;
		}	break;

		case BR4S_ON_SET_ALARM_OFF_HOUR_M_ID:
		case BR4S_ON_SET_ALARM_OFF_MIN_M_ID:
		{
			com_type_id = BR4S_SET_OFF_TIMER_ID;
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
	
	*br4s_button_reaction.p_br4s_state_valid = 0;
	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
	return ret_val;
} 

static uint32_t evt_br_menu_void(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	/*dummy handler*/
	return ret_val;
}
static uint32_t evt_br_menu_set_srv_temp(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	/*static uint8_t com_type_id = BR4S_SET_SRV_MODE_ID;

	if(*br4s_button_reaction.p_br4s_state_valid == 1)
	{
		*br4s_button_reaction.p_br4s_state_valid = 0;
		br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
		br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
	}*/
	return ret_val;
}
static uint32_t evt_br_menu_bond_erase(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	if(br4s_button_reaction.p_brc_main != NULL)
	{
		br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
				NULL, NULL, BLE_ERASE_BONDS);
		/*br4s_button_reaction.p_menu_state->state = BR4S_MENU_INIT_ID;
		br4s_button_reaction.p_menu_state->sub_state = BR4S_INIT_ERASE_BONDS_M_ID;
		br4s_menu_vptr.execute(NULL, NULL, NULL);*/ /*This statement leads to unknown screen after Power+Disp! Check it*/
	}
	return ret_val;
}

static uint32_t evt_br_menu_power(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BR4S_SET_PARAMS_ID;
	
	if(*br4s_button_reaction.p_br4s_state_valid != 1)
		return NRF_ERROR_BUSY;
	
	*br4s_button_reaction.p_br4s_state_valid = 0;
	br4s_button_reaction.p_br4s_params->dev_info.flags.power_state =
		~br4s_button_reaction.p_br4s_params->dev_info.flags.power_state;
	if (br4s_button_reaction.p_br4s_params->dev_info.flags.power_state)
	{
		br4s_button_reaction.p_menu_state->state = BR4S_MENU_ON_ID;
		br4s_button_reaction.p_menu_state->sub_state = BR4S_ON_DEFAULT_M_ID;
		br4s_button_reaction.p_menu_state->sub_state_mode = BR4S_ON_DEFAULT_TOUT_M_ID;
	}
	else
	{
		br4s_button_reaction.p_menu_state->state = BR4S_MENU_OFF_ID;
		br4s_button_reaction.p_menu_state->sub_state = BR4S_OFF_DEFAULT_M_ID;
		br4s_button_reaction.p_menu_state->sub_state_mode = BR4S_DEFAULT_MODE_ID;
	}
	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
			NULL, NULL, STOP_DEV_POLL);
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);

	return ret_val;
}

static uint32_t evt_br_menu_continues_param_change(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t com_type_id, button_id;

	button_id = *(uint8_t *)p_arg;

	switch (br4s_button_reaction.p_menu_state->sub_state)
	{
		case BR4S_ON_DEFAULT_M_ID:
		{
			com_type_id = BR4S_SET_PARAMS_ID;
			br4s_button_reaction.cont_param_change_on = 1;
			*br4s_button_reaction.p_br4s_state_valid = 0;
			switch (button_id)
			{
				case BTN_SPUP_ID:
				case BTN_SPDW_ID:
				{
					if (br4s_button_reaction.cont_param_change_on == 1)
					{
						br4s_button_reaction.cont_param_change_on = 0;
					}
					*br4s_button_reaction.p_br4s_state_valid = 1;
				}	break;
				case BTN_TUP_ID:
				{
					if (!br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode && br4s_button_reaction.p_br4s_params->dev_info.flags.heater_present)
					{
						if ((br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_TSET_M_ID) &&
							(br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_CONT_TSET_M_ID))
						{
							br4s_button_reaction.p_menu_state->sub_state_mode = 
								BR4S_ON_DEFAULT_TSET_M_ID;
						}
						else
						{
							if (br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp < 25)
							{
								br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp++;
							}
							else
							{
								br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp = 25;
							}
						}
						switch_param_change_indication();
						br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
							NULL, NULL, STOP_DEV_POLL);
						br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
							(void *)p_arg, NULL, START_CONT_PARAM_CHANGE_TIMER);
					}
					else
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_DEFAULT_HOFF_M_ID;
					}
				}	break;
				
				case BTN_TDW_ID:
				{
					if (!br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode && br4s_button_reaction.p_br4s_params->dev_info.flags.heater_present)
					{
						if ((br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_TSET_M_ID) &&
							(br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_CONT_TSET_M_ID))
						{
							br4s_button_reaction.p_menu_state->sub_state_mode = 
								BR4S_ON_DEFAULT_TSET_M_ID;
						}
						else
						{
							if (br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp > 0)
							{
								br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp--;
							}
							else
							{
								br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp = 0;
							}
						}
						switch_param_change_indication();
						br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
							NULL, NULL, STOP_DEV_POLL);
						br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
							(void *)p_arg, NULL, START_CONT_PARAM_CHANGE_TIMER);
					}
					else
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_DEFAULT_HOFF_M_ID;
					}
				}	break;
				default:
					break;
			}
		}	break;
		case BR4S_ON_SET_M_ID:
		{
			br4s_button_reaction.cont_param_change_on = 1;
			*br4s_button_reaction.p_br4s_state_valid = 0;
			switch (button_id)
			{
				case BTN_SPUP_ID:
				case BTN_SPDW_ID:
				{
					if (br4s_button_reaction.cont_param_change_on == 1)
					{
						br4s_button_reaction.cont_param_change_on = 0;
					}
				}	break;
				case BTN_TUP_ID:
				{
					change_current_menu_set_param(true);
					br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
						(void *)p_arg, NULL, START_CONT_PARAM_CHANGE_TIMER);
				}	break;
				case BTN_TDW_ID:
				{
					change_current_menu_set_param(false);
					br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
						(void *)p_arg, NULL, START_CONT_PARAM_CHANGE_TIMER);
				}	break;
				default:
					break;
			}
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
		}	break;
		default:
			break;
	}
	
	return ret_val;
}

static uint32_t evt_br_menu_single_param_change(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id, button_id;

	button_id = *(uint8_t *)p_arg;
	com_type_id = BR4S_SET_PARAMS_ID;

	switch (br4s_button_reaction.p_menu_state->sub_state)
	{
		case BR4S_ON_DEFAULT_M_ID:
		{
			switch (button_id)
			{
				case BTN_SPUP_ID:
				{
					if (br4s_button_reaction.cont_param_change_on == 2)
					{
						br4s_button_reaction.cont_param_change_on = 0;
					}
					if (!br4s_button_reaction.p_br4s_params->waiting_responce)
					{
						if (br4s_button_reaction.p_br4s_params->dev_info.main_params.fan_speed < 
							br4s_button_reaction.p_br4s_params->dev_info.max_speed)
						{
							br4s_button_reaction.p_br4s_params->dev_info.main_params.fan_speed++;
						}
						else
						{
							br4s_button_reaction.p_br4s_params->br4s_invalid_speed = 1;
						}
					}
				}	break;
				case BTN_SPDW_ID:
				{
					if (br4s_button_reaction.cont_param_change_on == 2)
					{
						br4s_button_reaction.cont_param_change_on = 0;
					}
					if (!br4s_button_reaction.p_br4s_params->waiting_responce)
					{
						if	(br4s_button_reaction.p_br4s_params->dev_info.main_params.fan_speed > 1)
						{
							br4s_button_reaction.p_br4s_params->dev_info.main_params.fan_speed--;
						}
						else
						{
							br4s_button_reaction.p_br4s_params->br4s_invalid_speed = 1;
						}
					}
				}	break;
				case BTN_TUP_ID:
				{
					if (!br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode)
					{
						if ((br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_TSET_M_ID) &&
							(br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_CONT_TSET_M_ID))
						{
							br4s_button_reaction.p_menu_state->sub_state_mode = 
								BR4S_ON_DEFAULT_TSET_M_ID;
						}
						else
						{
							if (br4s_button_reaction.cont_param_change_on == 2)
							{
								br4s_button_reaction.cont_param_change_on = 0;
							}
							else
							{
								if (br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp < 25)
								{
									br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp++;
								}
								else
								{
									br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp = 25;
								}
							}
						}
					}
					else
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_DEFAULT_HOFF_M_ID;
					}
				}	break;
				case BTN_TDW_ID:
				{
					if (!br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode)
					{
						if ((br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_TSET_M_ID) &&
							(br4s_button_reaction.p_menu_state->sub_state_mode != 
							 BR4S_ON_DEFAULT_CONT_TSET_M_ID))
						{
							br4s_button_reaction.p_menu_state->sub_state_mode = 
								BR4S_ON_DEFAULT_TSET_M_ID;
						}
						else
						{
							if (br4s_button_reaction.cont_param_change_on == 2)
							{
								br4s_button_reaction.cont_param_change_on = 0;
							}
							else
							{
								if (br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp > 0)
								{
									br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp--;
								}
								else
								{
									br4s_button_reaction.p_br4s_params->dev_info.main_params.target_temp = 0;
								}
							}
						}
					}
					else
					{
						br4s_button_reaction.p_menu_state->sub_state_mode = 
							BR4S_ON_DEFAULT_HOFF_M_ID;
					}
				}	break;
				default:
					break;
			}
			switch_param_change_indication();
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
			if ((br4s_button_reaction.cont_param_change_on == 0) &&
				(!br4s_button_reaction.p_br4s_params->waiting_responce))
			{
				com_type_id = BR4S_SET_PARAMS_ID;
				*br4s_button_reaction.p_br4s_state_valid = 0;
				br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
			}
		}	break;
		case BR4S_ON_SET_M_ID:
		{
			switch (button_id)
			{
				case BTN_SPUP_ID:
				{
					if (br4s_button_reaction.cont_param_change_on == 2)
					{
						br4s_button_reaction.cont_param_change_on = 0;
					}
					if(check_save_param_condition(button_id))
					{
						send_current_param_value_to_breezer();
					}
					br4s_evt_switch_to_next_menu_set_sub_state_mode(NEXT_MODE);
				}	break;
				case BTN_SPDW_ID:
				{
					if (br4s_button_reaction.cont_param_change_on == 2)
					{
						br4s_button_reaction.cont_param_change_on = 0;
					}
					if (check_save_param_condition(button_id))
					{
						send_current_param_value_to_breezer();
					}
					br4s_evt_switch_to_next_menu_set_sub_state_mode(PREV_MODE);
				}	break;
				case BTN_TUP_ID:
				{
					change_current_menu_set_param(true);
				}	break;
				case BTN_TDW_ID:
				{
					change_current_menu_set_param(false);
				}	break;
				default:
					break;
			}
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
		}	break;
		default:
			break;
	}
	return ret_val;
}

static uint32_t evt_br_menu_switch_snd_state(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BR4S_SET_PARAMS_ID;
	
	if(*br4s_button_reaction.p_br4s_state_valid != 1)
		return NRF_ERROR_BUSY;
	
	*br4s_button_reaction.p_br4s_state_valid = 0;
	br4s_button_reaction.p_br4s_params->dev_info.flags.sound_ind_state = 
		~br4s_button_reaction.p_br4s_params->dev_info.flags.sound_ind_state;
	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
			NULL, NULL, STOP_DEV_POLL);
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);

	return ret_val;
}

static uint32_t evt_br_menu_start_ma_pairing(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	/*static uint8_t com_type_id = BR4S_START_MA_PAIRING_ID;

	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
			NULL, NULL, STOP_DEV_POLL);
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);*/

	return ret_val;
}

static uint32_t evt_br_menu_reset_userset_to_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BR4S_HARD_RESET_ID;

	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
			NULL, NULL, STOP_DEV_POLL);
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);

	return ret_val;
}

 static uint32_t evt_br_menu_on_default_switch_state(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	uint8_t * p_local_sub_state_mode;
	
	if(br4s_button_reaction.p_menu_state->sub_state_mode ==  
			BR4S_ON_DEFAULT_INVALID_SPEED_M_ID)
	{
		p_local_sub_state_mode = 
			br4s_button_reaction.p_menu_state->p_prev_menu_sub_state_mode;
	}
	else
	{
		p_local_sub_state_mode = 
			(uint8_t *)&br4s_button_reaction.p_menu_state->sub_state_mode;
	}
	
	if (*p_local_sub_state_mode >= (BR4S_ON_DEFAULT_M_NUM - 1))
	{
		*(p_local_sub_state_mode) = BR4S_ON_DEFAULT_TOUT_M_ID;
	}
	else
	{
		*(p_local_sub_state_mode) += 1;
	}
	return ret_val;
}

static uint32_t evt_br_menu_on_switch_default_to_set(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BR4S_REQ_ON_TIMER_ID;
	
	if(*br4s_button_reaction.p_br4s_state_valid != 1)
		return NRF_ERROR_BUSY;
	
	*br4s_button_reaction.p_br4s_state_valid = 0;
	
	br4s_button_reaction.p_menu_state->sub_state = BR4S_ON_SET_M_ID;

	if (!br4s_button_reaction.p_br4s_params->dev_info.flags.heater_mode)
		br4s_button_reaction.p_menu_state->sub_state_mode = BR4S_ON_SET_HEATER_TSET_M_ID;
	else
		br4s_button_reaction.p_menu_state->sub_state_mode = BR4S_ON_SET_HEATER_HOFF_M_ID;
	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, STOP_DEV_POLL);
	
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
        com_type_id = BR4S_REQ_OFF_TIMER_ID;        
        br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);
	return ret_val;
}

static uint32_t evt_br_menu_on_switch_set_to_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	send_current_param_value_to_breezer();
	br4s_button_reaction.p_menu_state->sub_state = BR4S_ON_DEFAULT_M_ID;
	br4s_button_reaction.p_menu_state->sub_state_mode = BR4S_ON_DEFAULT_TOUT_M_ID;
	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, START_DEV_POLL);
	
	return ret_val;
}

static uint32_t evt_br_menu_on_default_switch_gate(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BR4S_SET_PARAMS_ID;

	if(*br4s_button_reaction.p_br4s_state_valid != 1)
		return NRF_ERROR_BUSY;
	*br4s_button_reaction.p_br4s_state_valid = 0;
		
	if (br4s_button_reaction.p_br4s_params->dev_info.main_params.substate == BR4S_INFLOW_SUBSTATE)
		br4s_button_reaction.p_br4s_params->dev_info.main_params.substate = BR4S_RECYCLE_SUBSTATE;
	else if (br4s_button_reaction.p_br4s_params->dev_info.main_params.substate == BR4S_RECYCLE_SUBSTATE)
		br4s_button_reaction.p_br4s_params->dev_info.main_params.substate = BR4S_INFLOW_SUBSTATE;
	br4s_protocol_vptr.input_data((void *)&com_type_id, NULL);

	return ret_val;
}

static uint32_t evt_br_menu_remote_test(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, STOP_DEV_POLL);
	br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, START_SCREEN_TEST);

	return ret_val;
}

/*class methods section*/
static uint32_t init_button_reaction_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	br4s_main_t * p_local_br4s_main;
	
	p_local_br4s_main = (br4s_main_t *)p_context;

	br4s_button_reaction.p_br4s_callback = p_local_br4s_main->p_br4s_callback;
	br4s_button_reaction.p_br4s_params = &p_local_br4s_main->br4s_state;
	br4s_button_reaction.p_brc_main	= p_local_br4s_main->p_brc_main;
	br4s_button_reaction.p_br4s_state_valid	= &p_local_br4s_main->br4s_state_valid;
	br4s_button_reaction.p_menu_state = p_local_br4s_main->p_menu_state;
	br4s_button_reaction.p_expected_resp_type = &p_local_br4s_main->expected_resp_type;
	br4s_button_reaction.button_in_process = 0;
	return ret_val;
}

static uint32_t deinit_button_reaction_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	br4s_button_reaction.p_br4s_params = NULL;
	br4s_button_reaction.button_in_process = 0;
	return ret_val;
}

static uint32_t evt_power_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_PWR_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_POWER_ID);
	return ret_val;
}

static uint32_t evt_state_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_DISP_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_STATE_ID);
	return ret_val;
}

static uint32_t evt_set_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_MENU_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_SET_ID);
	return ret_val;
}

static uint32_t evt_gate_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_FLOW_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_GATE_ID);
	return ret_val;
}

static uint32_t evt_fan_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_SPUP_ID;
	uint8_t * p_context;
	
	if (*br4s_button_reaction.p_expected_resp_type != NULL)
		return NRF_ERROR_BUSY;
	
	p_context = (uint8_t *)p_arg;
	if (p_context == NULL)
	{
		if (br4s_button_reaction.cont_param_change_on == 1)
		{
			br4s_button_reaction.cont_param_change_on = 2;
			switch_param_change_indication();
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, STOP_CONT_PARAM_CHANGE_TIMER);
		}
	}
	execute_button_evt(&button_id, BUTTON_INT_EVT_FAN_UP_ID);
	return ret_val;
}

static uint32_t evt_fan_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_SPDW_ID;
	uint8_t * p_context;
	
	if (*br4s_button_reaction.p_expected_resp_type != NULL)
		return NRF_ERROR_BUSY;
	
	p_context = (uint8_t *)p_arg;
	if (p_context == NULL)
	{
		if (br4s_button_reaction.cont_param_change_on == 1)
		{
			br4s_button_reaction.cont_param_change_on = 2;
			switch_param_change_indication();
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, STOP_CONT_PARAM_CHANGE_TIMER);
		}
	}
	execute_button_evt(&button_id, BUTTON_INT_EVT_FAN_DOWN_ID);
	return ret_val;
}

static uint32_t evt_temp_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_TUP_ID;
	uint8_t * p_context;
	
	if (*br4s_button_reaction.p_expected_resp_type != NULL)
		return NRF_ERROR_BUSY;
	
	p_context = (uint8_t *)p_arg;
	if (p_context == NULL)
	{
		if (br4s_button_reaction.cont_param_change_on == 1)
		{
			br4s_button_reaction.cont_param_change_on = 2;
			switch_param_change_indication();
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, STOP_CONT_PARAM_CHANGE_TIMER);
		}
	}
	execute_button_evt(&button_id, BUTTON_INT_EVT_TEMP_UP_ID);	
	return ret_val;
}

static uint32_t evt_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_TDW_ID;
	uint8_t * p_context;
	
	if (*br4s_button_reaction.p_expected_resp_type != NULL)
		return NRF_ERROR_BUSY;
	
	p_context = (uint8_t *)p_arg;
	if (p_context == NULL)
	{
		if (br4s_button_reaction.cont_param_change_on == 1)
		{
			br4s_button_reaction.cont_param_change_on = 2;
			switch_param_change_indication();
			br4s_button_reaction.p_br4s_callback((void *)&br4s_button_reaction, 
					NULL, NULL, STOP_CONT_PARAM_CHANGE_TIMER);
		}
	}
	execute_button_evt(&button_id, BUTTON_INT_EVT_TEMP_DOWN_ID);		
	return ret_val;
}

static uint32_t evt_long_fan_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_SPUP_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_FAN_UP_ID);
	return ret_val;
}

static uint32_t evt_long_fan_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_SPDW_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_FAN_DOWN_ID);
	return ret_val;
}

static uint32_t evt_long_temp_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_TUP_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_TEMP_UP_ID);
	return ret_val;
}

static uint32_t evt_long_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BTN_TDW_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_TEMP_DOWN_ID);
	return ret_val;
}

static uint32_t evt_long_state_set_gate_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_STATE_SET_GATE_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_STATE_SET_GATE_ID);
	return ret_val;
}

static uint32_t evt_long_state_set_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_STATE_SET_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_STATE_SET_ID);
	return ret_val;
}

static uint32_t evt_long_state_set_fan_up_fan_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_STATE_SET_FAN_UP_FAN_DOWN_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_STATE_SET_FAN_UP_FAN_DOWN_ID);
	return ret_val;
}

static uint32_t evt_long_set_temp_up_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_SET_TEMP_UP_TEMP_DOWN_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_SET_TEMP_UP_TEMP_DOWN_ID);
	return ret_val;
}

static uint32_t evt_long_temp_up_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_SET_TEMP_UP_TEMP_DOWN_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_SET_TEMP_UP_TEMP_DOWN_ID);
	return ret_val;
}

static uint32_t evt_long_temp_down_power_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_TEMP_DOWN_POWER_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_TEMP_DOWN_POWER_ID);
	return ret_val;
}

static uint32_t evt_long_state_power_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_STATE_POWER_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_STATE_POWER_ID);
	return ret_val;
}

static uint32_t evt_long_gate_set_temp_down_fan_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t button_id;
	button_id = BUTTON_INT_EVT_LONG_GATE_SETTEMP_DOWN_FAN_UP_ID;
			
	execute_button_evt(&button_id, BUTTON_INT_EVT_LONG_GATE_SETTEMP_DOWN_FAN_UP_ID);
	return ret_val;
}
/*Public functions*/
/******************************* END_OF_FILE **********************************/