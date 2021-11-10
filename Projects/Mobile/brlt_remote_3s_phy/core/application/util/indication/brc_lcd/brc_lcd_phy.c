/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "brc_lcd_phy.h"
#include "brc_lcd_indication.h"
#include "brc_lcd_ht16xx_driver.h"
#include "nrf_error.h"

/*Private defines*/
// clang-format off

// clang-format on
/*Private types*/
enum
{
	ALARM_CLOCK_ID = 0,
	FILTER_ID,
	HEAT_ID,
	HEAT_OFF_ID,
	FAN_ID,
	BREEZER_ID,
	LEFT_ARROW_ID,
	RIGHT_ARROW_ID,
	POINT_ID,
	CIRCULATION_ID,
	OUTSIDE_ID,
	MA_ID,
	AUTO_ID,
	SPEED_1_ID,
	SPEED_2_ID,
	SPEED_3_ID,
	SPEED_4_ID,
	SPEED_5_ID,
	SPEED_6_ID,
	NUM_OF_SEGMENTS,
};

typedef void (*lcd_seg_handler_t) (uint8_t state);

typedef struct _tag_brc_lcd_data_t
{
	uint8_t			g_ram_busy;
	uint8_t			* p_raw;
	brc_lcd_g_ram_t * p_g_ram;
}brc_lcd_data_t;

typedef struct _tag_brc_lcd_state_t
{
	brc_lcd_phy_t	interface;
	brc_lcd_data_t	data;
}brc_lcd_state_t;
/*Private functions prototypes*/
static uint32_t init_converter(void * p_context, 
							   void * p_target, void * p_source);
static uint32_t deinit_converter(void * p_context);
static uint32_t convert_g_ram_into_raw(void * p_context);

static void lcd_set_icon_state (icon_state_t * p_icon, uint8_t blink_state, uint8_t icon_id);

static void lcd_seg_alarm_clock (uint8_t state);
static void lcd_seg_filter (uint8_t state);
static void lcd_seg_heat (uint8_t state);
static void lcd_seg_heat_off (uint8_t state);
static void lcd_seg_fan (uint8_t state);
static void lcd_seg_breezer_icon (uint8_t state);
static void lcd_seg_left_arrow (uint8_t state);
static void lcd_seg_right_arrow (uint8_t state);
static void lcd_seg_point (uint8_t state);
static void lcd_seg_circulation (uint8_t state);
static void lcd_seg_outside (uint8_t state);
static void lcd_seg_ma (uint8_t state);
static void lcd_seg_auto (uint8_t state);
static void lcd_seg_speed_1 (uint8_t state);
static void lcd_seg_speed_2 (uint8_t state);
static void lcd_seg_speed_3 (uint8_t state);
static void lcd_seg_speed_4 (uint8_t state);
static void lcd_seg_speed_5 (uint8_t state);
static void lcd_seg_speed_6 (uint8_t state);
/*Exported constants*/
const brc_lcd_phy_m_list_t brc_lcd_phy_vptr = {
	.init 		= &init_converter,
	.deinit		= &deinit_converter,
	.convert	= &convert_g_ram_into_raw,
};
/*Private variables*/
static brc_lcd_state_t brc_lcd_state = {.interface.vptr = &brc_lcd_phy_vptr};
static const lcd_seg_handler_t lcd_seg_handler[] = 
{
	&lcd_seg_alarm_clock,
	&lcd_seg_filter,
	&lcd_seg_heat,
	&lcd_seg_heat_off,
	&lcd_seg_fan,
	&lcd_seg_breezer_icon,
	&lcd_seg_left_arrow,
	&lcd_seg_right_arrow,
	&lcd_seg_point,
	&lcd_seg_circulation,
	&lcd_seg_outside,
	&lcd_seg_ma,
	&lcd_seg_auto,
	&lcd_seg_speed_1,
	&lcd_seg_speed_2,
	&lcd_seg_speed_3,
	&lcd_seg_speed_4,
	&lcd_seg_speed_5,
	&lcd_seg_speed_6,
};
/*Private macro*/
// clang-format off
#define LCD_ON_SEG(a,b) a |= b
#define LCD_OFF_SEG(a,b) a &= (~b)

