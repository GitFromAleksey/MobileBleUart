#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "BRC_Menu_LCD.h"
#include "BRC_HT16XX_LCD.h"

#define INC 1
#define DEC 0

#define BLINK_TIME 500

#define BLINK_TIME_SPEED_LIMIT 1000
#define BLINK_NUMBER_SPEED_LIMIT 3


#define INC_HOURS(hours) if(hours == 23){hours = 0;} else {hours++;}
#define DEC_HOURS(hours) if(hours == 0){hours = 23;} else {hours--;}

#define INC_MINUTES(minutes) if(minutes == 59){minutes = 0;} else {minutes++;}
#define DEC_MINUTES(minutes) if(minutes == 0){minutes = 59;} else {minutes--;}

APP_TIMER_DEF(Setting_params_blink_timer_id);
APP_TIMER_DEF(Speed_limit_blink_timer_id);
APP_TIMER_DEF(MA_pairing_blink_timer_id);

APP_TIMER_DEF(Filter_time_blink_timer_id);

Param_struct_t * p_Param_struct;



void Save_Settings_Params(t_rem_con_setting_menu_state menu_state){}

void Setting_params_blink_timer_id_handler (void* p_context);

uint8_t blink_counter = 0;
uint8_t blink_counter_speed_limit = 0;
uint8_t blink_counter_ma_pairing = 0;
uint8_t blink_counter_filter_time = 0;

uint16_t previous_filter_time = 1;

uint8_t *  p_speed_limit_flag;
uint16_t previous_ma_pairing = 0;



void reset_ma_pairing_state (void)
{
	previous_ma_pairing = 0;
	app_timer_stop(MA_pairing_blink_timer_id);
}


void display_setting_heater_state (uint8_t blink_state)
{

	Write_ON_OFF_Little_Digit(p_Param_struct->Breezer_State_of_Flags.HeaterState);
	Display_Heater_State(p_Param_struct->Breezer_State_of_Flags.HeaterState);


	if(blink_state)
	{
		if(p_Param_struct->Breezer_State_of_Flags.HeaterState)
		 {
			Update_Temp_for_Setting(p_Param_struct->T_Set);
		 }
		 else
		 {
			Display_Heater_dis();
		 }
	}
	else
	{
			if(p_Param_struct->Breezer_State_of_Flags.HeaterState)
		 {
			Clear_ALL_Digit();
		 }
		 else
		 {
			Clear_ALL_Digit_without_Point();
		 }

	}

}

void display_setting_time (uint8_t blink_state)
{

	if(blink_state)
	{
		Update_Time(p_Param_struct->usHour,p_Param_struct->usMinutes);
	}
	else
	{
		Clear_ALL_Digit();
	}
}

void display_setting_time_alarm_on (uint8_t blink_state)
{
	Diplay_Timer_State(ON);
	Write_ON_OFF_Little_Digit(ON);

	if(blink_state)
	{
		Update_Time(p_Param_struct->Alarm_On_Hours,p_Param_struct->Alarm_On_Minutes);
	}
	else
	{
		Clear_ALL_Digit();
	}
}

void display_setting_time_alarm_off (uint8_t blink_state)
{
	Diplay_Timer_State(ON);
	Write_ON_OFF_Little_Digit(OFF);

	if(blink_state)
	{
		Update_Time(p_Param_struct->Alarm_Off_Hours,p_Param_struct->Alarm_Off_Minutes);
	}
	else
	{
		Clear_ALL_Digit();
	}
}


void display_setting_timer_state (uint8_t blink_state)
{
	Write_ON_OFF_Little_Digit(p_Param_struct->Breezer_State_of_Flags.TimerState);

	if(blink_state)
	{
		Diplay_Timer_State(ON);
	}
	else
	{
		Diplay_Timer_State(NOTHING);
	}
}


void display_setting_filter_time (uint8_t blink_state)
{
	if (blink_state)
	{
		Diplay_Filter_State(ON);
		Update_Setting_Filter_Time(p_Param_struct->Filter_Time);
	}
	else
	{
		Write_ON_OFF_Little_Digit(NOTHING);
		Diplay_Filter_State(NOTHING);
	}
}


