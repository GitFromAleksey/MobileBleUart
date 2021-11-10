#include <stdint.h>
#include <string.h>

//*****************************************************************
//LCD_MAP
//*****************************************************************
#define SEG_QUANTITY 19

#define LCD_DATA_SIZE SEG_QUANTITY

#define LCD_ON_SEG(a,b) a |= b
#define LCD_OFF_SEG(a,b) a &= (~b)


#define SEG(a) (23-a)    	//Convert_pin_number_in_SEG_number
#define COM(a) (1<<(8 - a))	//Convert_com_number_in_COM_bit_number

#define LCD_SEG_DIGIT_SET_A(NUMBER)				(LCD_ON_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(4)) )
#define LCD_SEG_DIGIT_SET_B(NUMBER)				(LCD_ON_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(3)) )
#define LCD_SEG_DIGIT_SET_C(NUMBER)				(LCD_ON_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(2)) )
#define LCD_SEG_DIGIT_SET_D(NUMBER)				(LCD_ON_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(1)) )
#define LCD_SEG_DIGIT_SET_E(NUMBER)				(LCD_ON_SEG(LCD_Data[SEG((18-2*NUMBER))],  COM(2)) )
#define LCD_SEG_DIGIT_SET_F(NUMBER)				(LCD_ON_SEG(LCD_Data[SEG((18-2*NUMBER))],  COM(4)) )
#define LCD_SEG_DIGIT_SET_G(NUMBER)				(LCD_ON_SEG(LCD_Data[SEG((18-2*NUMBER))],  COM(3)) )

#define LCD_SEG_DIGIT_CLEAR_A(NUMBER)			(LCD_OFF_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(4)) )
#define LCD_SEG_DIGIT_CLEAR_B(NUMBER)			(LCD_OFF_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(3)) )
#define LCD_SEG_DIGIT_CLEAR_C(NUMBER)			(LCD_OFF_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(2)) )
#define LCD_SEG_DIGIT_CLEAR_D(NUMBER)			(LCD_OFF_SEG(LCD_Data[SEG((17-2*NUMBER))],  COM(1)) )
#define LCD_SEG_DIGIT_CLEAR_E(NUMBER)			(LCD_OFF_SEG(LCD_Data[SEG((18-2*NUMBER))],  COM(2)) )
#define LCD_SEG_DIGIT_CLEAR_F(NUMBER)			(LCD_OFF_SEG(LCD_Data[SEG((18-2*NUMBER))],  COM(4)) )
#define LCD_SEG_DIGIT_CLEAR_G(NUMBER)			(LCD_OFF_SEG(LCD_Data[SEG((18-2*NUMBER))],  COM(3)) )

#define LCD_SEG_LITTLE_DIGIT_SET_A(NUMBER)		(LCD_ON_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(1)) )
#define LCD_SEG_LITTLE_DIGIT_SET_B(NUMBER)		(LCD_ON_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_SET_C(NUMBER)		(LCD_ON_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(3)) )
#define LCD_SEG_LITTLE_DIGIT_SET_D(NUMBER)		(LCD_ON_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_SET_E(NUMBER)		(LCD_ON_SEG(LCD_Data[SEG((15+2*NUMBER))],  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_SET_F(NUMBER)		(LCD_ON_SEG(LCD_Data[SEG((15+2*NUMBER))],  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_SET_G(NUMBER)		(LCD_ON_SEG(LCD_Data[SEG((15+2*NUMBER))],  COM(3)) )

#define LCD_SEG_LITTLE_DIGIT_CLEAR_A(NUMBER)	(LCD_OFF_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(1)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_B(NUMBER)	(LCD_OFF_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_C(NUMBER)	(LCD_OFF_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(3)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_D(NUMBER)	(LCD_OFF_SEG(LCD_Data[SEG((16+2*NUMBER))],  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_E(NUMBER)	(LCD_OFF_SEG(LCD_Data[SEG((15+2*NUMBER))],  COM(4)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_F(NUMBER)	(LCD_OFF_SEG(LCD_Data[SEG((15+2*NUMBER))],  COM(2)) )
#define LCD_SEG_LITTLE_DIGIT_CLEAR_G(NUMBER)	(LCD_OFF_SEG(LCD_Data[SEG((15+2*NUMBER))],  COM(3)) )

