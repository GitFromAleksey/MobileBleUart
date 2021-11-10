/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRC_LCD_HT16XX_DRIVER_H
#define __BRC_LCD_HT16XX_DRIVER_H

/*Includes*/
#include <stdint.h>
#include <string.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
/*Exported defines*/
// clang-format off
#define SEG_QUANTITY 19
#define LCD_DATA_SIZE SEG_QUANTITY

//*****************************************************************
//HT1622 PINS
//*****************************************************************
#define HT1622_CS_PIN						13
#define HT1622_RD_PIN						12
#define HT1622_WR_PIN						11
#define HT1622_DATA_PIN						10
#define HT1622_IRQ_PIN						9
//*****************************************************************
//COMMANDS
//*****************************************************************
#define SYS_DIS 					0x00
#define SYS_EN						0x01
#define LCD_OFF						0x02
#define LCD_ON						0x03
#define TEST_MODE					0xE0
#define NORMAL_MODE					0xE3
#define SET_1_2_BIAS				0x28 //Work only for HT1621
#define SET_1_3_BIAS				0x29 //Work only for HT1621
//*****************************************************************
#define NOTHING 2
#define ON 1
#define OFF 0
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef uint32_t (*init_lcd_driver_instance_t)(void * p_context, void * p_raw);
typedef uint32_t (*deinit_lcd_driver_instance_t)(void * p_context);
typedef uint32_t (*lcd_driver_event_t)(void * p_context, void * p_arg, uint32_t length);

typedef struct _tag_lcd_driver_m_list_t
{
	init_lcd_driver_instance_t 		init;
	deinit_lcd_driver_instance_t	deinit;
	
	lcd_driver_event_t				write;
}lcd_driver_m_list_t;

typedef struct _tag_lcd_driver_t
{
	const lcd_driver_m_list_t * vptr;
} lcd_driver_t;

/*Exported constants*/
extern const lcd_driver_m_list_t	lcd_driver_vptr;
/*Exported functions prototypes*/
lcd_driver_t * brc_lcd_ht16xx_driver_get_interface(void);
#endif /*__BRC_LCD_HT16XX_DRIVER_H*/
/******************************* END_OF_FILE **********************************/