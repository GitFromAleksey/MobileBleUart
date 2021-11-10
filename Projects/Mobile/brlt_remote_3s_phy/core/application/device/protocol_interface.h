/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __PROTOCOL_INTERFACE_H
#define __PROTOCOL_INTERFACE_H

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
typedef uint32_t (*init_instance_t)(void * p_arg);
typedef uint32_t (*deinit_instance_t)(void * p_arg);

typedef uint32_t (*input_data_handler_t)(void * p_arg, uint32_t length);
typedef uint32_t (*output_data_handler_t)(void * p_arg, uint32_t length);
typedef uint32_t (*input_raw_data_handler_t)(void * p_arg, uint32_t length);
typedef uint32_t (*output_raw_data_handler_t)(void * p_arg, uint32_t length);

typedef struct _tag_protocol_m_list_t
{
	init_instance_t 			init;
	deinit_instance_t			deinit;
	
	input_data_handler_t		input_data;	/* from pdu to breezer. Creating frame request to protocol */	
	output_data_handler_t		output_data; /* responce from breezer to pdu. Read last frame from breezer request */
	input_raw_data_handler_t	input_raw_data; /* read Raw frame from breezer*/
	output_raw_data_handler_t	output_raw_data; /* Send Raw frame to breezer */
}protocol_m_list_t;

typedef struct _tag_protocol_t
{
	const protocol_m_list_t * vptr;
}protocol_t;

/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/
#endif /*__PROTOCOL_INTERFACE_H*/
/******************************* END_OF_FILE **********************************/