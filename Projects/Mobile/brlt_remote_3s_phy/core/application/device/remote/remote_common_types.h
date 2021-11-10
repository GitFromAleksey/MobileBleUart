/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __REMOTE_COMMON_TYPES_H
#define __REMOTE_COMMON_TYPES_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*Exported defines*/
// clang-format off
#define SWITCH_TO_DEV_TIME	1000 //ms
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef struct _tag_remote_state_t
{
	uint8_t dummy;
	uint8_t screen_test_state;
}remote_state_t;
/*Exported constants*/

/*Exported variables*/

/*Exported functions prototypes*/

#endif /*__REMOTE_COMMON_TYPES_H*/
/******************************* END_OF_FILE **********************************/