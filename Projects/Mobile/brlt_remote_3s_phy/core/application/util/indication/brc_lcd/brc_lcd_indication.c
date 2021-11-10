/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "brc_lcd_indication.h"
#include "brc_lcd_ht16xx_driver.h"
#include "brc_lcd_phy.h"

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

/*Private defines*/
// clang-format off
#define TIME_SLOT 100
// clang-format on
/*Private types*/
typedef struct _tag_brc_lcd_indication_t
{	
	uint8_t				g_ram_busy;
	brc_lcd_g_ram_t		g_ram;
	uint8_t 			g_ram_raw[LCD_DATA_SIZE];
} brc_lcd_indication_t;
uint32_t local_screen_upd_cnt = 0;
uint32_t local_screen_blink_on_cnt = 0;
uint32_t local_screen_blink_off_cnt = 0;

/*Private functions prototypes*/
static void indication_timeslot_timer_id_handler (void * p_context);

/*class methods section*/
static uint32_t init_indication(void * p_context, 
										 void * p_arg, uint32_t length);
static uint32_t deinit_indication(void * p_context);

static uint32_t write_indication(void * p_arg, uint32_t length);
static uint32_t execute_indication(void * p_arg, uint32_t length);

/*Exported constants*/
const indication_m_list_t brc_lcd_indication_m_list = {
	.init 		= &init_indication,
	.deinit 	= &deinit_indication,
	.write 		= &write_indication,
	.execute 	= &execute_indication,
};

/*Private variables*/
APP_TIMER_DEF(indication_timeslot_timer_id);

static brc_lcd_phy_t * p_brc_lcd_phy;
static lcd_driver_t * p_lcd_driver;

static brc_lcd_indication_t brc_lcd_indication = {
	.g_ram	= {0},
	.g_ram_raw = {0},
	.g_ram_busy = 0,
};

static uint8_t b_local_current_pattern_num = 0;
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/ 

/*Private functions*/
static void indication_timeslot_timer_id_handler (void * p_context)
{
	brc_lcd_indication.g_ram_busy = 1;
	if (!(local_screen_upd_cnt%5))
	{
		if (brc_lcd_indication.g_ram.blink.state == SEG_ON)
		{
			if(brc_lcd_indication.g_ram.blink.current_state == 0)
			{
				brc_lcd_indication.g_ram.blink.current_state = 1;
				local_screen_blink_on_cnt++;
			}
			else
			{
				brc_lcd_indication.g_ram.blink.current_state = 0;
				local_screen_blink_off_cnt++;
			}
		}
		else
			brc_lcd_indication.g_ram.blink.current_state = 1;
	}
	p_brc_lcd_phy->vptr->convert((void *)p_brc_lcd_phy);
	p_lcd_driver->vptr->write((void *)p_lcd_driver, NULL, NULL);
	local_screen_upd_cnt++;
	brc_lcd_indication.g_ram_busy = 0;
}

/*class methods section*/
static uint32_t init_indication(void * p_context, 
										 void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;

	app_timer_create(&indication_timeslot_timer_id,
					APP_TIMER_MODE_REPEATED,
					indication_timeslot_timer_id_handler);
	p_brc_lcd_phy = brc_lcd_phy_get_interface();
	p_lcd_driver = brc_lcd_ht16xx_driver_get_interface();
	p_lcd_driver->vptr->init((void *) p_lcd_driver, 
							 (void *) &brc_lcd_indication.g_ram_raw);
	p_brc_lcd_phy->vptr->init((void *) p_brc_lcd_phy, 
							  (void *) &brc_lcd_indication.g_ram_raw,
							  (void *) &brc_lcd_indication.g_ram);
	
	memset(&brc_lcd_indication.g_ram, 0x00, sizeof(brc_lcd_g_ram_t));
	memset(brc_lcd_indication.g_ram_raw, 0x00, LCD_DATA_SIZE);
	
	brc_lcd_indication.g_ram.blink.pattern = DEFAULT_BLINK_PATTERN;
	p_brc_lcd_phy->vptr->convert((void *)p_brc_lcd_phy);
	p_lcd_driver->vptr->write((void *)p_lcd_driver, NULL, NULL);
	
	brc_lcd_indication.g_ram_busy = 0;
	
	ret_val = app_timer_stop(indication_timeslot_timer_id);
	APP_ERROR_CHECK(ret_val);

	ret_val = app_timer_start(indication_timeslot_timer_id,
							APP_TIMER_TICKS(TIME_SLOT,0),
							(void *) &brc_lcd_indication.g_ram);
	APP_ERROR_CHECK(ret_val);

	return ret_val;
}

static uint32_t deinit_indication(void * p_context)
{
	uint32_t ret_val = NRF_SUCCESS;

	ret_val = app_timer_stop(indication_timeslot_timer_id);
	APP_ERROR_CHECK(ret_val);

	p_lcd_driver->vptr->deinit((void *)p_lcd_driver);
	p_brc_lcd_phy->vptr->deinit((void *)p_brc_lcd_phy);

	p_lcd_driver = NULL;
	p_brc_lcd_phy = NULL;

	memset(&brc_lcd_indication.g_ram, 0x00, sizeof(brc_lcd_g_ram_t));
	memset(brc_lcd_indication.g_ram_raw, 0x00, LCD_DATA_SIZE);

	return ret_val;
}

static uint32_t write_indication(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	uint8_t	b_local_blink_current_state;
	
	b_local_blink_current_state = brc_lcd_indication.g_ram.blink.current_state;
	
	if(brc_lcd_indication.g_ram_busy)
		ret_val = NRF_ERROR_BUSY;
	else
	{
		if(length == sizeof(brc_lcd_g_ram_t))
		{
			memcpy(&brc_lcd_indication.g_ram, p_arg, 
				   sizeof(brc_lcd_g_ram_t));
			brc_lcd_indication.g_ram.blink.current_state = 
				b_local_blink_current_state;
			ret_val = NRF_SUCCESS;
			//b_local_current_pattern_num = 0;
			//indication_timeslot_timer_id_handler((void *)&brc_lcd_indication.g_ram);
		}
		else
			ret_val = NRF_ERROR_INVALID_LENGTH;
	}
	return ret_val;
}

static uint32_t execute_indication(void * p_arg, uint32_t length)
{
	uint32_t ret_val = NRF_SUCCESS;
	
	/*if(length == sizeof(brc_lcd_g_ram_t))
		indication_timeslot_timer_id_handler((void *)&brc_lcd_indication.g_ram);*/
	
	return ret_val;
}
/*Public functions*/
brc_lcd_g_ram_t * brc_lcd_indication_get_g_ram(void)
{
	return &brc_lcd_indication.g_ram;
}
/******************************* END_OF_FILE **********************************/