/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __REMOTE_MAIN_H
#define __REMOTE_MAIN_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "remote_common_types.h"
#include "remote_protocol.h"
#include "remote_button_reaction.h"
#include "remote_menu.h"
#include "main_common.h"
#include "device_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_remote_main_t
{
	device_t  					interface;
	remote_state_t				remote_state;
	remote_menu_state_t			* p_menu_state;
	brc_main_t					* p_brc_main;
	device_instance_callback_t	p_remote_callback;
} remote_main_t;
/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/
device_t * remote_get_interface(void);
ble_adv_data_parser_t remote_get_p_adv_data(void);
#endif /*__REMOTE_MAIN_H*/
/******************************* END_OF_FILE **********************************/