#include <stdint.h>
#include "B3_Params.h"

#define PARAM_DOWN 0
#define PARAM_UP 1


#define SCAN_WHITELIST 0
#define SCAN_SIMPLE 1


typedef enum _mode {
  	STANDBY_MODE = 0,
  	DISPLAY_MODE,
	SETTING_MODE,
	NOCONN_MODE,
	MA_PAIRING_MODE,
	CRITICAL_ERROR_MODE
} t_rem_con_work_mode;





typedef enum _display_info_type {
  TEMP_OUT_TYPE = 0,
  TEMP_IN_TYPE ,
	TIME_TYPE ,
	TEMP_SET_TYPE
} t_rem_con_display_info_type;

typedef enum _setting_menu_state
{
  	SET_HEATER_STATE = 0,
  	SET_TIME_HOURS_STATE,
	SET_TIME_MIN_STATE,
	SET_TIMER_STATE,
	SET_ALARM_ON_HOURS_STATE,
	SET_ALARM_ON_MIN_STATE,
	SET_ALARM_OFF_HOURS_STATE,
	SET_ALARM_OFF_MIN_STATE,
	SET_FILTER_TIME_STATE
} t_rem_con_setting_menu_state;

typedef void (*p_blinking_function_t)(uint8_t state);

void Exit_Settings_Menu(void);
void Start_Settings_Menu(void);

void Start_Standby_mode (void);
void Exit_Standby_mode (void);

void Start_No_Filter_Time (void);
void Exit_No_Filter_Time (void);

void Start_Tset_blink (void);
void Stop_Tset_blink (void);



void Start_Wait_Connect (uint8_t scan_type);
void Exit_Wait_Connect (uint8_t connect_flag);

void Exit_Speed_Limit (void);

//void Save_Settings_Params(t_rem_con_setting_menu_state menu_state);

void Execute_Settings_Menu_State (t_rem_con_setting_menu_state menu_state,uint8_t Direction_Param);



void Update_Settings_Menu_LCD_State(t_rem_con_setting_menu_state menu_state);
void Update_Display_LCD_State (t_rem_con_display_info_type display_info_type,t_rem_con_work_mode work_mode);

void LCD_Menu_Init (Param_struct_t * Current_Breezer_Params);

void Check_Speed_Limit_flag (uint8_t * speed_limit_flag);

uint8_t  Check_MA_Pairing (void);

void LCD_Menu_Sleep (void);

void reset_ma_pairing_state (void);

void test_test (void);
void display_clean (void);

void Display_firmw_version (uint8_t version);