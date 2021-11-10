/*******************************************************************************
 * @file    brc_buttons.c
 * @author  Sorokin N.N.
 * @date    02-08-2018
 * @brief   brlt remote buttons driver.
 *******************************************************************************/

/* Includes*/
#include "brc_buttons.h"

/*Private defines*/
// clang-format off
#define P_SIZE					sizeof(void *)
#define BUTTONS_ACTIVE_STATE	APP_BUTTON_ACTIVE_LOW

// clang-format on
/*Private macro*/
// clang-format off

// clang-format on
/*Private types*/

/*Private variables*/
APP_TIMER_DEF(m_button_timer_id);

static my_button_event_callback_t   m_registered_callback  = NULL;
static void my_button_event_handler(uint8_t pin_no, uint8_t button_action);
static uint32_t my_button_bits_field = 0;
static uint8_t flag_no_release_event_after_long_push = 0;
static uint8_t btn_list[REM_BUTTONS_NUMBER] = REM_BUTTONS_LIST;
static bool flag_no_release_event = 0;

/*Private constants*/
static const app_button_cfg_t my_buttons[REM_BUTTONS_NUMBER] =
{
	{REM_BUTTON_POWER, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler},
    {REM_BUTTON_STATUS, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler},
    {REM_BUTTON_SET, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler},
    {REM_BUTTON_GATE, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler},
    {REM_BUTTON_TEMP_UP, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler},
	{REM_BUTTON_TEMP_DOWN, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler},
    {REM_BUTTON_SPEED_UP, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler},
    {REM_BUTTON_SPEED_DOWN, BUTTONS_ACTIVE_STATE, NRF_GPIO_PIN_PULLUP, my_button_event_handler}
};
/*Private functions prototypes*/

/*Private functions*/
//**************************************************************
//
//**************************************************************
static void my_button_event_handler(uint8_t pin_no, uint8_t button_action)
{
	uint32_t release_event  = BUTTON_EVENT_NOTHING;
	static uint32_t pot_long_push_event  = BUTTON_EVENT_NOTHING;
	uint16_t time_for_long_push;

	switch(button_action)
	{
		case BUTTON_PUSH:
			my_button_bits_field |= (1<<pin_no);
			break;

		case BUTTON_RELEASE:
			my_button_bits_field &= ~(1<<pin_no);
			switch(pin_no)
			{
				case REM_BUTTON_POWER:
					if (flag_no_release_event == 0)
						release_event = POWER_BUTTON_RELEASE;
					else
						flag_no_release_event	= 0;
					break;
				case REM_BUTTON_STATUS:
					release_event = STATE_BUTTON_RELEASE;
					app_timer_stop(m_button_timer_id);
					break;
				case REM_BUTTON_SET:
					release_event = SET_BUTTON_RELEASE;
					app_timer_stop(m_button_timer_id);
					break;
				case REM_BUTTON_GATE:
					release_event = GATE_BUTTON_RELEASE;
					app_timer_stop(m_button_timer_id);
					break;
				case REM_BUTTON_TEMP_UP	:
					release_event = TEMP_UP_BUTTON_RELEASE;
					app_timer_stop(m_button_timer_id);
					break;
				case REM_BUTTON_TEMP_DOWN:
					release_event = TEMP_DOWN_BUTTON_RELEASE;
					app_timer_stop(m_button_timer_id);
					break;
				case REM_BUTTON_SPEED_UP:
					release_event = FAN_UP_BUTTON_RELEASE;
					app_timer_stop(m_button_timer_id);
					break;
				case REM_BUTTON_SPEED_DOWN:
					release_event = FAN_DOWN_BUTTON_RELEASE;
					app_timer_stop(m_button_timer_id);
					break;
			}
			break;
	}
	switch(my_button_bits_field)
	{
 		case (1<<REM_BUTTON_TEMP_UP):
			pot_long_push_event = TEMP_UP_BUTTON_LONG_PUSH;
			time_for_long_push = BUTTON_LONG_PUSH_FAN_TEMP_TIME;
			break;
		case (1<<REM_BUTTON_TEMP_DOWN):
			pot_long_push_event = TEMP_DOWN_BUTTON_LONG_PUSH;
			time_for_long_push = BUTTON_LONG_PUSH_FAN_TEMP_TIME;
			break;
		case (1<<REM_BUTTON_SPEED_UP):
			pot_long_push_event = FAN_UP_BUTTON_LONG_PUSH;
			time_for_long_push = BUTTON_LONG_PUSH_FAN_TEMP_TIME;
			break;
		case (1<<REM_BUTTON_SPEED_DOWN):
			pot_long_push_event = FAN_DOWN_BUTTON_LONG_PUSH;
			time_for_long_push = BUTTON_LONG_PUSH_FAN_TEMP_TIME;
			break;
		case ((1<<REM_BUTTON_STATUS)|(1<<REM_BUTTON_SET)|(1<<REM_BUTTON_GATE)):
			pot_long_push_event = SMLT_LONG_PUSH_STATE_SET_GATE;
			time_for_long_push = BUTTON_SMLT_LONG_PUSH_STATE_SET_GATE_TIME;
			break;
		case ((1<<REM_BUTTON_STATUS)|(1<<REM_BUTTON_SET)):
			pot_long_push_event = SMLT_LONG_PUSH_STATE_SET;
			time_for_long_push = BUTTON_SMLT_LONG_PUSH_STATE_SET_TIME;
			break;
		case ((1<<REM_BUTTON_STATUS)|(1<<REM_BUTTON_SET)|(1<<REM_BUTTON_SPEED_UP)|(1<<REM_BUTTON_SPEED_DOWN)):
			pot_long_push_event = SMLT_LONG_PUSH_STATE_SET_FAN_UP_FAN_DOWN;
			time_for_long_push = BUTTON_SMLT_LONG_PUSH_STATE_SET_FAN_UP_FAN_DOWN_TIME;
			break;
		case ((1<<REM_BUTTON_TEMP_DOWN)|(1<<REM_BUTTON_TEMP_UP)):
			pot_long_push_event = SMLT_PUSH_TEMP_DOWN_TEMP_UP;
			time_for_long_push = BUTTON_SMLT_PUSH_TEMP_DOWN_TEMP_UP_TIME;
			break;
		case ((1<<REM_BUTTON_TEMP_DOWN)|(1<<REM_BUTTON_POWER)):
			pot_long_push_event = SMLT_LONG_PUSH_TEMP_DOWN_POWER;
			time_for_long_push = BUTTON_SMLT_PUSH_TEMP_DOWN_POWER_TIME;
			break;
		case ((1<<REM_BUTTON_POWER)|(1<<REM_BUTTON_STATUS)):				 //case ((1<<BUTTON_STATUS)|(1<<BUTTON_POWER)):
			pot_long_push_event = SMLT_LONG_PUSH_STATUS_POWER;
			time_for_long_push =  BUTTON_SMLT_LONG_PUSH_STATUS_POWER_TIME;
			break;
		case ((1<<REM_BUTTON_GATE)|(1<<REM_BUTTON_SET)|(1<<REM_BUTTON_TEMP_DOWN)|(1<<REM_BUTTON_SPEED_UP)):
			pot_long_push_event = SMLT_LONG_PUSH_GATE_SET_TEMP_DOWN_SPEED_UP;
			time_for_long_push =  BUTTON_SMLT_LONG_PUSH_GATE_SET_TEMP_DOWN_SPEED_UP_TIME;
			break;
		case ((1<<REM_BUTTON_SET)|(1<<REM_BUTTON_TEMP_DOWN)|(1<<REM_BUTTON_TEMP_UP)):
			pot_long_push_event = SMLT_LONG_PUSH_TEMP_DOWN_TEMP_UP_SET;
			time_for_long_push =  BUTTON_SMLT_LONG_PUSH_TEMP_DOWN_TEMP_UP_SET_TIME;
			break;
		default:
			pot_long_push_event = BUTTON_EVENT_NOTHING;
			break;
	}

	if (pot_long_push_event != BUTTON_EVENT_NOTHING)
	{
		app_timer_stop(m_button_timer_id);
		app_timer_start(m_button_timer_id, APP_TIMER_TICKS(time_for_long_push,0), (void*)&pot_long_push_event);
	}
	if (flag_no_release_event_after_long_push == 1)
		release_event = BUTTON_EVENT_NOTHING;
	if (my_button_bits_field == 0)
		flag_no_release_event_after_long_push = 0;
	m_registered_callback(release_event, NULL, NULL);
}

