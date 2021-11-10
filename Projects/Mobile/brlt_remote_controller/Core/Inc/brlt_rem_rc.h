#ifndef _BRLT_REM_RC_H_
#define _BRLT_REM_RC_H_

#include "stdint.h"
#include "app_util_platform.h"
#include "ma_ble.h"

#define ERROR_TYPE_NUMBER			20


#define PRESET_NUMBER               3
typedef struct
{
	 int8_t temp[PRESET_NUMBER];
	uint8_t fan_speed[PRESET_NUMBER];
} brlt_settings_presets_t;

typedef struct
{
	uint8_t max_fan_speed;
} brlt_settings_limits_t;

typedef struct _tagBRLT_ComSet_TypeDef
{
	uint8_t  GatePosition;
	 int8_t  SetTemperature;
	uint8_t  FanSpeed;
}BRLT_ComSet_TypeDef;

typedef struct 
{
	uint16_t  State			:1;
	uint16_t  SndIndicationState	:1;
	uint16_t  LEDIndicationState	:1;
	uint16_t  AutoCO2		:1;
	uint16_t  HeaterState		:1;
	uint16_t  FactoryReset		:1;
	uint16_t  ErrorReset		:1;
	uint16_t  FilterReset		:1;
	uint16_t  Save			:1;
	uint16_t  Reserved		:7;
}BRLT_Com_Flags_TypeDef;

typedef struct 
{
	uint16_t  State			:1;
	uint16_t  SndIndicationState	:1;
	uint16_t  LEDIndicationState	:1;
	uint16_t  LastComSource		:1;
	uint16_t  FilterWornout		:1;
	uint16_t  AutoCO2		:1;
	uint16_t  HeaterState		:1;
	uint16_t  HeaterPresent		:1;
	uint16_t  Reserved		:8;
}BRLT_Sys_Flags_TypeDef;

typedef __packed struct  _tagBRLT_ComParams_TypeDef
{
  	BRLT_Com_Flags_TypeDef		ComFlags;
	BRLT_ComSet_TypeDef		ComSet;
	brlt_settings_presets_t	button_presets;
	uint16_t 	filter_time_in_days;
	uint8_t		test_type;
}BRLT_ComParams_TypeDef;

typedef __packed struct _tagBRLT_RC_RF_SetCmd_TypeDef
{	
	uint32_t CmdID;	
  	
	BRLT_ComParams_TypeDef	Command;
}BRLT_RC_RF_SetCmd_TypeDef;

typedef __packed struct _tagBRLT_RC_RF_SetPreset_TypeDef
{	
/*Copy from Breezer 3S		*/
	uint32_t CmdID;		
}BRLT_RC_RF_SetPreset_TypeDef;

ANON_UNIONS_ENABLE
typedef __packed struct _tagBRLT_RC_RF_Command_TypeDef
{	
	uint32_t FrameID;
	uint32_t RequestID;
	union{
	void *Data;
	BRLT_RC_RF_SetCmd_TypeDef *SetCmd;	
	BRLT_RC_RF_SetPreset_TypeDef *PresetCmd;
	};
	uint32_t DataSize;
}BRLT_RC_RF_Command_TypeDef;
ANON_UNIONS_DISABLE


typedef struct 
{
	int8_t T_in;
	int8_t T_out;
	int8_t PCB_Temp;
}BRLT_Sensor_State_TypeDef;

typedef struct 
{
	uint32_t WorkTime;
	uint32_t FanTime;
	uint32_t FilterTime;
	uint32_t airflow_counter;
}BRLT_Counters_TypeDef;

typedef struct
{
	uint8_t er[ERROR_TYPE_NUMBER];
}BRLT_ErrorCnt_TypeDef;

typedef struct
{
	uint32_t 			reg;
	BRLT_ErrorCnt_TypeDef	 	cnt;
}brlt_error_data;

typedef __packed struct _tagBRLT_UserParams_TypeDef
{
	BRLT_Sys_Flags_TypeDef		SysFlags;
	
	uint8_t  GatePosition;
	 int8_t  SetTemperature;
	uint8_t  FanSpeed;
}BRLT_UserParams_TypeDef;

typedef __packed struct _tagBRLT_RC_RF_Response_TypeDef
{
	uint32_t LastCmdId;
  
	BRLT_UserParams_TypeDef		UserSet;
	BRLT_Sensor_State_TypeDef	SensorState;
	BRLT_Counters_TypeDef		SysCounters;
	brlt_error_data errors;
	
	brlt_settings_presets_t	button_presets;
	brlt_settings_limits_t	limits;
	uint8_t					heater_var;
	uint8_t					test_type;
}BRLT_RC_RF_Response_TypeDef;
	
typedef __packed struct _tagBRLT_RC_RF_FrameResponseErrorLog_TypeDef
{
  uint8_t  sysErrorLogBuf[100];
  uint8_t  erNum;
  uint8_t  erLeft;
}BRLT_RC_RF_FrameResponseErrorLog_TypeDef;

typedef __packed struct _tagBLE_DevStatusFrame_TypeDef
{
  uint8_t  WorkMode;
  uint16_t DevType;
  uint16_t DevSType; //subtype
  uint16_t SoftVer;
  uint16_t HardVer;
  uint8_t  Reserved[16];
}BLE_DevStatusFrame_TypeDef;

void brlt_rem_rc_send_get_params_request (void);
void brlt_rem_rc_send_get_dev_status_request (void);

BRLT_RC_RF_Response_TypeDef brlt_rem_get_params_from_frame(ma_ble_frame_t frame);
void brlt_rem_send_set_params_cmd(BRLT_RC_RF_Response_TypeDef* params);
void brlt_rem_send_test_cmd(void);
uint32_t brlt_rem_rc_get_fv_ver (ma_ble_frame_t frame);
#endif /* _BRLT_REM_RC_H_*/

