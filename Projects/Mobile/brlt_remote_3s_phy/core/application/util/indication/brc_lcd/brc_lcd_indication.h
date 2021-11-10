/*******************************************************************************
*
*
*
*
*******************************************************************************/
/* Define to prevent recursive inclusion */
#ifndef __BRC_LCD_INDICATION_H
#define __BRC_LCD_INDICATION_H

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "indication_interface.h"

/*Exported defines*/
// clang-format off
#define PAIR_STR				"Pair "
#define NO_PAIR_STR				"NPair"

#define CONN_STR				"Conn "
#define NO_CONN_STR				"NConn"

#define SMALL_LINE_LEN			3
#define BIG_LINE_LEN			5
#define HEATER_PIC_LEN			2

#define DEFAULT_BLINK_PATTERN 	0x0F0F
// clang-format on
/*Exported macro*/
// clang-format off

// clang-format on
/*Exported types*/
enum 
{
	SEG_OFF = 0,
	SEG_ON,
	SEG_BLINK,
};

enum 
{
	SPEED_1_ICON_ID = 0,
	SPEED_2_ICON_ID,
	SPEED_3_ICON_ID,
	SPEED_4_ICON_ID,
	SPEED_5_ICON_ID,
	SPEED_6_ICON_ID,
	NUM_OF_SPEED_SEG,
};

enum 
{
	CIRCULATION_ICON_ID = 0,
	OUTSIDE_ICON_ID,
	NUM_OF_GATE_SEG,
};

enum 
{
	HEATER_ICON_ID = 0,
	HEATER_OFF_ICON_ID,
	NUM_OF_HEAT_SEG,
};

enum
{
	SEG_CELSIUM = ('~'+1),
	SEG_A,
	SEG_B,
	SEG_C,
	SEG_D,
	SEG_E,
	SEG_F,
	SEG_G,
};

enum
{
	DEFAULT_ID = 0,
	SMALL_LINE_TEST_ID,
	BIG_LINE_TEST_ID,
	SPEED_LINE_TEST_ID,
	HEATER_PIC_TEST_ID,
	GATE_PIC_TEST_ID,
	MA_ICON_TEST_ID,
	FAN_ICON_TEST_ID,
	BREEZER_ICON_TEST_ID,
	INPUT_ARROW_ICON_TEST_ID,
	OUTPUT_ARROW_ICON_TEST_ID,
	AUTO_ICON_TEST_ID,
	DEGREE_ICON_TEST_ID,
	FILTER_ICON_TEST_ID,
	ALARM_CLOCK_ICON_TEST_ID,

	SCREEN_TEST_NUM,
	END_OF_SCREEN_TEST = 0xFF,
};

typedef struct _tag_icon_state_t 
{
	uint8_t value : 1;
	uint8_t state : 2;
	uint8_t reserved : 5;
} icon_state_t;

typedef struct _tag_seven_seg_state_t
{
	uint8_t value;
	uint8_t state;
} seven_seg_state_t;

typedef struct _tag_blink_state_t
{
	uint16_t 	pattern;	// bit field 0b 0000 0000 max pattern len is 8 
	uint8_t 	state;
	uint8_t		current_state;
} blink_state_t;

typedef struct _tag_brc_lcd_g_ram_t
{
	blink_state_t		blink;
	seven_seg_state_t	small_line[SMALL_LINE_LEN];
	seven_seg_state_t	big_line[BIG_LINE_LEN];
	icon_state_t		speed_line[NUM_OF_SPEED_SEG];
	icon_state_t		heater_pic[HEATER_PIC_LEN];
	icon_state_t		gate_pic[NUM_OF_GATE_SEG];
	icon_state_t		ma_icon;
	icon_state_t		fan_icon;
	icon_state_t		breezer_icon;
	icon_state_t		input_arrow_icon;
	icon_state_t		output_arrow_icon;
	icon_state_t		auto_icon;
	icon_state_t		degree_icon;
	icon_state_t		filter_icon;
	icon_state_t		alarm_clock_icon;
} brc_lcd_g_ram_t;
/*Exported constants*/
extern const indication_m_list_t	brc_lcd_indication_m_list;
/*Exported variables*/

/*Exported functions prototypes*/
brc_lcd_g_ram_t * brc_lcd_indication_get_g_ram(void);
#endif /*__BRC_LCD_INDICATION_H*/
/******************************* END_OF_FILE **********************************/