void display_standby_mode (uint8_t blink_state)
{
	Diplay_Timer_State(p_Param_struct->Breezer_State_of_Flags.TimerState);
	Update_MaFlag (p_Param_struct->Breezer_State_of_Flags.MAConnect);
	Update_AutoFlag (p_Param_struct->Breezer_State_of_Flags.AutoState);

	if (blink_state)
	{
		Display_Standby();
	}
	else
	{
		Clear_ALL_Digit();
	}
}

void display_no_filter_time (uint8_t blink_state)
{
	if (blink_state)
	{
		Diplay_Filter_State(ON);
		Update_Setting_Filter_Time(p_Param_struct->Filter_Time);

		if ((*p_speed_limit_flag) == 0)
			Update_FanSpeed(p_Param_struct->FanSpeed);
	}
	else
	{
		if ((*p_speed_limit_flag) == 0)
			Clear_Speed_Info();

		Write_ON_OFF_Little_Digit(NOTHING);
		Diplay_Filter_State(NOTHING);
	}
}

void display_speed_limit (uint8_t blink_state)
{
	if (blink_state)
	{
		Update_FanSpeed(p_Param_struct->FanSpeed);
	}
	else
	{
		Clear_Speed_Info();
	}
}



void display_connect_waiting (uint8_t blink_state)
{
	if (blink_state)
	{
		Printf_Conn();
	}
	else
	{
		Clear_ALL_Digit();
	}
}

void display_pairing_waiting (uint8_t blink_state)
{
	if (blink_state)
	{
		Printf_Pair();
	}
	else
	{
		Clear_ALL_Digit();
	}
}

void display_ma_pairing (uint8_t blink_state)
{
	if (blink_state)
	{
		Update_MaFlag(ON);
	}
	else
	{
		Update_MaFlag(OFF);
	}
}



void display_Tset (uint8_t blink_state)
{
	Clear_ALL_Digit();

	if(blink_state)
	{
		if(p_Param_struct->Breezer_State_of_Flags.HeaterState)
		{
			Update_Temp_Set(p_Param_struct->T_Set);
		}
		else
		{
			Print_Heater_OFF();
		}
	}
	else
	{

		if(p_Param_struct->Breezer_State_of_Flags.HeaterState)
		{
			Clear_ALL_Digit();
		}
		else
		{
			Print_Heater_OFF();
		}
	}

}

void Exit_Settings_Menu(void)
{
app_timer_stop(Setting_params_blink_timer_id);
}

void Exit_Speed_Limit (void)
{
app_timer_stop(Speed_limit_blink_timer_id);
}




void MA_pairing_blink_timer_id_handler (void * p_context)
{
	p_blinking_function_t blinking_function;

	blinking_function = (p_blinking_function_t)p_context;
	if (blink_counter_ma_pairing == 0)
	{
		(*blinking_function)(ON);
		blink_counter_ma_pairing = 1;
	}
	else
	{
		(*blinking_function)(OFF);
		blink_counter_ma_pairing = 0;
	}
	LCD_HT_WriteData();
}


void Speed_limit_blink_timer_id_handler (void * p_context)
{
	p_blinking_function_t blinking_function;

	blinking_function = (p_blinking_function_t)p_context;
	if (blink_counter_speed_limit == 0)
	{
		(*blinking_function)(ON);
		blink_counter_speed_limit = 1;
	}
	else
	{
		(*blinking_function)(OFF);
		blink_counter_speed_limit = 0;
	}
//***************************************
//speed limit
	if ((*p_speed_limit_flag) != 0)
	{
	(*p_speed_limit_flag) ++;
	}

	if ((*p_speed_limit_flag) == (BLINK_NUMBER_SPEED_LIMIT*2+2))
	{
	(*p_speed_limit_flag) = 0;
	Exit_Speed_Limit();
	}

//***************************************
	LCD_HT_WriteData();
}
void Setting_params_blink_timer_id_handler (void * p_context)
{
	p_blinking_function_t blinking_function;

 	blinking_function = (p_blinking_function_t)p_context;

	if (blink_counter == 0)
	{
		(*blinking_function)(ON);
		blink_counter = 1;
	}
	else
	{
		(*blinking_function)(OFF);
		blink_counter = 0;
	}
	LCD_HT_WriteData();
}

void Filter_time_blink_timer_handler (void * p_context)
{
	p_blinking_function_t blinking_function;

	blinking_function = (p_blinking_function_t)p_context;
	if (blink_counter_filter_time == 0)
	{
		(*blinking_function)(ON);
		blink_counter_filter_time = 1;
	}
	else
	{
	(*blinking_function)(OFF);
		blink_counter_filter_time = 0;
	}

	LCD_HT_WriteData();
}


