/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRLT_MAIN_H
#define __BRLT_MAIN_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "brlt_common_types.h"
#include "brlt_protocol.h"
#include "brlt_button_reaction.h"
#include "brlt_menu.h"
#include "main_common.h"
#include "device_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_brlt_main_t
{
	device_t  					interface;
	brlt_state_t				brlt_state;
	brlt_menu_state_t			* p_menu_state;
	brc_main_t					* p_brc_main;
	device_instance_callback_t	p_brlt_callback;
	uint8_t 					brlt_state_valid;
	uint8_t						prev_menu_sub_state_mode;
} brlt_main_t;
/*Exported constants*/
extern const device_m_list_t	brlt_device_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
device_t * brlt_get_interface(void);
ble_adv_data_parser_t brlt_get_p_adv_data(void);
#endif /*__BRLT_MAIN_H*/
/******************************* END_OF_FILE **********************************/