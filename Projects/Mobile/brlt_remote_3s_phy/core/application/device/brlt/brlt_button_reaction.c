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
typedef struct _tag_brlt_button_reactiont_menu_t
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
} brlt_button_reactiont_menu_t;

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

static uint32_t evt_br_menu_reset_userset_to_default(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_remote_test(void * p_arg, uint32_t length);

static uint32_t evt_br_menu_on_default_switch_state(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_kiv_switch_state(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_switch_default_to_set(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_switch_set_to_default(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_switch_default_to_kiv(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_switch_kiv_to_default(void * p_arg, uint32_t length);
/*static uint32_t evt_br_menu_on_switch_kiv_to_kiv_set(void * p_arg, uint32_t length);*/
static uint32_t evt_br_menu_off_switch_to_kiv(void * p_arg, uint32_t length);
static uint32_t evt_br_menu_on_switch_kiv_set_to_kiv(void * p_arg, uint32_t length);

static bool can_be_changed_continuosly(uint32_t state);
static uint32_t switch_param_change_indication(void);
/*Exported constants*/
const button_reaction_m_list_t	brlt_button_reaction_vptr = {
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
static uint8_t cont_param_change_on = 0;
static brlt_button_reaction_t brlt_button_reaction;

static const brlt_button_reactiont_menu_t brlt_menu_init_clear_button_reaction = {0};
static const brlt_button_reactiont_menu_t brlt_menu_init_erase_bonds_button_reaction = {0};

static const brlt_button_reactiont_menu_t brlt_menu_on_clear_button_reaction = {0};
static const brlt_button_reactiont_menu_t brlt_menu_on_default_button_reaction =
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_on_default_switch_state,
	.set	= &evt_br_menu_on_switch_default_to_set,
	.gate	= &evt_br_menu_on_switch_default_to_kiv,
	.fan_up		= &evt_br_menu_single_param_change,
	.fan_down	= &evt_br_menu_single_param_change,
	.temp_up	= &evt_br_menu_single_param_change,
	.temp_down	= &evt_br_menu_single_param_change,
	
	.long_fan_up 	= &evt_br_menu_continues_param_change,
	.long_fan_down	= &evt_br_menu_continues_param_change,
	.long_temp_up	= &evt_br_menu_continues_param_change,
	.long_temp_down	= &evt_br_menu_continues_param_change,
		
	.long_state_set_gate 			= &evt_br_menu_switch_snd_state,
	.long_state_set					= &evt_br_menu_void,
	.long_state_set_fan_up_fan_down	= &evt_br_menu_reset_userset_to_default,
	.long_set_temp_up_temp_down		= &evt_br_menu_set_srv_temp,
	.long_temp_up_temp_down			= &evt_br_menu_void,
	.long_temp_down_power			= &evt_br_menu_void,
	.long_state_power				= &evt_br_menu_bond_erase,
	.long_gate_set_temp_down_fan_up	= &evt_br_menu_remote_test,
};
static const brlt_button_reactiont_menu_t brlt_menu_on_set_button_reaction =
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
static const brlt_button_reactiont_menu_t brlt_menu_on_error_button_reaction = 
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

static const brlt_button_reactiont_menu_t brlt_menu_on_kiv_button_reaction = 
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_on_kiv_switch_state,
	.set	= &evt_br_menu_void, //&evt_br_menu_on_switch_kiv_to_kiv_set,
	.gate	= &evt_br_menu_on_switch_kiv_to_default,
	.fan_up	= &evt_br_menu_single_param_change,
	.fan_down	= &evt_br_menu_single_param_change,
	.temp_up	= &evt_br_menu_single_param_change,
	.temp_down	= &evt_br_menu_single_param_change,
	
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

static const brlt_button_reactiont_menu_t brlt_menu_on_kiv_set_button_reaction =
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_void,
	.set	= &evt_br_menu_on_switch_kiv_set_to_kiv,
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

static const brlt_button_reactiont_menu_t brlt_menu_off_clear_button_reaction = {0};
static const brlt_button_reactiont_menu_t brlt_menu_off_default_button_reaction = 
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_void,
	.set	= &evt_br_menu_void,
	.gate	= &evt_br_menu_off_switch_to_kiv,
	.fan_up	= &evt_br_menu_void,
	.fan_down	= &evt_br_menu_void,
	.temp_up	= &evt_br_menu_void,
	.temp_down	= &evt_br_menu_void,
	
	.long_fan_up 	= &evt_br_menu_void,
	.long_fan_down	= &evt_br_menu_void,
	.long_temp_up	= &evt_br_menu_void,
	.long_temp_down	= &evt_br_menu_void,
		
	.long_state_set_gate 			= &evt_br_menu_switch_snd_state,
	.long_state_set					= &evt_br_menu_void,
	.long_state_set_fan_up_fan_down	= &evt_br_menu_reset_userset_to_default,
	.long_set_temp_up_temp_down		= &evt_br_menu_void,
	.long_temp_up_temp_down			= &evt_br_menu_void,
	.long_temp_down_power			= &evt_br_menu_void,
	.long_state_power				= &evt_br_menu_bond_erase,
	.long_gate_set_temp_down_fan_up	= &evt_br_menu_remote_test,
};
static const brlt_button_reactiont_menu_t brlt_menu_off_error_button_reaction = 
{
	.power	= &evt_br_menu_power,
	.state	= &evt_br_menu_void,
	.set	= &evt_br_menu_void,
	.gate	= &evt_br_menu_off_switch_to_kiv,
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

static const brlt_button_reactiont_menu_t * brlt_menu_init_button_reaction[BRLT_INIT_M_NUM] = 
{
	&brlt_menu_init_clear_button_reaction,
	&brlt_menu_init_erase_bonds_button_reaction,
};
static const brlt_button_reactiont_menu_t * brlt_menu_on_button_reaction[BRLT_ON_M_NUM] = 
{
	&brlt_menu_on_clear_button_reaction,
	&brlt_menu_on_default_button_reaction,
	&brlt_menu_on_set_button_reaction,
	&brlt_menu_on_error_button_reaction,
	&brlt_menu_on_kiv_button_reaction,
	&brlt_menu_on_kiv_set_button_reaction,
};
static const brlt_button_reactiont_menu_t * brlt_menu_off_button_reaction[BRLT_OFF_M_NUM] = 
{
	&brlt_menu_off_clear_button_reaction,
	&brlt_menu_off_default_button_reaction,
	&brlt_menu_off_error_button_reaction,
};

static const brlt_button_reactiont_menu_t ** brlt_menu_button_reaction[BRLT_MENU_SIZE] = 
{
	brlt_menu_init_button_reaction,
	brlt_menu_on_button_reaction,
	brlt_menu_off_button_reaction,
};
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/

/*Private functions*/
static uint32_t switch_param_change_indication(void)
{
	uint32_t ret_val = NRF_SUCCESS; 
	uint32_t b_local_state = brlt_button_reaction.p_menu_state->sub_state;
	uint32_t b_local_sub_state = brlt_button_reaction.p_menu_state->sub_state_mode;

	if(b_local_state == BRLT_ON_SET_M_ID)
	{
		if(cont_param_change_on == 1)
		{
			switch (b_local_sub_state)
			{
				case BRLT_ON_SET_FILTER_TIME_M_ID:
				{
					brlt_button_reaction.p_menu_state->sub_state_mode = 
						BRLT_ON_SET_FILTER_TIME_M_ID;
				}	break;
				default:
					break;
			}
		}
		else
		{
			switch (b_local_sub_state)
			{
				case BRLT_ON_SET_FILTER_TIME_M_ID:
				{
					brlt_button_reaction.p_menu_state->sub_state_mode = 
						BRLT_ON_SET_FILTER_TIME_M_ID;
				}	break;
				default:
					break;
			}
		}
	}
	else if (b_local_state == BRLT_ON_DEFAULT_M_ID)
	{
		uint8_t *p_local_sub_state_mode;
		if (brlt_button_reaction.p_menu_state->sub_state_mode == 
			BRLT_ON_DEFAULT_INVALID_SPEED_M_ID)
		{
			p_local_sub_state_mode = 
				brlt_button_reaction.p_menu_state->p_prev_menu_sub_state_mode;
		}
		else
		{
			p_local_sub_state_mode = 
				(uint8_t *)&brlt_button_reaction.p_menu_state->sub_state_mode;
		}
		if(cont_param_change_on == 1)
		{
			switch (b_local_sub_state)
			{
				case BRLT_ON_DEFAULT_TSET_M_ID:
				{
					*p_local_sub_state_mode = BRLT_ON_DEFAULT_CONT_TSET_M_ID;
				}	break;
				default:
					break;
			}
		}
		else
		{
			switch (b_local_sub_state)
			{
				case BRLT_ON_DEFAULT_CONT_TSET_M_ID:
				{
					*p_local_sub_state_mode = BRLT_ON_DEFAULT_TSET_M_ID;
				}	break;
				default:
					break;
			}
		}
	}
	return ret_val;
}

static bool can_be_changed_continuosly(uint32_t state)
{
	bool ret_val = false;
	
	if (brlt_button_reaction.p_menu_state->state != BRLT_MENU_ON_ID)
	{
		return ret_val;
	}
	
	if (brlt_button_reaction.p_menu_state->sub_state == BRLT_ON_DEFAULT_M_ID)
	{
		switch (state)
		{
			case BRLT_ON_DEFAULT_HOFF_M_ID:
			case BRLT_ON_DEFAULT_H_INACTIVE_M_ID:
			case BRLT_ON_DEFAULT_DEV_REPORT_M_ID:
			case BRLT_ON_DEFAULT_DEV_FW_REPORT_M_ID:
			case BRLT_ON_DEFAULT_INVALID_SPEED_M_ID:
			{
				return ret_val = false;
			}	break;
			case BRLT_ON_DEFAULT_TSET_M_ID:
			case BRLT_ON_DEFAULT_CONT_TSET_M_ID:
			case BRLT_ON_DEFAULT_TOUT_M_ID:
			{
				return ret_val = true;
			}	break;
			default:
				break;
		}
	}
	else if (brlt_button_reaction.p_menu_state->sub_state == BRLT_ON_SET_M_ID)
	{
		switch (state)
		{
			case BRLT_ON_SET_HEATER_HOFF_M_ID:
			case BRLT_ON_SET_HEATER_TSET_M_ID:
			case BRLT_ON_SET_HEATER_H_INACTIVE_M_ID:
			{
				return ret_val = false; 
			}	break;
			case BRLT_ON_SET_FILTER_TIME_M_ID:
			{
				return ret_val = true;
			}	break;
			default:
				break;
		}
	}
	else if(brlt_button_reaction.p_menu_state->sub_state == BRLT_ON_KIV_SET_M_ID)
	{
		switch (state)
		{
			case BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID:
			{
				return ret_val = false; 
			}	break;
			case BRLT_ON_KIV_SET_AUTOKIV_TEMP_M_ID:
			{
				return ret_val = true;
			}	break;
			default:
				break;
		}
	}
	else
	{
		return ret_val = false; 
	}
	return ret_val = false; 
}

static uint32_t execute_button_evt(uint8_t *p_button_id, uint8_t evt_id)
{
	uint32_t ret_val = NRF_SUCCESS; 
	uint32_t b_local_state;
	uint32_t b_local_sub_state;
	
	b_local_state = brlt_button_reaction.p_menu_state->state;
	b_local_sub_state = brlt_button_reaction.p_menu_state->sub_state;
	
	switch (evt_id)
	{
		case BUTTON_INT_EVT_POWER_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->power !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->power((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_STATE_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->state !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->state((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_SET_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->set !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->set((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_GATE_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->gate !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->gate((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_FAN_UP_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->fan_up !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->fan_up((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_FAN_DOWN_ID: 
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->fan_down !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->fan_down((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_TEMP_UP_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->temp_up !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->temp_up((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_TEMP_DOWN_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->temp_down !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->temp_down((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_FAN_UP_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_up !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_up((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_FAN_DOWN_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_down !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_fan_down((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_TEMP_UP_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_up !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_up((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_TEMP_DOWN_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_SET_GATE_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_gate !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_gate((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_SET_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_SET_FAN_UP_FAN_DOWN_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_fan_up_fan_down !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_set_fan_up_fan_down((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_SET_TEMP_UP_TEMP_DOWN_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_set_temp_up_temp_down !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_set_temp_up_temp_down((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_TEMP_DOWN_POWER_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down_power !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_temp_down_power((void *)p_button_id, NULL);
				return ret_val;
			}		
		}	break;
		case BUTTON_INT_EVT_LONG_STATE_POWER_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_power !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_state_power((void *)p_button_id, NULL);
				return ret_val;
			}
		}	break;
		case BUTTON_INT_EVT_LONG_GATE_SETTEMP_DOWN_FAN_UP_ID:
		{
			if(brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_gate_set_temp_down_fan_up !=0)
			{
				brlt_menu_button_reaction[b_local_state][b_local_sub_state]->long_gate_set_temp_down_fan_up((void *)p_button_id, NULL);
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
		brlt_button_reaction.p_menu_state->sub_state_mode;
	
	if (state)
	{
		switch (b_sub_state_mode)
		{
			case BRLT_ON_SET_HEATER_H_INACTIVE_M_ID:
			{
				brlt_button_reaction.p_menu_state->sub_state_mode = 
					BRLT_ON_SET_HEATER_TSET_M_ID;
				brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode = 1;
			}	break;
			case BRLT_ON_SET_HEATER_TSET_M_ID:
			{
				brlt_button_reaction.p_menu_state->sub_state_mode = 
					BRLT_ON_SET_HEATER_H_INACTIVE_M_ID;
				brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode = 0;
			}	break;
			case BRLT_ON_SET_FILTER_TIME_M_ID:
			{
				if(brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time <= (150*86400))
					brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time = 
					brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time + (30*86400);
				else
					brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time = 180*86400;
			}	break;
			case BRLT_ON_SET_HEATER_HOFF_M_ID:
			default:
				break;
		}
	}
	else
	{
		switch (b_sub_state_mode)
		{
			case BRLT_ON_SET_HEATER_H_INACTIVE_M_ID:
			{
				brlt_button_reaction.p_menu_state->sub_state_mode = 
					BRLT_ON_SET_HEATER_TSET_M_ID;
				brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode = 1;				
			}	break;
			case BRLT_ON_SET_HEATER_TSET_M_ID:
			{
				brlt_button_reaction.p_menu_state->sub_state_mode = 
					BRLT_ON_SET_HEATER_H_INACTIVE_M_ID;
				brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode = 0;				
			}	break;
			case BRLT_ON_SET_FILTER_TIME_M_ID:
			{
				if(brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time > (60*86400))
					brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time = 
					brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time - (30*86400);
				else
					brlt_button_reaction.p_brlt_params->dev_info.counters.filter_time = (30*86400);
			}	break;
			case BRLT_ON_SET_HEATER_HOFF_M_ID:
			default:
				break;
		}
	}
	return ret_val;
}

static uint32_t change_current_menu_kiv_set_param(bool state)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint32_t b_sub_state_mode = 
		brlt_button_reaction.p_menu_state->sub_state_mode;
	
	if (state)
	{
		switch (b_sub_state_mode)
		{
			case BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID:
				brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_state = 
					~brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_state;	
			break;
			case BRLT_ON_KIV_SET_AUTOKIV_TEMP_M_ID:
				if(++brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_temp > BRLT_AUTOKIV_MAX_TEMP)
					brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_temp = BRLT_AUTOKIV_MAX_TEMP;
			break;
			default:
				break;
		}
	}
	else
	{
		switch (b_sub_state_mode)
		{
			case BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID:
				brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_state = 
					~brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_state;	
			break;
			case BRLT_ON_KIV_SET_AUTOKIV_TEMP_M_ID:
				if(--brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_temp < BRLT_AUTOKIV_MIN_TEMP)
					brlt_button_reaction.p_brlt_params->dev_info.autokiv_info.autokiv_temp = BRLT_AUTOKIV_MIN_TEMP;
			break;
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
		brlt_button_reaction.p_menu_state->sub_state_mode;
	switch (button_id)
	{
		case BTN_SPUP_ID:
		case BTN_SPDW_ID:
		{
			if ((b_local_sub_state_mode == BRLT_ON_SET_HEATER_HOFF_M_ID) ||
				(b_local_sub_state_mode == BRLT_ON_SET_HEATER_TSET_M_ID) ||
				(b_local_sub_state_mode == BRLT_ON_SET_HEATER_H_INACTIVE_M_ID) ||
				(b_local_sub_state_mode == BRLT_ON_SET_FILTER_TIME_M_ID))
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
	if( brlt_button_reaction.p_menu_state->sub_state == BRLT_ON_SET_M_ID )
	{
		switch (brlt_button_reaction.p_menu_state->sub_state_mode)
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
	}
	else if ( brlt_button_reaction.p_menu_state->sub_state == BRLT_ON_KIV_SET_M_ID )
	{
		switch (brlt_button_reaction.p_menu_state->sub_state_mode)
		{
			case BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID:
			case BRLT_ON_KIV_SET_AUTOKIV_TEMP_M_ID:
			{
				com_type_id = BRLT_SET_AUTOKIV_PARAMS_ID;
			}	break;
			default:
				com_type_id = BRLT_SET_AUTOKIV_PARAMS_ID;
			break;
		}
	}

	*brlt_button_reaction.p_brlt_state_valid = 0;
	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
	brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
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
	static uint8_t com_type_id = BRLT_SET_SRV_MODE_ID;

	if(*brlt_button_reaction.p_brlt_state_valid == 1)
	{
		*brlt_button_reaction.p_brlt_state_valid = 0;
		brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
		brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
	}
	return ret_val;
}
static uint32_t evt_br_menu_bond_erase(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	if(brlt_button_reaction.p_brc_main != NULL)
	{
		brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
				NULL, NULL, BLE_ERASE_BONDS);
		/*brlt_button_reaction.p_menu_state->state = BRLT_MENU_INIT_ID;
		brlt_button_reaction.p_menu_state->sub_state = BRLT_INIT_ERASE_BONDS_M_ID;
		brlt_menu_vptr.execute(NULL, NULL, NULL);*/ /*See function in 3S!*/
	}
	return ret_val;
}
static uint32_t evt_br_menu_power(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BRLT_SET_PARAMS_ID;

	*brlt_button_reaction.p_brlt_state_valid = 0;

	if (brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_active)
	{
		brlt_button_reaction.p_brlt_params->dev_info.flags.power_mode = 0;
	  	brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_active = 0;
	} else
	{
		brlt_button_reaction.p_brlt_params->dev_info.flags.power_mode =
				~brlt_button_reaction.p_brlt_params->dev_info.flags.power_mode;
	}


	if (brlt_button_reaction.p_brlt_params->dev_info.flags.power_mode)
	{
		brlt_button_reaction.p_menu_state->state = BRLT_MENU_ON_ID;
		brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_DEFAULT_M_ID;
		brlt_button_reaction.p_menu_state->sub_state_mode = 
											BRLT_ON_SET_HEATER_H_INACTIVE_M_ID;
		brlt_button_reaction.p_brlt_params->dev_info.errors.flag_register = 
				brlt_button_reaction.p_brlt_params->dev_info.errors.flag_register 
				  												& 0x00FFFFFF;
	}
	else
	{
		brlt_button_reaction.p_menu_state->state = BRLT_MENU_OFF_ID;
		brlt_button_reaction.p_menu_state->sub_state = BRLT_OFF_DEFAULT_M_ID;
		brlt_button_reaction.p_menu_state->sub_state_mode = BRLT_DEFAULT_MODE_ID;
	}

	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);
	brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
	return ret_val;
}

static uint32_t evt_br_menu_continues_param_change(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t com_type_id, button_id;

	button_id = *(uint8_t *)p_arg;

	switch (brlt_button_reaction.p_menu_state->sub_state)
	{
		case BRLT_ON_DEFAULT_M_ID:
		{
			com_type_id = BRLT_SET_PARAMS_ID;
			cont_param_change_on = 1;
			*brlt_button_reaction.p_brlt_state_valid = 0;

			switch (button_id)
			{
				case BTN_SPUP_ID:
				case BTN_SPDW_ID:
				{
					if (cont_param_change_on == 1)
					{
						cont_param_change_on = 0;
					}
				}	break;
				case BTN_TUP_ID:
				{
					if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_present)
					{
						if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode)
						{
							if ((brlt_button_reaction.p_menu_state->sub_state_mode != 
								 BRLT_ON_DEFAULT_TSET_M_ID) ||
							    (brlt_button_reaction.p_menu_state->sub_state_mode != 
								 BRLT_ON_DEFAULT_CONT_TSET_M_ID))
							{
								brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
							}
							else
							{
								if (brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp < 
														BRLT_MAX_TEMP)
								{
									brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp++;
								}
								else
								{
									brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp = BRLT_MAX_TEMP;
								}
							}
							switch_param_change_indication();
							brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
										NULL, NULL, STOP_DEV_POLL);
							brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
										(void *)p_arg, NULL, START_CONT_PARAM_CHANGE_TIMER);
						}
						else
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
						}
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode = 
								BRLT_ON_DEFAULT_TSET_M_ID;
					}
					
				}	break;
				case BTN_TDW_ID:
				{
					if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_present)
					{
						if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode)
						{
							if((brlt_button_reaction.p_menu_state->sub_state_mode != 
								BRLT_ON_DEFAULT_TSET_M_ID) &&
							   (brlt_button_reaction.p_menu_state->sub_state_mode != 
								BRLT_ON_DEFAULT_CONT_TSET_M_ID))
							
							{
								brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
							}
							else
							{
								if (brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp > 
														BRLT_MIN_TEMP)
								{
									brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp--;
								}
								else
								{
									brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp = BRLT_MIN_TEMP;
								}
							}
							switch_param_change_indication();
							brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
								NULL, NULL, STOP_DEV_POLL);
							brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
								(void *)p_arg, NULL, START_CONT_PARAM_CHANGE_TIMER);
						}
						else
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
						}
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode = 
								BRLT_ON_DEFAULT_TSET_M_ID;
					}
				}	break;
				default:
					break;
			}
		}	break;
		case BRLT_ON_SET_M_ID:
		{
			cont_param_change_on = 1;
			*brlt_button_reaction.p_brlt_state_valid = 0;
			switch (button_id)
			{
				case BTN_SPUP_ID:
				case BTN_SPDW_ID:
				{
					if (cont_param_change_on == 1)
					{
						cont_param_change_on = 0;
					}
				}	break;
				case BTN_TUP_ID:
				{
					change_current_menu_set_param(true);
					if (can_be_changed_continuosly(brlt_button_reaction.p_menu_state->sub_state_mode))
					{
						brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
							(void *)&button_id, NULL, START_CONT_PARAM_CHANGE_TIMER);
					}
				}	break;
				case BTN_TDW_ID:
				{
					change_current_menu_set_param(false);
					if (can_be_changed_continuosly(brlt_button_reaction.p_menu_state->sub_state_mode))
					{
						brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
							(void *)&button_id, NULL, START_CONT_PARAM_CHANGE_TIMER);
					}
				}	break;
				default:
					break;
			}
		}	break;
		case BRLT_ON_KIV_SET_M_ID:
		{
						cont_param_change_on = 1;
			*brlt_button_reaction.p_brlt_state_valid = 0;
			switch (button_id)
			{
				case BTN_SPUP_ID:
				case BTN_SPDW_ID:
				{
					if (cont_param_change_on == 1)
					{
						cont_param_change_on = 0;
					}
				}	break;
				case BTN_TUP_ID:
				{
					change_current_menu_kiv_set_param(true);
					if (can_be_changed_continuosly(brlt_button_reaction.p_menu_state->sub_state_mode))
					{
						brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
							(void *)&button_id, NULL, START_CONT_PARAM_CHANGE_TIMER);
					}
				}	break;
				case BTN_TDW_ID:
				{
					change_current_menu_set_param(false);
					if (can_be_changed_continuosly(brlt_button_reaction.p_menu_state->sub_state_mode))
					{
						brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
							(void *)&button_id, NULL, START_CONT_PARAM_CHANGE_TIMER);
					}
				}	break;
				default:
					break;
			}
		}	break;
		default:
			break;
	}
	
	return ret_val;
}

static uint32_t evt_br_menu_single_param_change(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t com_type_id, loc_button_id;

	loc_button_id = *(uint8_t *)p_arg;
	com_type_id = BRLT_SET_PARAMS_ID;

	switch (brlt_button_reaction.p_menu_state->sub_state)
	{
		case BRLT_ON_DEFAULT_M_ID:
		{
			switch (loc_button_id)
			{
				case BTN_SPUP_ID:
				{
					if (cont_param_change_on == 2)
					{
						cont_param_change_on = 0;
					}
					if (brlt_button_reaction.p_brlt_params->dev_info.user_set.fan_speed < 
						brlt_button_reaction.p_brlt_params->dev_info.misc_info.max_fan_speed)
					{
						brlt_button_reaction.p_brlt_params->dev_info.user_set.fan_speed++;
					}
					else
					{
						brlt_button_reaction.p_brlt_params->brlt_invalid_speed = 1;
					}
					
				}	break;
				case BTN_SPDW_ID:
				{
					if (cont_param_change_on == 2)
					{
						cont_param_change_on = 0;
					}
					if	(brlt_button_reaction.p_brlt_params->dev_info.user_set.fan_speed > 1)
					{
						brlt_button_reaction.p_brlt_params->dev_info.user_set.fan_speed--;
					}
					else
					{
						brlt_button_reaction.p_brlt_params->brlt_invalid_speed = 1;
					}
				}	break;
				case BTN_TUP_ID:
				{
					if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_present == 1)
					{
						if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode == 1)
						{
							if((brlt_button_reaction.p_menu_state->sub_state_mode != 
								BRLT_ON_DEFAULT_TSET_M_ID) &&
							   (brlt_button_reaction.p_menu_state->sub_state_mode != 
								BRLT_ON_DEFAULT_CONT_TSET_M_ID))
							{
								brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
							}
							else
							{
								if (cont_param_change_on == 2)
								{
									cont_param_change_on = 0;
								}
								else
								{
									if (brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp < 
														BRLT_MAX_TEMP)
									{
										brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp++;
									}
									else
									{
										brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp = BRLT_MAX_TEMP; 
									}
								}
							}
						}
						else
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
						}
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
					}
				}	break;
				case BTN_TDW_ID:
				{
					if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_present == 1)
					{
						if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode == 1)
						{
							if ((brlt_button_reaction.p_menu_state->sub_state_mode != 
								 BRLT_ON_DEFAULT_TSET_M_ID) &&
							    (brlt_button_reaction.p_menu_state->sub_state_mode != 
								 BRLT_ON_DEFAULT_CONT_TSET_M_ID))
							{
								brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
							}
							else
							{
								if (cont_param_change_on == 2)
								{
									cont_param_change_on = 0;
								}
								else
								{
									if (brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp > 
														BRLT_MIN_TEMP)
									{
										brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp--;
									}
									else
									{
										brlt_button_reaction.p_brlt_params->dev_info.user_set.target_temp = 0;
									}
								}
							}
						}
						else
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
						}
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_DEFAULT_TSET_M_ID;
					}
				}	break;
				default:
					break;
			}
			switch_param_change_indication();
			brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
										NULL, NULL, STOP_DEV_POLL);
			if (cont_param_change_on == 0)
			{
				*brlt_button_reaction.p_brlt_state_valid = 0;
				brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
			}
		}	break;
		case BRLT_ON_SET_M_ID:
		{
			*brlt_button_reaction.p_brlt_state_valid = 0;
			switch (loc_button_id)
			{
				case BTN_SPUP_ID:
				{
					if (cont_param_change_on == 2)
						cont_param_change_on = 0;
					if(check_save_param_condition(loc_button_id))
					{
						send_current_param_value_to_breezer();
						if (brlt_button_reaction.p_menu_state->sub_state_mode >= 
								(BRLT_ON_SET_M_NUM - 1))
						{
							if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_present)
							{
								if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode)
									brlt_button_reaction.p_menu_state->sub_state_mode = 
										BRLT_ON_SET_HEATER_TSET_M_ID;
								else
									brlt_button_reaction.p_menu_state->sub_state_mode = 
										BRLT_ON_SET_HEATER_H_INACTIVE_M_ID;
							}
							else
								brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_SET_HEATER_HOFF_M_ID;
						}
						else 
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
							BRLT_ON_SET_FILTER_TIME_M_ID;
						}
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode++;
					}
				}	break;
				case BTN_SPDW_ID:
				{
					if (cont_param_change_on == 2)
						cont_param_change_on = 0;
					if(check_save_param_condition(loc_button_id))
					{
						send_current_param_value_to_breezer();
						if((brlt_button_reaction.p_menu_state->sub_state_mode == 
							(BRLT_ON_SET_HEATER_TSET_M_ID)) ||
							(brlt_button_reaction.p_menu_state->sub_state_mode == 
							(BRLT_ON_SET_HEATER_HOFF_M_ID)) ||
							(brlt_button_reaction.p_menu_state->sub_state_mode == 
							(BRLT_ON_SET_HEATER_H_INACTIVE_M_ID)))
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
								BRLT_ON_SET_M_NUM - 1;
						}
						else if(brlt_button_reaction.p_menu_state->sub_state_mode == 
							BRLT_ON_SET_FILTER_TIME_M_ID)
						{
							if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_present)
							{
								if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode)
									brlt_button_reaction.p_menu_state->sub_state_mode = 
										BRLT_ON_SET_HEATER_TSET_M_ID;
								else
									brlt_button_reaction.p_menu_state->sub_state_mode = 
										BRLT_ON_SET_HEATER_H_INACTIVE_M_ID;
							}
							else
								brlt_button_reaction.p_menu_state->sub_state_mode = 
									BRLT_ON_SET_HEATER_HOFF_M_ID;
						}
						else
							brlt_button_reaction.p_menu_state->sub_state_mode--;
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode--;
					}
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
		}	break;
		case BRLT_ON_KIV_SET_M_ID:
		{
			*brlt_button_reaction.p_brlt_state_valid = 0;
			switch (loc_button_id)
			{
				case BTN_SPUP_ID:
				{
					if (cont_param_change_on == 2)
						cont_param_change_on = 0;
					if(check_save_param_condition(loc_button_id))
					{
						send_current_param_value_to_breezer();
						if (brlt_button_reaction.p_menu_state->sub_state_mode >= 
								(BRLT_ON_KIV_SET_AUTOKIV_M_NUM - 1))
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
													BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID;
						}
						else 
						{
							brlt_button_reaction.p_menu_state->sub_state_mode++;
						}
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode++;
					}
				}	break;
				case BTN_SPDW_ID:
				{
					if (cont_param_change_on == 2)
						cont_param_change_on = 0;
					if(check_save_param_condition(loc_button_id))
					{
						send_current_param_value_to_breezer();
						if (brlt_button_reaction.p_menu_state->sub_state_mode <= 
								0)
						{
							brlt_button_reaction.p_menu_state->sub_state_mode = 
													BRLT_ON_KIV_SET_AUTOKIV_M_NUM - 1;
						}
						else 
						{
							brlt_button_reaction.p_menu_state->sub_state_mode--;
						}
					}
					else
					{
						brlt_button_reaction.p_menu_state->sub_state_mode--;
					}
				}	break;
				case BTN_TUP_ID:
				{
					change_current_menu_kiv_set_param(true);
				}	break;
				case BTN_TDW_ID:
				{
					change_current_menu_kiv_set_param(false);
				}	break;
				default:
					break;
			}
		} break;
		case BRLT_ON_KIV_M_ID:
		{
			*brlt_button_reaction.p_brlt_state_valid = 0;
			switch (loc_button_id)
			{
				case BTN_SPUP_ID:
				case BTN_SPDW_ID:
				{
					brlt_button_reaction.p_menu_state->sub_state_mode =
								BRLT_ON_KIV_VSET_M_ID;
				}	break;
				case BTN_TUP_ID:
				case BTN_TDW_ID:
				{
					brlt_button_reaction.p_menu_state->sub_state_mode =
								BRLT_ON_KIV_TSET_M_ID;
				}	break;
				default:
					break;
			}
		}	break;
		default:
			break;
	}
	return ret_val;
}

static uint32_t evt_br_menu_switch_snd_state(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BRLT_SET_PARAMS_ID;

	*brlt_button_reaction.p_brlt_state_valid = 0;
	brlt_button_reaction.p_brlt_params->dev_info.flags.snd_mode =
			~brlt_button_reaction.p_brlt_params->dev_info.flags.snd_mode;
	brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);

	return ret_val;
}

static uint32_t evt_br_menu_reset_userset_to_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BRLT_HARD_RESET_ID;

	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
			NULL, NULL, STOP_DEV_POLL);
	brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);

	return ret_val;
}

static uint32_t evt_br_menu_on_kiv_switch_state(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	switch (brlt_button_reaction.p_menu_state->sub_state_mode)
	{
		case BRLT_ON_KIV_TOUT_M_ID:
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_KIV_DEV_REPORT_M_ID;
		}	break;
		case BRLT_ON_KIV_DEV_REPORT_M_ID:
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_KIV_DEV_FW_REPORT_M_ID;
		}	break;
		case BRLT_ON_KIV_DEV_FW_REPORT_M_ID:
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_KIV_TOUT_M_ID;
		}	break;
		default:
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_KIV_TOUT_M_ID;
		}	break;
	}
	return ret_val;
}

 static uint32_t evt_br_menu_on_default_switch_state(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	switch (brlt_button_reaction.p_menu_state->sub_state_mode)
	{
		case BRLT_ON_DEFAULT_HOFF_M_ID:
		case BRLT_ON_DEFAULT_TSET_M_ID:
		case BRLT_ON_DEFAULT_H_INACTIVE_M_ID:
		
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_DEFAULT_DEV_REPORT_M_ID;
		}	break;
		case BRLT_ON_DEFAULT_TOUT_M_ID:
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_DEFAULT_DEV_REPORT_M_ID;
		}	break;
		case BRLT_ON_DEFAULT_DEV_REPORT_M_ID:
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_DEFAULT_DEV_FW_REPORT_M_ID;
		}	break;
		case BRLT_ON_DEFAULT_DEV_FW_REPORT_M_ID:
		default:
		{
			brlt_button_reaction.p_menu_state->sub_state_mode = 
						BRLT_ON_DEFAULT_H_INACTIVE_M_ID;
		}	break;
	}
	return ret_val;
}