#define LCD_SEG_ALARM_CLOCK() 					(LCD_ON_SEG(LCD_Data[SEG(23)],  COM(1)) )
#define LCD_SEG_ALARM_CLOCK_CLEAR() 			(LCD_OFF_SEG(LCD_Data[SEG(23)], COM(1)) )

#define LCD_SEG_FILTER() 						(LCD_ON_SEG(LCD_Data[SEG(17)],  COM(1)) )
#define LCD_SEG_FILTER_CLEAR() 					(LCD_OFF_SEG(LCD_Data[SEG(17)], COM(1)) )

#define LCD_SEG_HEAT() 							(LCD_ON_SEG(LCD_Data[SEG(21)],  COM(1)) )
#define LCD_SEG_HEAT_OFF()						(LCD_ON_SEG(LCD_Data[SEG(19)],  COM(1)) )
#define LCD_SEG_HEAT_CLEAR() 					(LCD_OFF_SEG(LCD_Data[SEG(21)], COM(1)) )
#define LCD_SEG_HEAT_OFF_CLEAR()				(LCD_OFF_SEG(LCD_Data[SEG(19)], COM(1)) )

#define LCD_SEG_LEFT_ARROW()					(LCD_ON_SEG(LCD_Data[SEG(23)],  COM(2)) )
#define LCD_SEG_BREZEER_ICON()					(LCD_ON_SEG(LCD_Data[SEG(23)],  COM(3)) )
#define LCD_SEG_RIGHT_ARROW()					(LCD_ON_SEG(LCD_Data[SEG(23)],  COM(4)) )

#define LCD_SEG_LEFT_ARROW_CLEAR()				(LCD_OFF_SEG(LCD_Data[SEG(23)], COM(2)) )
#define LCD_SEG_BREZEER_ICON_CLEAR()			(LCD_OFF_SEG(LCD_Data[SEG(23)], COM(3)) )
#define LCD_SEG_RIGHT_ARROW_CLEAR()				(LCD_OFF_SEG(LCD_Data[SEG(23)], COM(4)) )

#define LCD_SEG_POINT()							(LCD_ON_SEG(LCD_Data[SEG(10)],  COM(1)) )
#define LCD_SEG_POINT_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(10)], COM(1)) )

#define LCD_SEG_FAN() 							(LCD_ON_SEG(LCD_Data[SEG(16)],  COM(1)) )
#define LCD_SEG_FAN_CLEAR() 					(LCD_OFF_SEG(LCD_Data[SEG(16)], COM(1)) )

#define LCD_SEG_SPEED1()						(LCD_ON_SEG(LCD_Data[SEG(14)],  COM(1)) )
#define LCD_SEG_SPEED2()						(LCD_ON_SEG(LCD_Data[SEG(12)],  COM(1)) )
#define LCD_SEG_SPEED3()						(LCD_ON_SEG(LCD_Data[SEG(8)],  	COM(1)) )
#define LCD_SEG_SPEED4()						(LCD_ON_SEG(LCD_Data[SEG(6)], 	COM(2)) )
#define LCD_SEG_SPEED5()						(LCD_ON_SEG(LCD_Data[SEG(6)],  	COM(3)) )
#define LCD_SEG_SPEED6()						(LCD_ON_SEG(LCD_Data[SEG(6)],  	COM(4)) )

#define LCD_SEG_SPEED1_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(14)], COM(1)) )
#define LCD_SEG_SPEED2_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(12)], COM(1)) )
#define LCD_SEG_SPEED3_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(8)],  COM(1)) )
#define LCD_SEG_SPEED4_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(6)],  COM(2)) )
#define LCD_SEG_SPEED5_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(6)],  COM(3)) )
#define LCD_SEG_SPEED6_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(6)],  COM(4)) )

