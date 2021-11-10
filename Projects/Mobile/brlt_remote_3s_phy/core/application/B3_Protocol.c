#include "B3_Protocol.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


void Convert_Response_Packet_to_Param (Breezer_Get_Param_Struct_t * Recieve_Ble_Param_Struct,Param_struct_t * Param_Struct)
{
	Param_Struct->FanSpeed = Recieve_Ble_Param_Struct->FanSpeed;
	Param_Struct->DumperPos = Recieve_Ble_Param_Struct->DumperPos;
	
	memcpy(&(Param_Struct->T_Set),&(Recieve_Ble_Param_Struct->T_Set),sizeof(*Param_Struct)-5); //warning -5

	Param_Struct->Filter_Time = Recieve_Ble_Param_Struct->Filter_time;
}


void Check_in_Response_Packet_in_Settings (Breezer_Get_Param_Struct_t * Recieve_Ble_Param_Struct,Param_struct_t * Param_Struct)
{
	Param_Struct->Breezer_State_of_Flags.PowerState = Recieve_Ble_Param_Struct->Breezer_State_of_Flags.PowerState;
	Param_Struct->erLastErrorNum = Recieve_Ble_Param_Struct->erLastErrorNum;
}

void Convert_Alarm_Response_Packet_to_Param (Breezer_Alarm_Response_Struct_t * Recieve_Ble_Param_Struct,Param_struct_t * Param_Struct)
{
	Param_Struct->Alarm_On_Hours = Recieve_Ble_Param_Struct->Alarm1.Hours;
	Param_Struct->Alarm_On_Minutes = Recieve_Ble_Param_Struct->Alarm1.Minutes;

	Param_Struct->Alarm_Off_Hours = Recieve_Ble_Param_Struct->Alarm2.Hours;
	Param_Struct->Alarm_Off_Minutes = Recieve_Ble_Param_Struct->Alarm2.Minutes;
}




void Create_Get_Param_request (uint8_t * buffer_for_ble_send)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

Breezer_Get_Param_Struct_t Send_Request_Get_Param_Frame;

memset(&Send_Request_Get_Param_Frame,0x00,sizeof(Send_Request_Get_Param_Frame));

Send_Request_Get_Param_Frame.devID = ADAPTER_ID;
Send_Request_Get_Param_Frame.comID = SEND_REQ_COM_ID;
Send_Request_Get_Param_Frame.CRC8 = CALCULATE_CRC;

memcpy(buffer_for_ble_send,&Send_Request_Get_Param_Frame,FRAME_SIZE);
}

void Create_Set_Param_command (uint8_t * buffer_for_ble_send,Param_struct_t * Param_struct)
{
	memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

	Breezer_Set_Param_Struct_t Set_Param_Frame;
	memset(&Set_Param_Frame,0x00,sizeof(Set_Param_Frame));
	Set_Param_Frame.devID = ADAPTER_ID;
	Set_Param_Frame.comID = SEND_SET_PARAM_COM_ID;
	Set_Param_Frame.CRC8 = CALCULATE_CRC;
	memcpy(&Set_Param_Frame.Breezer_State_of_Flags,
		   &(Param_struct->Breezer_State_of_Flags),
		   sizeof(Set_Param_Frame.Breezer_State_of_Flags));
	Set_Param_Frame.DumperPos = Param_struct->DumperPos;
	Set_Param_Frame.FanSpeed = Param_struct->FanSpeed;
	Set_Param_Frame.T_Set = Param_struct->T_Set;
	Set_Param_Frame.FilterTime.FTValue = Param_struct->Filter_Time;
	memcpy(buffer_for_ble_send,&Set_Param_Frame,FRAME_SIZE);
}

void Create_Hard_reset_command (uint8_t * buffer_for_ble_send)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));
Breezer_Set_Param_Struct_t Set_Param_Frame;
memset(&Set_Param_Frame,0x00,sizeof(Set_Param_Frame));