static uint32_t evt_br_menu_on_switch_default_to_set(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_SET_M_ID;

	if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_present)
	{
		if (brlt_button_reaction.p_brlt_params->dev_info.flags.heater_mode)
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_SET_HEATER_TSET_M_ID;
		else
			brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_SET_HEATER_H_INACTIVE_M_ID;
	}
	else
		brlt_button_reaction.p_menu_state->sub_state_mode = 
			BRLT_ON_DEFAULT_HOFF_M_ID;

	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_DEV_POLL);
	return ret_val;
}

static uint32_t evt_br_menu_on_switch_set_to_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	send_current_param_value_to_breezer();
	brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_DEFAULT_M_ID;
	brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_DEFAULT_H_INACTIVE_M_ID;

	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, START_DEV_POLL);
	
	return ret_val;
}

static uint32_t evt_br_menu_on_switch_kiv_set_to_kiv(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	send_current_param_value_to_breezer();
	brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_KIV_M_ID;
	brlt_button_reaction.p_menu_state->sub_state_mode = 
				BRLT_ON_KIV_TOUT_M_ID;

	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, START_DEV_POLL);
	
	return ret_val;
}

static uint32_t evt_br_menu_on_switch_default_to_kiv(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_ERROR_INVALID_PARAM;
	static uint8_t com_type_id = BRLT_SET_PARAMS_ID;

	if (brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_present)
	{
		brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_DEV_POLL);		
		brlt_button_reaction.p_menu_state->state = BRLT_MENU_ON_ID;		
		brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_KIV_M_ID;
		brlt_button_reaction.p_menu_state->sub_state_mode = 
													BRLT_ON_KIV_TOUT_M_ID;
		brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_active = 1;			
 
		brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
	}

	return ret_val;
}

