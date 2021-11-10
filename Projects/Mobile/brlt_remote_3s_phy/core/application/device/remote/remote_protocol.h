/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __REMOTE_PROTOCOL_H
#define __REMOTE_PROTOCOL_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main_common.h"
#include "core_ble.h"
#include "protocol_interface.h"
/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/

/*Exported constants*/
extern const protocol_m_list_t	remote_protocol_vptr;
/*Exported variables*/

/*Exported functions prototypes*/
ble_adv_data_parser_t remote_get_adv_parser (void);
#endif /*__REMOTE_PROTOCOL_H*/
/******************************* END_OF_FILE **********************************/