#define SEG(a) (23-a)    	//Convert_pin_number_in_SEG_number
#define COM(a) (1<<(8 - a))	//Convert_com_number_in_COM_bit_number

#define LCD_SEG_DIGIT_SET_A(NUMBER)				(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(4)) )
#define LCD_SEG_DIGIT_SET_B(NUMBER)				(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(3)) )
#define LCD_SEG_DIGIT_SET_C(NUMBER)				(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(2)) )
#define LCD_SEG_DIGIT_SET_D(NUMBER)				(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(1)) )
#define LCD_SEG_DIGIT_SET_E(NUMBER)				(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((18-2*NUMBER)))),  COM(2)) )
#define LCD_SEG_DIGIT_SET_F(NUMBER)				(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((18-2*NUMBER)))),  COM(4)) )
#define LCD_SEG_DIGIT_SET_G(NUMBER)				(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((18-2*NUMBER)))),  COM(3)) )

#define LCD_SEG_DIGIT_CLEAR_A(NUMBER)			(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(4)) )
#define LCD_SEG_DIGIT_CLEAR_B(NUMBER)			(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(3)) )
#define LCD_SEG_DIGIT_CLEAR_C(NUMBER)			(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(2)) )
#define LCD_SEG_DIGIT_CLEAR_D(NUMBER)			(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((17-2*NUMBER)))),  COM(1)) )
#define LCD_SEG_DIGIT_CLEAR_E(NUMBER)			(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((18-2*NUMBER)))),  COM(2)) )
#define LCD_SEG_DIGIT_CLEAR_F(NUMBER)			(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((18-2*NUMBER)))),  COM(4)) )
#define LCD_SEG_DIGIT_CLEAR_G(NUMBER)			(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((18-2*NUMBER)))),  COM(3)) )

#define LCD_SEG_LITTLE_DIGIT_SET_A(NUMBER)		(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(1)) )
#define LCD_SEG_LITTLE_DIGIT_SET_B(NUMBER)		(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_SET_C(NUMBER)		(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(3)) )
#define LCD_SEG_LITTLE_DIGIT_SET_D(NUMBER)		(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_SET_E(NUMBER)		(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((15+2*NUMBER)))),  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_SET_F(NUMBER)		(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((15+2*NUMBER)))),  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_SET_G(NUMBER)		(LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG((15+2*NUMBER)))),  COM(3)) )

#define LCD_SEG_LITTLE_DIGIT_CLEAR_A(NUMBER)	(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(1)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_B(NUMBER)	(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_C(NUMBER)	(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(3)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_D(NUMBER)	(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((16+2*NUMBER)))),  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_E(NUMBER)	(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((15+2*NUMBER)))),  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_F(NUMBER)	(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((15+2*NUMBER)))),  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_G(NUMBER)	(LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG((15+2*NUMBER)))),  COM(3)) )

#define LCD_ALL_SEG_CLEAR()           			(memset(brc_lcd_state.data.p_raw,0x00,LCD_DATA_SIZE))