static uint32_t evt_br_menu_off_switch_to_kiv(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BRLT_SET_PARAMS_ID;

	*brlt_button_reaction.p_brlt_state_valid = 0;

	if (brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_present)
	{     
		brlt_button_reaction.p_menu_state->state = BRLT_MENU_ON_ID;
		brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_KIV_M_ID;
		brlt_button_reaction.p_menu_state->sub_state_mode = 
													BRLT_ON_KIV_TOUT_M_ID;
		brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_active = 1;

		brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
				NULL, NULL, STOP_DEV_POLL);	
		brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
	} 

	return ret_val;
}

static uint32_t evt_br_menu_on_switch_kiv_to_default(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	static uint8_t com_type_id = BRLT_SET_PARAMS_ID;

	if (brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_present)
	{
		brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_DEV_POLL);

		if (brlt_button_reaction.p_brlt_params->dev_info.flags.power_mode)
		{
			brlt_button_reaction.p_menu_state->state = BRLT_MENU_ON_ID;
			brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_DEFAULT_M_ID;
			brlt_button_reaction.p_menu_state->sub_state_mode = 
													BRLT_ON_SET_HEATER_H_INACTIVE_M_ID;
		} else
		{
			brlt_button_reaction.p_menu_state->state = BRLT_MENU_OFF_ID;
			brlt_button_reaction.p_menu_state->sub_state = BRLT_OFF_DEFAULT_M_ID;
			brlt_button_reaction.p_menu_state->sub_state_mode = 0;
		}

		brlt_button_reaction.p_brlt_params->dev_info.flags.kiv_active = 0;

	brlt_protocol_vptr.input_data((void *)&com_type_id, NULL);
	}
	return ret_val;
}

