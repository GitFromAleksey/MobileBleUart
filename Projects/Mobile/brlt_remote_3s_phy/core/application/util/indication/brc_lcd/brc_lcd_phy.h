/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRC_LCD_PHY_H
#define __BRC_LCD_PHY_H

/*Includes*/
#include <stdint.h>
#include <string.h>

/*Exported defines*/
// clang-format off

// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
typedef uint32_t (*init_brc_lcd_phy_t)(void * p_context, void * p_target, void * p_source);
typedef uint32_t (*deinit_brc_lcd_phy_t)(void * p_context);

typedef uint32_t (*brc_lcd_handler_t)(void * p_context);

typedef struct _tag_brc_lcd_phy_m_list_t
{
	init_brc_lcd_phy_t 		init;
	deinit_brc_lcd_phy_t	deinit;
	
	brc_lcd_handler_t		convert;
}brc_lcd_phy_m_list_t;

typedef struct _tag_brc_lcd_phy_t
{
	const brc_lcd_phy_m_list_t * vptr;
}brc_lcd_phy_t;
/*Exported constants*/

/*Exported functions prototypes*/
brc_lcd_phy_t * brc_lcd_phy_get_interface(void);
#endif /*__BRC_LCD_PHY_H*/
/******************************* END_OF_FILE **********************************/