// clang-format on
/*Private constants*/ 
//0b x x x x x x x x
//   X G F E D C B A
static const uint8_t digit_code[]=
{	0x00, 0x00, 0x40, 0x00, 0x00, //+ , - . /
	0x3F, 0x06, 0x5B, 0x4F, 0x66, //0 1 2 3 4
	0x6D, 0x7D, 0x07, 0x7F, 0x6F, //5 6 7 8 9
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //: ; < = > ?
	0x00, 0x77, 0x7F, 0x39, 0x5E, //@ A B C D
	0x79, 0x71, 0x3D, 0x76, 0x30, //E F G H I
	0x1E, 0x00, 0x38, 0x00, 0x37, //J K L M N
	0x3F, 0x73, 0x00, 0x33, 0x6D, //O P Q R S
	0x78, 0x3E, 0x3E, 0x00, 0x00, //T U V W X
	0x6E, 0x5B, 0x39, 0x00, 0x0F, //Y Z [ \ ]
	0x00, 0x08, 0x02, 0x5F, 0x7C, //^ _ ' a b
	0x61, 0x5E, 0x79, 0x71, 0x5F, //c d e f g
	0x74, 0x04, 0x0C, 0x00, 0x30, //h i j k l
	0x00, 0x54, 0x5C, 0x73, 0x67, //m n o p q
	0x50, 0x6D, 0x78, 0x1C, 0x1C, //r s t u v
	0x00, 0x00, 0x6E, 0x5B, 0x00, //w x y z {
	0x00, 0x00, 0x40, 			  //| } ~
	0x61, 0x01, 0x03, 0x07, 0x0F, //SEG_CELSIUM, SEG_A, SEG_B, SEG_C, SEG_D,
	0x1f, 0x3F, 0x7F, 			  //SEG_E, SEG_F, SEG_G
	0x00,  	   //OFF
};

/*Private functions*/
static void lcd_seg_alarm_clock (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(1));
}

static void lcd_seg_filter (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(17))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(17))), COM(1));
}

static void lcd_seg_heat (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(21))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(21))), COM(1));
}

static void lcd_seg_heat_off (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(19))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(19))), COM(1));
}

static void lcd_seg_breezer_icon (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(3));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(3));
}

static void lcd_seg_left_arrow (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(2));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(2));
}

static void lcd_seg_right_arrow (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(4));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(23))), COM(4));
}

static void lcd_seg_point (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(10))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(10))), COM(1));
}

static void lcd_seg_fan (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(16))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(16))), COM(1));
}

static void lcd_seg_circulation (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(5))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(5))), COM(1));
}

static void lcd_seg_outside (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(1));
}

static void lcd_seg_ma (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(5))), COM(2));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(5))), COM(2));
}

static void lcd_seg_auto (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(5))), COM(3));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(5))), COM(3));
}

static void lcd_seg_speed_1 (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(14))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(14))), COM(1));
}

static void lcd_seg_speed_2 (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(12))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(12))), COM(1));
}

static void lcd_seg_speed_3 (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(8))), COM(1));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(8))), COM(1));
}

static void lcd_seg_speed_4 (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(2));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(2));
}

static void lcd_seg_speed_5 (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(3));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(3));
}

static void lcd_seg_speed_6 (uint8_t state)
{
	if(state)
		LCD_ON_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(4));
	else
		LCD_OFF_SEG((*(brc_lcd_state.data.p_raw + SEG(6))), COM(4));
}

static void lcd_set_digit_value (uint8_t number_of_digit, uint8_t value)
{
	static uint8_t buff;
	if (value >= 0x2B)
		buff  = digit_code[(value-0x2B)];
	else
		buff = 0x00;
	
	(buff&0x01) ? LCD_SEG_DIGIT_SET_A(number_of_digit) : LCD_SEG_DIGIT_CLEAR_A(number_of_digit);
	(buff&0x02) ? LCD_SEG_DIGIT_SET_B(number_of_digit) : LCD_SEG_DIGIT_CLEAR_B(number_of_digit);
	(buff&0x04) ? LCD_SEG_DIGIT_SET_C(number_of_digit) : LCD_SEG_DIGIT_CLEAR_C(number_of_digit);
	(buff&0x08) ? LCD_SEG_DIGIT_SET_D(number_of_digit) : LCD_SEG_DIGIT_CLEAR_D(number_of_digit);
	(buff&0x10) ? LCD_SEG_DIGIT_SET_E(number_of_digit) : LCD_SEG_DIGIT_CLEAR_E(number_of_digit);
	(buff&0x20) ? LCD_SEG_DIGIT_SET_F(number_of_digit) : LCD_SEG_DIGIT_CLEAR_F(number_of_digit);
	(buff&0x40) ? LCD_SEG_DIGIT_SET_G(number_of_digit) : LCD_SEG_DIGIT_CLEAR_G(number_of_digit);
}