Set_Param_Frame.devID = ADAPTER_ID;
Set_Param_Frame.comID = SEND_SET_PARAM_COM_ID;
Set_Param_Frame.CRC8 = CALCULATE_CRC;

Set_Param_Frame.Hard_reset_flag = 1;

memcpy(buffer_for_ble_send,&Set_Param_Frame,FRAME_SIZE);

}


void Create_Set_Alarm_Command (uint8_t * buffer_for_ble_send,Param_struct_t * Alarm_struct, uint8_t alarm_type)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

Breezer_Set_Alarm_Struct_t Set_Alarm_Frame;
memset(&Set_Alarm_Frame,0x00,sizeof(Set_Alarm_Frame));


Set_Alarm_Frame.devID = ADAPTER_ID;
Set_Alarm_Frame.comID = SEND_SET_ALARM_COM_ID;
Set_Alarm_Frame.CRC8 = CALCULATE_CRC;

Set_Alarm_Frame.comType = 1;
Set_Alarm_Frame.Alarm.TempSet = Alarm_struct->T_Set;
Set_Alarm_Frame.Alarm.AlarmSet.DumperPos = Alarm_struct->DumperPos;
Set_Alarm_Frame.Alarm.AlarmSet.FanSpeed = Alarm_struct->FanSpeed;


if (alarm_type == 1) //Alarm ON
{
Set_Alarm_Frame.Alarm.AlarmStatus.AlarmID = 0;
Set_Alarm_Frame.Alarm.AlarmStatus.ComSource = 0;
Set_Alarm_Frame.Alarm.AlarmStatus.AlarmState = 1;
Set_Alarm_Frame.Alarm.AlarmStatus.PowerState = 1;

Set_Alarm_Frame.Alarm.AlarmTimeStart.Hours = Alarm_struct->Alarm_On_Hours;
Set_Alarm_Frame.Alarm.AlarmTimeStart.Minutes = Alarm_struct->Alarm_On_Minutes;
}
else if (alarm_type == 0) //Alarm OFF
{
Set_Alarm_Frame.Alarm.AlarmStatus.AlarmID = 1;
Set_Alarm_Frame.Alarm.AlarmStatus.ComSource = 0;
Set_Alarm_Frame.Alarm.AlarmStatus.AlarmState = 1;
Set_Alarm_Frame.Alarm.AlarmStatus.PowerState = 0;

Set_Alarm_Frame.Alarm.AlarmTimeStart.Hours = Alarm_struct->Alarm_Off_Hours;
Set_Alarm_Frame.Alarm.AlarmTimeStart.Minutes = Alarm_struct->Alarm_Off_Minutes;
}

memcpy(buffer_for_ble_send,&Set_Alarm_Frame,FRAME_SIZE);

}

void Create_Set_Time_Command (uint8_t * buffer_for_ble_send,Param_struct_t * Time_struct)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

Breezer_Set_Time_Struct_t Set_Time_Frame;
memset(&Set_Time_Frame,0x00,sizeof(Set_Time_Frame));


Set_Time_Frame.devID = ADAPTER_ID;
Set_Time_Frame.comID = SEND_SET_TIME_COM_ID;
Set_Time_Frame.CRC8 = CALCULATE_CRC;



Set_Time_Frame.usDateTime.Time.Hours = Time_struct->usHour;
Set_Time_Frame.usDateTime.Time.Minutes = Time_struct->usMinutes;

Set_Time_Frame.usDateTime.Time.Seconds = 0;

memcpy(buffer_for_ble_send,&Set_Time_Frame,FRAME_SIZE);

}


void Create_Set_Alarm_request (uint8_t * buffer_for_ble_send)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

Breezer_Set_Alarm_Struct_t Set_Alarm_Frame;
memset(&Set_Alarm_Frame,0x00,sizeof(Set_Alarm_Frame));


Set_Alarm_Frame.devID = ADAPTER_ID;
Set_Alarm_Frame.comID = SEND_SET_ALARM_COM_ID;
Set_Alarm_Frame.CRC8 = CALCULATE_CRC;