void Start_Settings_Menu(void)
{
	app_timer_create(&Setting_params_blink_timer_id,
                      APP_TIMER_MODE_REPEATED,
                      Setting_params_blink_timer_id_handler);

	(*p_speed_limit_flag) = 0;
	Exit_Speed_Limit();

}

void Start_Standby_mode (void)
{
	uint32_t err_code;
	p_blinking_function_t current_blink_function;

	LCD_Clear();
	display_standby_mode(ON);
	current_blink_function = &display_standby_mode;

	err_code = app_timer_stop(Setting_params_blink_timer_id);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(Setting_params_blink_timer_id,
				APP_TIMER_TICKS(BLINK_TIME,0),(void *)current_blink_function);
	APP_ERROR_CHECK(err_code);
	LCD_HT_WriteData();
	blink_counter = 1;
	(*p_speed_limit_flag) = 0;
	Exit_Speed_Limit();
}

void Start_Wait_Connect (uint8_t scan_type)
{
	uint32_t err_code;
	p_blinking_function_t current_blink_function;

	LCD_Clear();
	if (scan_type == SCAN_SIMPLE)
	{
		display_pairing_waiting(ON);
		current_blink_function = &display_pairing_waiting;
	}
	else if (scan_type == SCAN_WHITELIST)
	{
		display_connect_waiting(ON);
		current_blink_function = &display_connect_waiting;
	}
	err_code = app_timer_stop(Setting_params_blink_timer_id);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(Setting_params_blink_timer_id,
				APP_TIMER_TICKS(BLINK_TIME,0), (void *)current_blink_function);
	APP_ERROR_CHECK(err_code);
	LCD_HT_WriteData();
	blink_counter = 1;
	(*p_speed_limit_flag) = 0;
	Exit_Speed_Limit();
}

void Exit_Wait_Connect (uint8_t connect_flag)
{
	app_timer_stop(Setting_params_blink_timer_id);
	if (connect_flag == 1)
	{
		//Printf_Nores();
	}
	else if (connect_flag == 0)
	{
		Printf_Nocon();
	}
	else if (connect_flag == 2)
	{
		Printf_Npair();
	}
	LCD_HT_WriteData();
}

void Exit_Standby_mode (void)
{
	uint32_t err_code;

	err_code = app_timer_stop(Setting_params_blink_timer_id);
	APP_ERROR_CHECK(err_code);
}


void Start_No_Filter_Time (void)
{
	uint32_t err_code;
	p_blinking_function_t current_blink_function;

	display_no_filter_time(ON);
	current_blink_function = &display_no_filter_time;
	err_code = app_timer_stop(Filter_time_blink_timer_id);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(Filter_time_blink_timer_id,
				APP_TIMER_TICKS(BLINK_TIME,0), (void *)current_blink_function);
	APP_ERROR_CHECK(err_code);
	LCD_HT_WriteData();
	blink_counter_filter_time = 1;
	(*p_speed_limit_flag) = 0;
	Exit_Speed_Limit();
}

void Exit_No_Filter_Time (void)
{
	app_timer_stop(Filter_time_blink_timer_id);
}

void Start_Tset_blink (void)
{
	uint32_t err_code;
	p_blinking_function_t current_blink_function;

	Clear_ALL_Digit();
	display_Tset(ON);
	current_blink_function = &display_Tset;

	err_code = app_timer_stop(Setting_params_blink_timer_id);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(Setting_params_blink_timer_id,
				APP_TIMER_TICKS(BLINK_TIME,0), (void *)current_blink_function);
	APP_ERROR_CHECK(err_code);
	LCD_HT_WriteData();
	blink_counter = 1;
}

void Stop_Tset_blink (void)
{
	app_timer_stop(Setting_params_blink_timer_id);
}


void Start_Speed_Limit (void)
{
	uint32_t err_code;
	p_blinking_function_t current_blink_function;

	display_speed_limit(ON);
	current_blink_function = &display_speed_limit;
	err_code = app_timer_stop(Speed_limit_blink_timer_id);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(Speed_limit_blink_timer_id,
	APP_TIMER_TICKS(BLINK_TIME_SPEED_LIMIT,0), (void *)current_blink_function);
	APP_ERROR_CHECK(err_code);
	LCD_HT_WriteData();
	blink_counter_speed_limit = 1;
}

