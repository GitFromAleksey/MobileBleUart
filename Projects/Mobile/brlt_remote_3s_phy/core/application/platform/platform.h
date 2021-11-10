/*******************************************************************************
* @file    platform.h
* @author  Sorokin N.N
* @date    28-06-2018
* @brief
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __PLATFORM_H
#define __PLATFORM_H

/*Includes*/

/*Exported defines*/
// clang-format off
#define __BRLT_REMOTE_3S_PHY

#define REM_TX_PIN_NUMBER						23
#define REM_RX_PIN_NUMBER						22

#define REM_RTS_PIN_NUMBER						UART_PIN_DISCONNECTED
#define REM_CTS_PIN_NUMBER						UART_PIN_DISCONNECTED

#define LCD_DRV_IRQ							9
#define LCD_DRV_CS							13
#define LCD_DRV_RD							12
#define LCD_DRV_WR							11
#define LCD_DRV_DATA						10

#define REM_BUTTON_SPUP_PIN						6
#define REM_BUTTON_SPDW_PIN						1
#define REM_BUTTON_TUP_PIN						4
#define REM_BUTTON_TDW_PIN						3
#define REM_BUTTON_POWER_PIN					16
#define REM_BUTTON_DISP_PIN						7
#define REM_BUTTON_MENU_PIN						5
#define REM_BUTTON_FLOW_PIN						2

#define INPUTS_NUMBER						2
#define OUTPUTS_NUMBER						3
// clang-format on
/*Exported macro*/
// clang-format off
#define INPUTS_LIST			\
{							\
	REM_BUTTON_SPUP_PIN,		\
	REM_BUTTON_SPDW_PIN,		\
	REM_BUTTON_TUP_PIN,			\
	REM_BUTTON_TDW_PIN,			\
	REM_BUTTON_POWER_PIN,		\
	REM_BUTTON_DISP_PIN,		\
	REM_BUTTON_MENU_PIN,		\
	REM_BUTTON_FLOW_PIN,		\
}
/*#define OUTPUTS_LIST						\
{											\
}*/
// clang-format on
/*Exported types*/

/*Exported constants*/

/*Exported functions prototypes*/

#endif /*__BRLT_REMOTE_3S_PHY*/
/******************************* END_OF_FILE **********************************/