/*static uint32_t evt_br_menu_on_switch_kiv_to_kiv_set(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	brlt_button_reaction.p_menu_state->sub_state = BRLT_ON_KIV_SET_M_ID;
	brlt_button_reaction.p_menu_state->sub_state_mode = BRLT_ON_KIV_SET_AUTOKIV_SWITCH_M_ID;

	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_DEV_POLL);
	return ret_val;
}*/

static uint32_t evt_br_menu_remote_test(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_DEV_POLL);
	brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, START_SCREEN_TEST);

	return ret_val;
}
/*class methods section*/

static uint32_t init_button_reaction_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	brlt_main_t * p_local_brlt_main;
	
	p_local_brlt_main = (brlt_main_t *)p_context;
	
	brlt_button_reaction.p_brlt_callback = p_local_brlt_main->p_brlt_callback;
	brlt_button_reaction.p_brlt_params = &p_local_brlt_main->brlt_state;
	brlt_button_reaction.p_brc_main	= p_local_brlt_main->p_brc_main;
	brlt_button_reaction.p_brlt_state_valid	= &p_local_brlt_main->brlt_state_valid;
	brlt_button_reaction.p_menu_state = p_local_brlt_main->p_menu_state;
	return ret_val;
}

static uint32_t deinit_button_reaction_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	brlt_button_reaction.p_brlt_params = NULL;
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
	uint8_t * p_context;
	uint8_t button_id;
	button_id = BTN_SPUP_ID;
	
	p_context = (uint8_t *)p_arg;
	if(p_context == NULL)
	{
		if(cont_param_change_on == 1)
		{
			cont_param_change_on = 2;
			switch_param_change_indication();
			brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_CONT_PARAM_CHANGE_TIMER);
		}
	}
	execute_button_evt(&button_id, BUTTON_INT_EVT_FAN_UP_ID);
	return ret_val;
}

