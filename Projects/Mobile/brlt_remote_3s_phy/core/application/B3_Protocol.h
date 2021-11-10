#include <stdint.h>

#include "B3_Params.h"

#define ADAPTER_ID 					0x3D
#define BREEZER_CUB_ID				0xB3
#define FRAME_SIZE					20
#define CALCULATE_CRC				0x5A

#define SEND_REQ_COM_ID       		0x01
#define SEND_REQ_RESP_ID       		0x10

#define SEND_SET_PARAM_COM_ID		0x02
#define SEND_SET_PARAM_RESP_ID		0x20

#define SEND_SET_TIME_COM_ID       	0x03
#define SEND_SET_TIME_RESP_ID		0x30

#define SEND_SET_ALARM_COM_ID		0x04
#define SEND_SET_ALARM_RESP_ID		0x40



typedef __packed struct Flags_Struct
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
	

}Flags_State_struct_t;


typedef __packed struct tagBLE_RC_AlarmTime_TypeDef
{
  uint8_t Hours;  
  uint8_t Minutes;
}BLE_RC_AlarmTime_TypeDef;

typedef __packed struct tagMAIN_SystemTime_TypeDef
{
 uint8_t Hours;
 uint8_t Minutes;
 uint8_t Seconds;
}MAIN_SystemTime_TypeDef;

typedef __packed struct tagMAIN_SystemDate_TypeDef
{
 uint8_t Year;
        uint8_t WeekDay :4;
 uint8_t Month   :4;
 uint8_t Day;
}MAIN_SystemDate_TypeDef;



typedef __packed struct tagMAIN_SystemTimePackage_TypeDef
{
        MAIN_SystemDate_TypeDef         Date;
        MAIN_SystemTime_TypeDef         Time;
}MAIN_SystemTimePackage_TypeDef;



typedef __packed struct tagMAIN_SystemAlarmState_TypeDef
{
        uint8_t AlarmID         : 4;
        uint8_t AlarmState      : 1;
        uint8_t PowerState      : 1; 
        uint8_t ComSource       : 2; 
}MAIN_SystemAlarmState_TypeDef;


typedef __packed struct tagMAIN_SystemUserSet_TypeDef
{
        int8_t FanSpeed        : 4;
 int8_t DumperPos       : 4;
}MAIN_SystemUserSet_TypeDef;


 typedef __packed struct tagMAIN_SystemAlarm_TypeDef
{
        MAIN_SystemAlarmState_TypeDef   AlarmStatus;
        MAIN_SystemUserSet_TypeDef      AlarmSet;
  int8_t TempSet;
        MAIN_SystemTime_TypeDef         AlarmTimeStart;
}MAIN_SystemAlarm_TypeDef;


typedef __packed struct tagBLE_RC_FilterTimeSet_TypeDef
{
  uint8_t FTSaveBit     :1;
  uint8_t FTReset       :1;
  uint8_t FTReserved    :6;
  uint16_t FTValue;
}BLE_RC_FilterTimeSet_TypeDef;




typedef __packed  struct  Breezer_Get_Param_Struct
{
  uint8_t devID;
  uint8_t comID;
  uint8_t FanSpeed :4;
	uint8_t DumperPos:4;
   int8_t T_Set;
	Flags_State_struct_t Breezer_State_of_Flags;
   int8_t OutputTemp1;
   int8_t OutputTemp2;
   int8_t InputTemp;
	uint16_t Filter_time; 
  uint8_t usHour;
  uint8_t usMinutes;  
	 uint8_t erLastErrorNum; 
	uint8_t Reserved1;          
  uint8_t Reserved2; 
  uint8_t Reserved3; 
  uint8_t Reserved4;
  uint8_t Reserved5; 
  uint8_t CRC8; 
}Breezer_Get_Param_Struct_t;



