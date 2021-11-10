/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __REMOTE_BUTTON_REACTION_H
#define __REMOTE_BUTTON_REACTION_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "remote_menu.h"
#include "main_common.h"
#include "button_reaction_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_remote_button_reaction_t
{
	remote_state_t  			* p_remote_params;
	brc_main_t					* p_brc_main;
	remote_menu_state_t			* p_menu_state;
	device_instance_callback_t	p_remote_callback;
} remote_button_reaction_t;
/*Exported constants*/
extern const button_reaction_m_list_t	remote_button_reaction_vptr;
/*Exported variables*/

/*Exported functions prototypes*/

#endif /*__REMOTE_BUTTON_REACTION_H*/
/******************************* END_OF_FILE **********************************/