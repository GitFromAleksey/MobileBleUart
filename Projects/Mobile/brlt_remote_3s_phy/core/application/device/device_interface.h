/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __DEVICE_INTERFACE_H
#define __DEVICE_INTERFACE_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "protocol_interface.h"
#include "button_reaction_interface.h"
#include "menu_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef uint32_t (*device_instance_callback_t)(void * p_context,
								void * p_arg, uint32_t length, uint32_t evt);

typedef uint32_t (*init_device_instance_t)(void * p_context, 
		void * p_arg, uint32_t length, void * p_dev_callback);
typedef uint32_t (*deinit_device_instance_t)(void * p_arg);
typedef uint32_t (*device_event_instance_t)(void * p_context,
						void * p_arg, uint32_t length, 
						uint32_t evt_type);

typedef struct _tag_device_m_list_t
{
	init_device_instance_t 			init;
	deinit_device_instance_t		deinit;
	device_event_instance_t			execute;
	device_event_instance_t			event;
}device_m_list_t;

typedef struct _tag_device_t
{
	const device_m_list_t * vptr_dev;
	const protocol_m_list_t * vptr_protocol;
	const button_reaction_m_list_t * vptr_button_reaction;
	const menu_m_list_t * vptr_menu;
}device_t;

/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__DEVICE_INTERFACE_H*/
/******************************* END_OF_FILE **********************************/