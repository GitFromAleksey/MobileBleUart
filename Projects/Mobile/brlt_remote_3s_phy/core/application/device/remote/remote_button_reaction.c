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

/*Exported constants*/
const button_reaction_m_list_t	remote_button_reaction_vptr = {
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
static remote_button_reaction_t remote_button_reaction;
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/

/*Private functions*/
static uint32_t evt_br_menu_bond_erase(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	if(remote_button_reaction.p_brc_main != NULL)
	{
		remote_button_reaction.p_menu_state->state = REMOTE_MENU_BLE_ID;
		remote_button_reaction.p_menu_state->sub_state = REMOTE_BLE_PAIR_ATT_M_ID;
		remote_menu_vptr.execute(NULL, NULL, NULL);
		remote_button_reaction.p_remote_callback((void *)&remote_button_reaction, 
				NULL, NULL, BLE_ERASE_BONDS);
	}
	return ret_val;
}

static uint32_t evt_br_menu_remote_test(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	if(remote_button_reaction.p_brc_main != NULL)
	{
		remote_button_reaction.p_remote_callback((void *)&remote_button_reaction, 
						NULL, NULL, STOP_DEV_POLL);
		remote_button_reaction.p_remote_callback((void *)&remote_button_reaction, 
						NULL, NULL, START_SCREEN_TEST);
	}
	return ret_val;
}
/*class methods section*/

static uint32_t init_button_reaction_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	remote_main_t * p_local_remote_main;

	p_local_remote_main = (remote_main_t *)p_context;
	remote_button_reaction.p_remote_callback = p_local_remote_main->p_remote_callback;
	remote_button_reaction.p_brc_main = p_local_remote_main->p_brc_main;
	remote_button_reaction.p_remote_params = &p_local_remote_main->remote_state;
	remote_button_reaction.p_menu_state = p_local_remote_main->p_menu_state;

	return ret_val;
}

static uint32_t deinit_button_reaction_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	remote_button_reaction.p_remote_callback = NULL;
	remote_button_reaction.p_remote_params = NULL;
	remote_button_reaction.p_brc_main = NULL;
	remote_button_reaction.p_menu_state = NULL;
	
	return ret_val;
}

static uint32_t evt_power_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	return ret_val;
}

static uint32_t evt_state_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_set_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_gate_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_fan_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_fan_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_temp_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_fan_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_fan_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_temp_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_state_set_gate_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_state_set_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_state_set_fan_up_fan_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_set_temp_up_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_temp_up_temp_down_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;

	return ret_val;
}

static uint32_t evt_long_temp_down_power_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	return ret_val;
}

static uint32_t evt_long_state_power_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	evt_br_menu_bond_erase(NULL, NULL);
	return ret_val;
}

static uint32_t evt_long_gate_set_temp_down_fan_up_button(void * p_arg, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	evt_br_menu_remote_test(NULL, NULL);
	return ret_val;
}
/*Public functions*/
/******************************* END_OF_FILE **********************************/