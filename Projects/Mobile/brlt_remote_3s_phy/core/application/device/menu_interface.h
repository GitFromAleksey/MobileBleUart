/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __MENU_INTERFACE_H
#define __MENU_INTERFACE_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef uint32_t (*init_menu_instance_t)(void * p_context, 
						void * p_arg, uint32_t length);
typedef uint32_t (*deinit_menu_instance_t)(void * p_context);

typedef uint32_t (*menu_event_t)(void * p_context, 
						void * p_arg, uint32_t length);

typedef struct _tag_menu_m_list_t
{
	init_menu_instance_t 		init;
	deinit_menu_instance_t		deinit;
	menu_event_t				execute;
}menu_m_list_t;

typedef struct _tag_menu_t
{
	const menu_m_list_t * vptr;
}menu_t;

typedef uint32_t (*set_indication_t)(void * p_arg, uint32_t length);
/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__MENU_INTERFACE_H*/
/******************************* END_OF_FILE **********************************/