#define LCD_SEG_CIRCULATION() 					(LCD_ON_SEG(LCD_Data[SEG(5)],  	COM(1)) )
#define LCD_SEG_OUTSIDE()						(LCD_ON_SEG(LCD_Data[SEG(6)],  	COM(1)) )
#define LCD_SEG_MA()							(LCD_ON_SEG(LCD_Data[SEG(5)],  	COM(2)) )
#define LCD_SEG_AUTO()							(LCD_ON_SEG(LCD_Data[SEG(5)],  	COM(3)) )

#define LCD_SEG_CIRCULATION_CLEAR() 			(LCD_OFF_SEG(LCD_Data[SEG(5)],  COM(1)) )
#define LCD_SEG_OUTSIDE_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(6)],  COM(1)) )
#define LCD_SEG_AUTO_CLEAR()					(LCD_OFF_SEG(LCD_Data[SEG(5)],  COM(3)) )
#define LCD_SEG_MA_CLEAR()						(LCD_OFF_SEG(LCD_Data[SEG(5)],  COM(2)) )

#define LCD_ALL_SEG_CLEAR()           (memset(LCD_Data,0x00,LCD_DATA_SIZE))

//*****************************************************************
//HT1622 PINS
//*****************************************************************
#define HT1622_CS_PIN						13
#define HT1622_RD_PIN						12
#define HT1622_WR_PIN						11
#define HT1622_DATA_PIN						10
#define HT1622_IRQ_PIN						9
//*****************************************************************
//COMMANDS
//*****************************************************************
#define SYS_DIS 					0x00
#define SYS_EN						0x01
#define LCD_OFF						0x02
#define LCD_ON						0x03
#define TEST_MODE					0xE0
#define NORMAL_MODE					0xE3
#define SET_1_2_BIAS				0x28 //Work only for HT1621
#define SET_1_3_BIAS				0x29 //Work only for HT1621
//*****************************************************************
#define NOTHING 2
#define ON 1
#define OFF 0
//*****************************************************************
//FUNCTION PROTOTYPES
//*****************************************************************
void LCD_HT_WriteData (void);
void LCD_HT_WriteCommand(uint8_t command);
void LCD_HT_Init(void);
void LCD_Set_Digit_Value(uint8_t number_of_digit, uint8_t value);

void LCD_Clear (void);
void LCD_Test_All_Seg_Static(void);
void LCD_Test_All_Seg_Dynamic(uint16_t delay);

void Update_FanSpeed (uint8_t speed);
void Update_DumpPos (uint8_t pos);
void Update_MaFlag (uint8_t MAflag);
void Update_AutoFlag (uint8_t AutoFlag);
void Update_Time (uint8_t Hours,uint8_t Minutes);
void Update_Temp (int8_t Temp);
void Update_Temp_Set (int8_t Temp);
void Update_Temp_In (int8_t Temp);
void Update_Temp_Out (int8_t Temp);
void Update_Filter_Time (uint16_t Filter_Time);
void Update_Heater_State (uint8_t heater_state);

void Diplay_Timer_State (uint8_t timer_state);
void Diplay_Preset_State (uint8_t preset_state);

void Display_Heater_State (uint8_t heater_state);
void Diplay_Filter_State (uint8_t filter_state);
void Write_ON_OFF_Little_Digit (uint8_t state);

void Print_Heater_OFF (void);

void Update_Setting_Filter_Time (uint16_t filter_time);

void Update_Temp_for_Setting (int8_t Temp);

void Clear_ALL_Digit (void);

void Update_PowerState (void);

void Display_Standby (void);
void Clear_Speed_Info (void) ;

void Printf_Nocon (void) ;
void Printf_Conn (void) ;
void Printf_Nores (void) ;

void Printf_ERR_Code (uint8_t err_code);
void Printf_Clean (void);

void Printf_Pair (void);
void Printf_Npair (void);

void Display_Heater_dis (void);
void Clear_ALL_Digit_without_Point (void);

void Write_FirmW_Version (uint8_t version);

void Clear_Air_Direction (void);
//*****************************************************************
//VARIABLES
//*****************************************************************
