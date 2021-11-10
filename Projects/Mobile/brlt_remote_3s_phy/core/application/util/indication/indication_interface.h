/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __INDICATION_INTERFACE_H
#define __INDICATION_INTERFACE_H

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
typedef uint32_t (*init_indication_instance_t)(void * p_context, 
										 void * p_arg, uint32_t length);
typedef uint32_t (*deinit_indication_instance_t)(void * p_arg);

typedef uint32_t (*indication_event_t)(void * p_arg, uint32_t length);

typedef struct _tag_indication_m_list_t
{
	init_indication_instance_t 		init;
	deinit_indication_instance_t	deinit;
	
	indication_event_t				write;
	indication_event_t				execute;
}indication_m_list_t;

typedef struct _tag_indication_t
{
	const indication_m_list_t * vptr;
} indication_t;

/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__INDICATION_INTERFACE_H*/
/******************************* END_OF_FILE **********************************/