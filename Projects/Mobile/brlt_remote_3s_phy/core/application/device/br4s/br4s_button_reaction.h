/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BR4S_BUTTON_REACTION_H
#define __BR4S_BUTTON_REACTION_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "br4s_menu.h"
#include "main_common.h"
#include "button_reaction_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_br4s_button_reaction_t
{
	brc_main_t					* p_brc_main;
	br4s_state_t  				* p_br4s_params;
	br4s_menu_state_t			* p_menu_state;
	device_instance_callback_t	p_br4s_callback;
	uint8_t						* p_br4s_state_valid;
	uint8_t						* p_expected_resp_type;
	uint8_t 					button_in_process;
	uint8_t						cont_param_change_on;
} br4s_button_reaction_t;
/*Exported constants*/
extern const button_reaction_m_list_t	br4s_button_reaction_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__BR4S_BUTTON_REACTION_H*/
/******************************* END_OF_FILE **********************************/