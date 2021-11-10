/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "timer_drv.h"
/*Private defines*/
// clang-format off
#define APP_TIMER_PRESCALER 	0	//< Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE 12	//< Size of timer operation queues. 
// clang-format on
/*Private macro*/
// clang-format off

// clang-format on
/*Private types*/

/*Exported constants*/


/*Private variables*/
APP_TIMER_DEF(m_timeslot_timer); 
APP_TIMER_DEF(m_blink_timer);
APP_TIMER_DEF(m_get_status_timer);
APP_TIMER_DEF(m_button_timer);
APP_TIMER_DEF(m_auto_sleep_timer);
APP_TIMER_DEF(m_user_timeout_timer);
APP_TIMER_DEF(m_misc_timer);
APP_TIMER_DEF(m_cont_param_change_timer);


static const app_timer_id_t * p_timers[NUMBER_OF_USER_TIMERS] = 
{
	&m_get_status_timer,
	&m_timeslot_timer,
	&m_blink_timer,
	&m_button_timer,
	&m_auto_sleep_timer,
	&m_user_timeout_timer,
	&m_misc_timer,
	&m_cont_param_change_timer,
};
/*Private functions prototypes*/
static timer_drv_output_evt_handler_t output_evt;

static uint32_t timer_drv_input_evt_handler(timer_drv_input_evt_type_t event, 
							uint8_t * p_data, uint16_t data_len);

static void get_status_timer_handler(void * p_context);
static void timeslot_timer_handler(void * p_context);
static void blink_timer_handler(void * p_context);
static void button_timer_handler(void * p_context);
static void auto_sleep_timer_handler(void * p_context);
static void user_timeout_timer_handler(void * p_context);
static void misc_timer_handler(void * p_context);
static void cont_param_change_timer_handler(void * p_context);
/*Private functions*/

static uint8_t context;
static uint32_t timer_drv_input_evt_handler(timer_drv_input_evt_type_t event, 
							uint8_t * p_data, uint16_t data_len)
{
	uint32_t err_code = NRF_SUCCESS;
	timer_config_t * p_timer = (timer_config_t *) p_data;
    switch (event)
    {
		case START_TIMER:
		{
			uint32_t timeout;
			timeout = APP_TIMER_TICKS(p_timer->timeout, 0);
			err_code = app_timer_start(*p_timers[p_timer->id], 
									   timeout, p_timer->p_context);
		}	break;
		
		case STOP_TIMER:
		{
			err_code = app_timer_stop(*p_timers[p_timer->id]);
		}	break;
		
		case RELOAD_TIMER:
		{
			uint32_t timeout;
			timeout = APP_TIMER_TICKS(p_timer->timeout, 0);
			if (p_timer->id == CONT_PARAM_CHANGE_TIMER_ID)
			{
				uint8_t b_local_timer_id;
				context = *(uint8_t *)p_timer->p_context;
				
				b_local_timer_id = p_timer->id;
				err_code = app_timer_stop(*p_timers[b_local_timer_id]);
				err_code = app_timer_start(*p_timers[b_local_timer_id], 
									   timeout, (void *)&context);
			}
			else
			{
				err_code = app_timer_stop(*p_timers[p_timer->id]);
				err_code = app_timer_start(*p_timers[p_timer->id], 
									   timeout, p_timer->p_context);
			}
		}	break;
		case GET_TIMER_STATE:
		{			
		}	break;
		default:
			break;
	}
	
	return err_code;
}

static void get_status_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(GET_STATUS_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
}

static void timeslot_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(TIMESLOT_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
	
}

static void blink_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(BLINK_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
}

static void button_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(BUTTON_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
}

static void auto_sleep_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(AUTO_SLEEP_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
}

static void user_timeout_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(USER_TIMEOUT_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
}

static void misc_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(MISC_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
}

static void cont_param_change_timer_handler(void * p_context)
{
	if(output_evt != NULL)
	{
		output_evt(CONT_PARAM_CHANGE_TIMER_OVF, (uint8_t *)p_context, NULL);
	}
}
/*Public functions*/
timer_drv_input_evt_handler_t timer_drv_init(timer_drv_output_evt_handler_t 
										 timer_drv_output_evt_handler)
{
	uint32_t err_code;
		
	output_evt = timer_drv_output_evt_handler;
		
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);
	
	err_code = app_timer_create(&m_get_status_timer,
								APP_TIMER_MODE_REPEATED,
								get_status_timer_handler);
	err_code = app_timer_create(&m_timeslot_timer,
								APP_TIMER_MODE_REPEATED,
								timeslot_timer_handler);
	err_code = app_timer_create(&m_blink_timer,
								APP_TIMER_MODE_REPEATED,
								blink_timer_handler);
	err_code = app_timer_create(&m_button_timer,
								APP_TIMER_MODE_SINGLE_SHOT,
								button_timer_handler);
	err_code = app_timer_create(&m_auto_sleep_timer,
								APP_TIMER_MODE_SINGLE_SHOT,
								auto_sleep_timer_handler);
	err_code = app_timer_create(&m_user_timeout_timer,
								APP_TIMER_MODE_SINGLE_SHOT,
								user_timeout_timer_handler);
	err_code = app_timer_create(&m_misc_timer,
								APP_TIMER_MODE_SINGLE_SHOT,
								misc_timer_handler);
	err_code = app_timer_create(&m_cont_param_change_timer,
								APP_TIMER_MODE_REPEATED,
								cont_param_change_timer_handler);
	if(err_code != NRF_SUCCESS)
		APP_ERROR_CHECK(err_code);
	
	return &timer_drv_input_evt_handler;
}
/******************************* END_OF_FILE **********************************/