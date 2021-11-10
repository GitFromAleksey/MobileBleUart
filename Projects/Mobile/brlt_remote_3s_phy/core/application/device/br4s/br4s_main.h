/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BR4S_MAIN_H
#define __BR4S_MAIN_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "br4s_common_types.h"
#include "br4s_protocol.h"
#include "br4s_button_reaction.h"
#include "br4s_menu.h"
#include "main_common.h"
#include "device_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_br4s_main_t
{
	device_t  					interface;
	br4s_state_t				br4s_state;
	br4s_menu_state_t			* p_menu_state;
	brc_main_t					* p_brc_main;
	device_instance_callback_t	p_br4s_callback;
	uint8_t						br4s_state_valid;
	uint8_t						prev_menu_sub_state_mode;
	uint8_t						expected_resp_type;
	uint8_t						first_resp_recieved;
} br4s_main_t;
/*Exported constants*/
extern const device_m_list_t	br4s_device_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
device_t * br4s_get_interface(void);
ble_adv_data_parser_t br4s_get_p_adv_data(void);
#endif /*__BR4S_MAIN_H*/
/******************************* END_OF_FILE **********************************/