static void lcd_set_little_digit_value (uint8_t number_of_digit, uint8_t value)
{
	static uint8_t buff;
	if (value >= 0x2B)
		buff  = digit_code[(value-0x2B)];
	else
		buff = 0x00;
	
	(buff&0x01) ? LCD_SEG_LITTLE_DIGIT_SET_A(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_A(number_of_digit);
	(buff&0x02) ? LCD_SEG_LITTLE_DIGIT_SET_B(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_B(number_of_digit);
	(buff&0x04) ? LCD_SEG_LITTLE_DIGIT_SET_C(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_C(number_of_digit);
	(buff&0x08) ? LCD_SEG_LITTLE_DIGIT_SET_D(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_D(number_of_digit);
	(buff&0x10) ? LCD_SEG_LITTLE_DIGIT_SET_E(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_E(number_of_digit);
	(buff&0x20) ? LCD_SEG_LITTLE_DIGIT_SET_F(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_F(number_of_digit);
	(buff&0x40) ? LCD_SEG_LITTLE_DIGIT_SET_G(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_G(number_of_digit);
}

static void lcd_set_icon_state (icon_state_t * p_icon, uint8_t blink_state, uint8_t icon_id)
{
	if(blink_state)
	{ 
		if(p_icon->state != OFF)
		{
			if(p_icon->value)
				lcd_seg_handler[icon_id](SEG_ON);
			else
				lcd_seg_handler[icon_id](SEG_OFF);
		}
		else
			lcd_seg_handler[icon_id](SEG_OFF);
	}
	else
	{
		if(p_icon->state == ON)
		{
			if(p_icon->value)
				lcd_seg_handler[icon_id](SEG_ON);
			else
				lcd_seg_handler[icon_id](SEG_OFF);
		}
		else
			lcd_seg_handler[icon_id](SEG_OFF);
	}
}

/*class methods section*/
static uint32_t init_converter(void * p_context, void * p_target, void * p_source)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	brc_lcd_state.data.p_raw = (uint8_t *)p_target;
	brc_lcd_state.data.p_g_ram = (brc_lcd_g_ram_t *)p_source;
	
	return ret_val;
}

static uint32_t deinit_converter(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	brc_lcd_state.data.p_raw = NULL;
	brc_lcd_state.data.p_g_ram = NULL;
	
	return ret_val;	
}

static uint32_t convert_g_ram_into_raw(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t cnt = 0;
	uint8_t buff = 0;
	
	if ((brc_lcd_state.data.p_raw == NULL) ||
		(brc_lcd_state.data.p_g_ram == NULL))
	{
		ret_val = NRF_ERROR_NOT_FOUND;
		return ret_val;	
	}
	for(cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		if(brc_lcd_state.data.p_g_ram->blink.current_state)
		{ 
		   if(brc_lcd_state.data.p_g_ram->big_line[cnt].state != SEG_OFF)
			   buff = brc_lcd_state.data.p_g_ram->big_line[cnt].value;
		   else
			   buff = 0x00;
		}
		else
		{
			if(brc_lcd_state.data.p_g_ram->big_line[cnt].state == SEG_ON)
				buff = brc_lcd_state.data.p_g_ram->big_line[cnt].value;
		   	else
				buff = 0x00;
		}
		lcd_set_digit_value(cnt+1, buff);
	}
			
	for(cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		if(brc_lcd_state.data.p_g_ram->blink.current_state)
		{ 
		   if(brc_lcd_state.data.p_g_ram->small_line[cnt].state != SEG_OFF)
			   buff = brc_lcd_state.data.p_g_ram->small_line[cnt].value;
		   else
			   buff = 0x00;
		}
		else
		{
			if(brc_lcd_state.data.p_g_ram->small_line[cnt].state == SEG_ON)
				buff = brc_lcd_state.data.p_g_ram->small_line[cnt].value;
		   	else
				buff = 0x00;
		}
		lcd_set_little_digit_value(cnt+1, buff);
	}		

	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->alarm_clock_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, ALARM_CLOCK_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->filter_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, FILTER_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->heater_pic[HEATER_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, HEAT_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->heater_pic[HEATER_OFF_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, HEAT_OFF_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->fan_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, FAN_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->breezer_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, BREEZER_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->input_arrow_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, LEFT_ARROW_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->output_arrow_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, RIGHT_ARROW_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->degree_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, POINT_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->gate_pic[CIRCULATION_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, CIRCULATION_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->gate_pic[OUTSIDE_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, OUTSIDE_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->ma_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, MA_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->auto_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, AUTO_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_1_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_1_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_2_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_2_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_3_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_3_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_4_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_4_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_5_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_5_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_6_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_6_ID);
		
	return ret_val;	
}

static uint32_t create_test_raw_g_ram(void * p_context, uint32_t seg_cnt)
{
	uint32_t ret_val = NRF_SUCCESS;

	uint8_t cnt = 0;
	uint8_t buff = 0;
	
	if ((brc_lcd_state.data.p_raw == NULL) ||
		(brc_lcd_state.data.p_g_ram == NULL))
	{
		ret_val = NRF_ERROR_NOT_FOUND;
		return ret_val;	
	}
	for(cnt = 0; cnt < BIG_LINE_LEN; cnt++)
	{
		if(brc_lcd_state.data.p_g_ram->blink.current_state)
		{ 
		   if(brc_lcd_state.data.p_g_ram->big_line[cnt].state != SEG_OFF)
			   buff = brc_lcd_state.data.p_g_ram->big_line[cnt].value;
		   else
			   buff = 0x00;
		}
		else
		{
			if(brc_lcd_state.data.p_g_ram->big_line[cnt].state == SEG_ON)
				buff = brc_lcd_state.data.p_g_ram->big_line[cnt].value;
		   	else
				buff = 0x00;
		}
		lcd_set_digit_value(cnt+1, buff);
	}
			
	for(cnt = 0; cnt < SMALL_LINE_LEN; cnt++)
	{
		if(brc_lcd_state.data.p_g_ram->blink.current_state)
		{ 
		   if(brc_lcd_state.data.p_g_ram->small_line[cnt].state != SEG_OFF)
			   buff = brc_lcd_state.data.p_g_ram->small_line[cnt].value;
		   else
			   buff = 0x00;
		}
		else
		{
			if(brc_lcd_state.data.p_g_ram->small_line[cnt].state == SEG_ON)
				buff = brc_lcd_state.data.p_g_ram->small_line[cnt].value;
		   	else
				buff = 0x00;
		}
		lcd_set_little_digit_value(cnt+1, buff);
	}		

	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->alarm_clock_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, ALARM_CLOCK_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->filter_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, FILTER_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->heater_pic[HEATER_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, HEAT_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->heater_pic[HEATER_OFF_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, HEAT_OFF_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->fan_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, FAN_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->breezer_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, BREEZER_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->input_arrow_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, LEFT_ARROW_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->output_arrow_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, RIGHT_ARROW_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->degree_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, POINT_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->gate_pic[CIRCULATION_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, CIRCULATION_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->gate_pic[OUTSIDE_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, OUTSIDE_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->ma_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, MA_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->auto_icon, 
		brc_lcd_state.data.p_g_ram->blink.current_state, AUTO_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_1_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_1_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_2_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_2_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_3_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_3_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_4_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_4_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_5_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_5_ID);
	lcd_set_icon_state(&brc_lcd_state.data.p_g_ram->speed_line[SPEED_6_ICON_ID], 
		brc_lcd_state.data.p_g_ram->blink.current_state, SPEED_6_ID);

	return ret_val;	
}
/*Public functions*/
brc_lcd_phy_t * brc_lcd_phy_get_interface(void)
{
	return &brc_lcd_state.interface;
}
/******************************* END_OF_FILE **********************************/