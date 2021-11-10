#include <stdint.h>


#ifndef B3_PARAM_H__
#define B3_PARAM_H__

typedef __packed struct Param_Flags_Struct
{	
	uint16_t HeaterState           : 1;  
	uint16_t PowerState            : 1;
	uint16_t TimerState            : 1;
	uint16_t BuzzerState           : 1;
	uint16_t AutoState             : 1;
	uint16_t MAConnect             : 1;
	uint16_t SaveBit               : 1;
	uint16_t MAPairing             : 1;
	uint16_t PresetState           : 1;
	uint16_t Reserved              : 7;
}Param_Flags_Struct_t;

typedef __packed struct Param_struct
{
  	uint8_t FanSpeed :4;
	uint8_t DumperPos:4;
	 int8_t T_Set;
	Param_Flags_Struct_t Breezer_State_of_Flags;
	 int8_t OutputTemp1;
	 int8_t OutputTemp2;
	 int8_t InputTemp;
	uint16_t Filter_Time; 
	uint8_t usHour;
	uint8_t usMinutes;  
	uint8_t erLastErrorNum; 
	uint8_t m3_per_hour;          
	uint16_t filter_time_days; 
	uint16_t br3s_fv;
	uint8_t Alarm_On_Hours;
	uint8_t Alarm_On_Minutes;
	uint8_t Alarm_Off_Hours;
	uint8_t Alarm_Off_Minutes;
}Param_struct_t;

#endif