void Start_MA_Pairing (void)
{

	uint32_t err_code;
	p_blinking_function_t current_blink_function;

	display_ma_pairing(ON);
	current_blink_function = &display_ma_pairing;
	err_code = app_timer_stop(MA_pairing_blink_timer_id);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(MA_pairing_blink_timer_id,
				APP_TIMER_TICKS(BLINK_TIME,0), (void*)current_blink_function);
	APP_ERROR_CHECK(err_code);
	LCD_HT_WriteData();
	blink_counter_ma_pairing = 1;
}

void Exit_MA_Pairing (void)
{
	app_timer_stop(MA_pairing_blink_timer_id);
}

void Update_Settings_Menu_LCD_State(t_rem_con_setting_menu_state menu_state)
{
	uint32_t err_code;
	//LCD_Clear();
	p_blinking_function_t current_blink_function;
	switch (menu_state)
	{
		case SET_HEATER_STATE:
			display_setting_heater_state(ON);
			current_blink_function = &display_setting_heater_state;
			break;

		case SET_TIME_HOURS_STATE:
			display_setting_time(ON);
			current_blink_function = &display_setting_time;
			break;

		case SET_TIME_MIN_STATE:
			display_setting_time(ON);
			current_blink_function = &display_setting_time;
			break;

		case SET_TIMER_STATE:
			display_setting_timer_state(ON);
			current_blink_function = &display_setting_timer_state;
			break;

		case SET_ALARM_ON_HOURS_STATE:
			display_setting_time_alarm_on(ON);
			current_blink_function = &display_setting_time_alarm_on;
			break;

		case SET_ALARM_ON_MIN_STATE:
			display_setting_time_alarm_on(ON);
			current_blink_function = &display_setting_time_alarm_on;
			break;

		case SET_ALARM_OFF_HOURS_STATE:
			display_setting_time_alarm_off(ON);
			current_blink_function = &display_setting_time_alarm_off;
			break;

		case SET_ALARM_OFF_MIN_STATE:
			display_setting_time_alarm_off(ON);
			current_blink_function = &display_setting_time_alarm_off;
			break;

		case SET_FILTER_TIME_STATE:
			display_setting_filter_time(ON);
			current_blink_function = &display_setting_filter_time;
			break;
	}

	err_code = app_timer_stop(Setting_params_blink_timer_id);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(Setting_params_blink_timer_id,APP_TIMER_TICKS(BLINK_TIME,0),(void *)current_blink_function);
	APP_ERROR_CHECK(err_code);

	LCD_HT_WriteData();
	blink_counter = 1;
}