//**************************************************************
//
//**************************************************************
void button_timer_handler(void * p_context)
{
    uint32_t long_push_event = (*(uint32_t *)p_context);

	if (long_push_event == SMLT_LONG_PUSH_STATE_SET ||
		long_push_event == SMLT_LONG_PUSH_STATE_SET_GATE	||
		long_push_event == SMLT_LONG_PUSH_STATE_SET_FAN_UP_FAN_DOWN ||
		long_push_event == SMLT_PUSH_TEMP_DOWN_TEMP_UP||
		long_push_event == SMLT_LONG_PUSH_TEMP_DOWN_TEMP_UP_SET)
		flag_no_release_event_after_long_push = 1;

		m_registered_callback(long_push_event, NULL, NULL);
}
//**************************************************************
//
//**************************************************************
void my_button_init (my_button_event_callback_t callback)
{
	m_registered_callback = callback;
	app_button_init((app_button_cfg_t *)my_buttons,
                                       REM_BUTTONS_NUMBER,
                                       APP_TIMER_TICKS(10,0));
	app_button_enable();
	app_timer_create(&m_button_timer_id,
					APP_TIMER_MODE_SINGLE_SHOT,
					button_timer_handler);

	flag_no_release_event = app_button_is_pushed(0);
}

uint8_t check_erase_bond_button (void)
{
	nrf_gpio_cfg_input(btn_list[BTN_SPUP_ID],NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(btn_list[BTN_SPDW_ID],NRF_GPIO_PIN_PULLUP);
	if ((nrf_gpio_pin_read(btn_list[BTN_SPUP_ID]) == 0)&&
		(nrf_gpio_pin_read(btn_list[BTN_SPDW_ID]) == 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


uint8_t check_pairing_buttons (void)
{
	bool is_pushed_power = 0;
	bool is_pushed_status = 0;

	is_pushed_power = app_button_is_pushed(BTN_PWR_ID);
	is_pushed_status = app_button_is_pushed(BTN_DISP_ID);
	if ((is_pushed_power == 1)&&(is_pushed_status == 1))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t check_all_buttons (void)
{
	static uint8_t b_local_buttons_state = 1;

	for(uint8_t i = 0; i < BTN_QUANITY; i++)
	{
		nrf_gpio_cfg_input(btn_list[i],NRF_GPIO_PIN_PULLUP);
	}
	for(uint8_t i = 0; i < BTN_QUANITY; i++)
	{
		b_local_buttons_state &= (nrf_gpio_pin_read(btn_list[i]));
	}
	if(b_local_buttons_state != 0)
		return 0;
	else
		return 1;
}