typedef __packed  struct Breezer_Set_Param_Struct
{
  uint8_t devID;
  uint8_t comID;
  uint8_t FanSpeed;
   int8_t T_Set;
  uint8_t DumperPos;
	Flags_State_struct_t Breezer_State_of_Flags;
  BLE_RC_FilterTimeSet_TypeDef  FilterTime;
  uint8_t Hard_reset_flag;        
  uint8_t Service_mode_flag;        
  uint8_t Reserved1;      
  uint8_t Reserved2;  
  uint8_t Reserved3;          
  uint8_t Reserved4; 
  uint8_t Reserved5;
  uint8_t Reserved6;          
  uint8_t Reserved7;         
  uint8_t CRC8;    
}Breezer_Set_Param_Struct_t;



typedef __packed  struct Breezer_Set_Time_Struct
{
  uint8_t devID;
  uint8_t comID;
  MAIN_SystemTimePackage_TypeDef usDateTime;
  uint8_t Reserved1;     
  uint8_t Reserved2;
  uint8_t Reserved3;
  uint8_t Reserved4;  
  uint8_t Reserved5;
  uint8_t Reserved6;
  uint8_t Reserved7;
  uint8_t Reserved8;
  uint8_t Reserved9;  
  uint8_t Reserved10;
  uint8_t Reserved11;
  uint8_t CRC8;
}Breezer_Set_Time_Struct_t;


typedef __packed  struct Breezer_Set_Alarm_Struct
{
  uint8_t devID;
  uint8_t comID;
  uint8_t comType;
  MAIN_SystemAlarm_TypeDef      Alarm;
  uint8_t Reserved1;          
  uint8_t Reserved2;        
  uint8_t Reserved3; 
  uint8_t Reserved4;
  uint8_t Reserved5;
  uint8_t Reserved6;
  uint8_t Reserved7;
  uint8_t Reserved8;
  uint8_t Reserved9;  
  uint8_t Reserved10;
  uint8_t CRC8;
}Breezer_Set_Alarm_Struct_t;


typedef __packed  struct Breezer_Alarm_Response_Struct
{
  uint8_t devID;
  uint8_t comID;
  BLE_RC_AlarmTime_TypeDef      Alarm1;
  BLE_RC_AlarmTime_TypeDef      Alarm2;
  BLE_RC_AlarmTime_TypeDef      Alarm3;
  BLE_RC_AlarmTime_TypeDef      Alarm4;
  uint8_t Reserved1;          
  uint8_t Reserved2; 
  uint8_t Reserved3; 
  uint8_t Reserved4;
  uint8_t Reserved5; 
  uint8_t Reserved6;
  uint8_t Reserved7;          
  uint8_t Reserved8; 
  uint8_t Reserved9;
  uint8_t CRC8;
}Breezer_Alarm_Response_Struct_t;



void Convert_Response_Packet_to_Param (Breezer_Get_Param_Struct_t * Recieve_Ble_Param_Struct,Param_struct_t * Param_Struct);
void Convert_Alarm_Response_Packet_to_Param (Breezer_Alarm_Response_Struct_t * Recieve_Ble_Param_Struct,Param_struct_t * Param_Struct);

void Create_Get_Param_request (uint8_t * buffer_for_ble_send);
void Create_Set_Param_command (uint8_t * buffer_for_ble_send,Param_struct_t * Param_struct);
void Create_Set_Time_Command (uint8_t * buffer_for_ble_send,Param_struct_t * Time_struct);

void Create_Set_Param_command_Save_filter (uint8_t * buffer_for_ble_send,Param_struct_t * Param_struct);
void Create_Set_Param_command_Reset_filter (uint8_t * buffer_for_ble_send,Param_struct_t * Param_struct);
void Create_Set_Param_command_Service_mode (uint8_t * buffer_for_ble_send,Param_struct_t * Param_struct);

void Create_Hard_reset_command (uint8_t * buffer_for_ble_send);


void Create_Set_Alarm_Command (uint8_t * buffer_for_ble_send,Param_struct_t * Alarm_struct, uint8_t alarm_type);
void Create_Set_Alarm_request (uint8_t * buffer_for_ble_send);

void Check_in_Response_Packet_in_Settings (Breezer_Get_Param_Struct_t * Recieve_Ble_Param_Struct,Param_struct_t * Param_Struct); //SHIIIT