static uint32_t evt_fan_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t * p_context;
	uint8_t button_id;
	button_id = BTN_SPDW_ID;
	
	p_context = (uint8_t *)p_arg;
	if(p_context == NULL)
	{
		if(cont_param_change_on == 1)
		{
			cont_param_change_on = 2;
			switch_param_change_indication();
			brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_CONT_PARAM_CHANGE_TIMER);
		}
	}
	execute_button_evt(&button_id, BUTTON_INT_EVT_FAN_DOWN_ID);
	return ret_val;
}

static uint32_t evt_temp_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t * p_context;
	uint8_t button_id;
	button_id = BTN_TUP_ID;
	
	p_context = (uint8_t *)p_arg;
	if(p_context == NULL)
	{
		if(cont_param_change_on == 1)
		{
			cont_param_change_on = 2;
			switch_param_change_indication();
			brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
					NULL, NULL, STOP_CONT_PARAM_CHANGE_TIMER);
		}
	}
	execute_button_evt(&button_id, BUTTON_INT_EVT_TEMP_UP_ID);
	return ret_val;
}

static uint32_t evt_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t * p_context;
	uint8_t button_id;
	button_id = BTN_TDW_ID;
	
	p_context = (uint8_t *)p_arg;
	if(p_context == NULL)
	{
		if(cont_param_change_on == 1)
		{
			cont_param_change_on = 2;
			switch_param_change_indication();
			brlt_button_reaction.p_brlt_callback((void *)&brlt_button_reaction, 
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