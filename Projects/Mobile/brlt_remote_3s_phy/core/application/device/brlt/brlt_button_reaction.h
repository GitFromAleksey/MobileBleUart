/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRLT_BUTTON_REACTION_H
#define __BRLT_BUTTON_REACTION_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "brlt_menu.h"
#include "main_common.h"
#include "button_reaction_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_brlt_button_reaction_t
{
	brc_main_t					* p_brc_main;
	brlt_state_t  				* p_brlt_params;
	brlt_menu_state_t			* p_menu_state;
	device_instance_callback_t	p_brlt_callback;
	uint8_t						* p_brlt_state_valid;
} brlt_button_reaction_t;
/*Exported constants*/
extern const button_reaction_m_list_t	brlt_button_reaction_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__BRLT_BUTTON_REACTION_H*/
/******************************* END_OF_FILE **********************************/