void Execute_Settings_Menu_State (t_rem_con_setting_menu_state menu_state,uint8_t Direction_Param)
{

switch (menu_state)
{
	case SET_HEATER_STATE:

		p_Param_struct->Breezer_State_of_Flags.HeaterState =!p_Param_struct->Breezer_State_of_Flags.HeaterState;

	break;
	case SET_TIME_HOURS_STATE:

	if (p_Param_struct->Breezer_State_of_Flags.MAConnect)
	{
	}
	else
	{

		if(Direction_Param)
		{
			INC_HOURS(p_Param_struct->usHour);
		}
		else
		{
			DEC_HOURS(p_Param_struct->usHour);
		}
	}
	break;

	case SET_TIME_MIN_STATE:

	if (p_Param_struct->Breezer_State_of_Flags.MAConnect)
	{
	}
	else
	{
		if(Direction_Param)
		{
			INC_MINUTES(p_Param_struct->usMinutes);
		}
		else
		{
			DEC_MINUTES(p_Param_struct->usMinutes);
		}
	}
	break;
	case SET_TIMER_STATE:


			if ( (p_Param_struct->Breezer_State_of_Flags.MAConnect == 1) &&
					(p_Param_struct->Breezer_State_of_Flags.AutoState == 1))
		{
		}
		else
		{
			if (p_Param_struct->Breezer_State_of_Flags.TimerState == 0)
			{
				if (p_Param_struct->Breezer_State_of_Flags.PresetState)
				{
				p_Param_struct->Breezer_State_of_Flags.PresetState = 0;
				}

			p_Param_struct->Breezer_State_of_Flags.TimerState = 1;
			}
			else
			{
			p_Param_struct->Breezer_State_of_Flags.TimerState = 0;

			}


		}



	break;
	case SET_ALARM_ON_HOURS_STATE:

		if(Direction_Param)
		{
			INC_HOURS(p_Param_struct->Alarm_On_Hours);
		}
		else
		{
			DEC_HOURS(p_Param_struct->Alarm_On_Hours);
		}

	break;
	case SET_ALARM_ON_MIN_STATE:

		if(Direction_Param)
		{
			INC_MINUTES(p_Param_struct->Alarm_On_Minutes);
		}
		else
		{
			DEC_MINUTES(p_Param_struct->Alarm_On_Minutes);
		}

	break;
	case SET_ALARM_OFF_HOURS_STATE:

		if(Direction_Param)
		{
			INC_HOURS(p_Param_struct->Alarm_Off_Hours);
		}
		else
		{
			DEC_HOURS(p_Param_struct->Alarm_Off_Hours);
		}

	break;
	case SET_ALARM_OFF_MIN_STATE:

		if(Direction_Param)
		{
			INC_MINUTES(p_Param_struct->Alarm_Off_Minutes);
		}
		else
		{
			DEC_MINUTES(p_Param_struct->Alarm_Off_Minutes);
		}

	break;
	case SET_FILTER_TIME_STATE:

		if(Direction_Param)
		{
			if(p_Param_struct->Filter_Time >= 330)
			{
				p_Param_struct->Filter_Time  = 360;
			}
			else
			{

				p_Param_struct->Filter_Time = ((p_Param_struct->Filter_Time/30)+1)*30;
			}
		}
		else
		{
			if(p_Param_struct->Filter_Time <= 30)
			{
				p_Param_struct->Filter_Time  = 0;
			}
			else
			{
				if(p_Param_struct->Filter_Time%30 != 0)
				{
					p_Param_struct->Filter_Time = ((p_Param_struct->Filter_Time/30))*30;
				}
				else
				{
					p_Param_struct->Filter_Time = ((p_Param_struct->Filter_Time/30)-1)*30;
				}
				}

		}

		break;

	}
	Update_Settings_Menu_LCD_State(menu_state);
}

void Check_Speed_Limit_flag (uint8_t * speed_limit_flag)
{
	p_speed_limit_flag = speed_limit_flag;
	if ((*p_speed_limit_flag) == 1)
	{
		Start_Speed_Limit();
		(*p_speed_limit_flag)++;
	}
}




uint8_t  Check_MA_Pairing (void)
{


		if ( ( p_Param_struct->Breezer_State_of_Flags.MAPairing == 0) && ( previous_ma_pairing == 1))
		{
		Exit_MA_Pairing();
		Update_MaFlag (OFF);

		return 0;
		}
		else if ( ( p_Param_struct->Breezer_State_of_Flags.MAPairing == 1) && ( previous_ma_pairing == 0))
		{
		Start_MA_Pairing();

		return 1;
		}


		return 2;
}