Set_Alarm_Frame.comType = 0;

memcpy(buffer_for_ble_send,&Set_Alarm_Frame,FRAME_SIZE);

}


void Create_Set_Param_command_Save_filter (uint8_t * buffer_for_ble_send,Param_struct_t * Param_struct)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

Breezer_Set_Param_Struct_t Set_Param_Frame;
memset(&Set_Param_Frame,0x00,sizeof(Set_Param_Frame));


Set_Param_Frame.devID = ADAPTER_ID;
Set_Param_Frame.comID = SEND_SET_PARAM_COM_ID;
Set_Param_Frame.CRC8 = CALCULATE_CRC;

memcpy(&Set_Param_Frame.Breezer_State_of_Flags,&(Param_struct->Breezer_State_of_Flags),sizeof(Set_Param_Frame.Breezer_State_of_Flags));
//Set_Param_Frame.Breezer_State_of_Flags.SaveBit = save_bit;

Set_Param_Frame.DumperPos = Param_struct->DumperPos;
Set_Param_Frame.FanSpeed = Param_struct->FanSpeed;
Set_Param_Frame.T_Set = Param_struct->T_Set;

Set_Param_Frame.FilterTime.FTValue = Param_struct->Filter_Time;
Set_Param_Frame.FilterTime.FTSaveBit = 1;

memcpy(buffer_for_ble_send,&Set_Param_Frame,FRAME_SIZE);

}


void Create_Set_Param_command_Reset_filter (uint8_t * buffer_for_ble_send,Param_struct_t * Param_struct)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

Breezer_Set_Param_Struct_t Set_Param_Frame;
memset(&Set_Param_Frame,0x00,sizeof(Set_Param_Frame));


Set_Param_Frame.devID = ADAPTER_ID;
Set_Param_Frame.comID = SEND_SET_PARAM_COM_ID;
Set_Param_Frame.CRC8 = CALCULATE_CRC;

memcpy(&Set_Param_Frame.Breezer_State_of_Flags,&(Param_struct->Breezer_State_of_Flags),sizeof(Set_Param_Frame.Breezer_State_of_Flags));
//Set_Param_Frame.Breezer_State_of_Flags.SaveBit = save_bit;

Set_Param_Frame.DumperPos = Param_struct->DumperPos;
Set_Param_Frame.FanSpeed = Param_struct->FanSpeed;
Set_Param_Frame.T_Set = Param_struct->T_Set;

Set_Param_Frame.FilterTime.FTValue = Param_struct->Filter_Time;
Set_Param_Frame.FilterTime.FTReset = 1;

memcpy(buffer_for_ble_send,&Set_Param_Frame,FRAME_SIZE);

}


void Create_Set_Param_command_Service_mode (uint8_t * buffer_for_ble_send, Param_struct_t * Param_struct)
{
memset(buffer_for_ble_send,0x00,sizeof(*buffer_for_ble_send));

Breezer_Set_Param_Struct_t Set_Param_Frame;
memset(&Set_Param_Frame,0x00,sizeof(Set_Param_Frame));


Set_Param_Frame.devID = ADAPTER_ID;
Set_Param_Frame.comID = SEND_SET_PARAM_COM_ID;
Set_Param_Frame.CRC8 = CALCULATE_CRC;

memcpy(&Set_Param_Frame.Breezer_State_of_Flags,&(Param_struct->Breezer_State_of_Flags),sizeof(Set_Param_Frame.Breezer_State_of_Flags));
//Set_Param_Frame.Breezer_State_of_Flags.SaveBit = save_bit;

Set_Param_Frame.DumperPos = Param_struct->DumperPos;
Set_Param_Frame.FanSpeed = Param_struct->FanSpeed;
Set_Param_Frame.T_Set = Param_struct->T_Set;

Set_Param_Frame.FilterTime.FTValue = Param_struct->Filter_Time;
Set_Param_Frame.Service_mode_flag = 1;

memcpy(buffer_for_ble_send,&Set_Param_Frame,FRAME_SIZE);

}