void Update_Display_LCD_State (t_rem_con_display_info_type display_info_type,t_rem_con_work_mode work_mode)
{
	switch (work_mode)
	{
		case CRITICAL_ERROR_MODE:
			Printf_ERR_Code(p_Param_struct->erLastErrorNum);
			LCD_HT_WriteData();
			break;

		case STANDBY_MODE:

			break;

		case MA_PAIRING_MODE: //warning

		case DISPLAY_MODE:
		//	LCD_Clear();

			if(display_info_type != TEMP_SET_TYPE)
				Clear_ALL_Digit();
			
			Update_DumpPos(p_Param_struct->DumperPos);
			Diplay_Preset_State(p_Param_struct->Breezer_State_of_Flags.PresetState);
			if (p_Param_struct->Breezer_State_of_Flags.PresetState == 0)
			{
				Diplay_Timer_State(p_Param_struct->Breezer_State_of_Flags.TimerState);
				Update_AutoFlag (p_Param_struct->Breezer_State_of_Flags.AutoState);
			}
			Update_Heater_State(p_Param_struct->Breezer_State_of_Flags.HeaterState);
			
			if(p_Param_struct->Filter_Time != 0)
			{
				if(p_Param_struct->Filter_Time <= 30)
				{
					Diplay_Filter_State(ON);
					Update_Setting_Filter_Time(p_Param_struct->Filter_Time);
				}
				else
				{
					Write_ON_OFF_Little_Digit(NOTHING);
					Diplay_Filter_State(NOTHING);
				}
			}

			if ( ( (*p_speed_limit_flag) == 0) && (p_Param_struct->Filter_Time != 0))
			{
				Clear_Speed_Info();
				Update_FanSpeed(p_Param_struct->FanSpeed);
			}

			if ( (p_Param_struct->Filter_Time == 0) && (previous_filter_time>0) )
			{
				Start_No_Filter_Time();
			}
			else if ( (p_Param_struct->Filter_Time > 0) && (previous_filter_time == 0) )
			{
				Exit_No_Filter_Time();
			}
			if (!p_Param_struct->Breezer_State_of_Flags.MAPairing)
				Update_MaFlag (p_Param_struct->Breezer_State_of_Flags.MAConnect);
	
			switch (display_info_type)
			{
				case TEMP_OUT_TYPE:
					if ( (p_Param_struct->OutputTemp1 + p_Param_struct->OutputTemp2) >= 0 )
					{
						Update_Temp_Out( (p_Param_struct->OutputTemp1+p_Param_struct->OutputTemp2+1)/2);
					}
					else
					{
						Update_Temp_Out( (p_Param_struct->OutputTemp1+p_Param_struct->OutputTemp2-1)/2);
					}
					break;
						
				case TEMP_IN_TYPE:
					Update_Temp_In(p_Param_struct->InputTemp);
					break;
						
				case TIME_TYPE:
					Update_Time(p_Param_struct->usHour,p_Param_struct->usMinutes);
					break;

				case TEMP_SET_TYPE:
		/*
					if(p_Param_struct->Breezer_State_of_Flags.HeaterState)
					{
						Update_Temp_Set(p_Param_struct->T_Set);
					}
					else
					{
						Print_Heater_OFF();
					}
	*/
					break;

				default:
					break;
			}
			previous_filter_time = p_Param_struct->Filter_Time;
			previous_ma_pairing = p_Param_struct->Breezer_State_of_Flags.MAPairing;
			LCD_HT_WriteData();
			break;

	case SETTING_MODE:

		break;

	default:
		break;
	}
}

void LCD_Menu_Init (Param_struct_t * Current_Breezer_Params)
{
	uint8_t erase_speed_limit =0;

	p_Param_struct = Current_Breezer_Params;
	LCD_HT_Init();
	Check_Speed_Limit_flag(&erase_speed_limit);
	app_timer_create(&Setting_params_blink_timer_id,
                      APP_TIMER_MODE_REPEATED,
                      Setting_params_blink_timer_id_handler);

	app_timer_create(&Speed_limit_blink_timer_id,
                      APP_TIMER_MODE_REPEATED,
                      Speed_limit_blink_timer_id_handler);

	app_timer_create(&MA_pairing_blink_timer_id,
                      APP_TIMER_MODE_REPEATED,
                      MA_pairing_blink_timer_id_handler);

	app_timer_create(&Filter_time_blink_timer_id,
                      APP_TIMER_MODE_REPEATED,
                      Filter_time_blink_timer_handler);
}

void LCD_Menu_Sleep (void)
{

	LCD_HT_WriteCommand(LCD_OFF);
	LCD_HT_WriteCommand(SYS_DIS);

	nrf_gpio_cfg_output(HT1622_CS_PIN);
	nrf_gpio_pin_set(HT1622_CS_PIN);

	nrf_gpio_cfg_output(HT1622_RD_PIN);
	nrf_gpio_pin_set(HT1622_RD_PIN);

	nrf_gpio_cfg_output(HT1622_WR_PIN);
	nrf_gpio_pin_set(HT1622_WR_PIN);

	nrf_gpio_cfg_output(HT1622_DATA_PIN);
	nrf_gpio_pin_set(HT1622_DATA_PIN);

	nrf_gpio_cfg_output(HT1622_IRQ_PIN);
	nrf_gpio_pin_set(HT1622_IRQ_PIN);
}

void test_test (void)
{
	LCD_Test_All_Seg_Dynamic(25);
}

void Display_firmw_version (uint8_t version)
{
	Write_FirmW_Version(version);
	LCD_HT_WriteData();
}

void display_clean (void)
{
	Printf_Clean();
	LCD_HT_WriteData();
}