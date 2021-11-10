#include <string.h>

#include "BRC_HT16XX_LCD.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_spi.h"
#include "app_util_platform.h"

uint8_t LCD_Data[LCD_DATA_SIZE];

uint8_t SPI_TX_buffer [LCD_DATA_SIZE+1];

//0b x x x x x x x x
//   X G F E D C B A
const uint8_t DigitCode[]=
{0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71,
 //0   //1   //2   //3   //4   //5   //6   //7   //8   //9   //A   //b   //C   //d   //E   //F

0x37,0x3F,0x61,0x54,0x00, 0x76,0x50,0x6D,0x38,0x73,0x30
//N	 //O   //C //n  //OFF ,H   ,r    ,S  ,L   ,P   ,I
};

volatile uint8_t SPI_Command_TC = 0;


void LCD_HT_WriteData(void);
void LCD_HT_WriteCommand(uint8_t command);
void LCD_HT_Init(void);

// дефолтный инстанс для SPI 0
static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);

void spi_master_0_event_handler(nrf_drv_spi_evt_t const * p_event)
{
   SPI_Command_TC = 0;
}

void LCD_HT_WriteData(void)
{
	char MSB = 0, LSB;
	uint8_t staple_buffer = 0;
	
	/*SPI_Command_TC = 1;*/
	SPI_TX_buffer[0] = 0xA0;

	for (int i = 0; i < (LCD_DATA_SIZE/2)+1; i++)
	{
		staple_buffer = (LCD_Data[2*i]&0xF0)|((LCD_Data[2*i+1]&0xF0)>>4);
		if ( (staple_buffer & 0x01) != 0) LSB = 0x80;
		else LSB = 0x00;
		SPI_TX_buffer[i+1] =  staple_buffer>>1;
		SPI_TX_buffer[i+1] |= MSB;
		MSB = LSB;
	}	
	/*nrf_drv_spi_transfer(&m_spi_master_0,SPI_TX_buffer,(LCD_DATA_SIZE/2)+2,0,0);
	while (SPI_Command_TC == 1){};*/
}

void LCD_HT_WriteCommand(uint8_t command)
{
	/*SPI_Command_TC = 1;
	SPI_TX_buffer[0] = (command>>3)|0x80;
	SPI_TX_buffer[1] = (command<<5);
	nrf_drv_spi_transfer(&m_spi_master_0,SPI_TX_buffer,2,0,0);
	while (SPI_Command_TC == 1){};*/
}
void LCD_Set_Digit_Value (uint8_t number_of_digit, uint8_t value)
{
	(DigitCode[value]&0x01) ? LCD_SEG_DIGIT_SET_A(number_of_digit) : LCD_SEG_DIGIT_CLEAR_A(number_of_digit);
	(DigitCode[value]&0x02) ? LCD_SEG_DIGIT_SET_B(number_of_digit) : LCD_SEG_DIGIT_CLEAR_B(number_of_digit);
	(DigitCode[value]&0x04) ? LCD_SEG_DIGIT_SET_C(number_of_digit) : LCD_SEG_DIGIT_CLEAR_C(number_of_digit);
	(DigitCode[value]&0x08) ? LCD_SEG_DIGIT_SET_D(number_of_digit) : LCD_SEG_DIGIT_CLEAR_D(number_of_digit);
	(DigitCode[value]&0x10) ? LCD_SEG_DIGIT_SET_E(number_of_digit) : LCD_SEG_DIGIT_CLEAR_E(number_of_digit);
	(DigitCode[value]&0x20) ? LCD_SEG_DIGIT_SET_F(number_of_digit) : LCD_SEG_DIGIT_CLEAR_F(number_of_digit);
	(DigitCode[value]&0x40) ? LCD_SEG_DIGIT_SET_G(number_of_digit) : LCD_SEG_DIGIT_CLEAR_G(number_of_digit);
}

void LCD_Set_Little_Digit_Value (uint8_t number_of_digit, uint8_t value)
{
	(DigitCode[value]&0x01) ? LCD_SEG_LITTLE_DIGIT_SET_A(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_A(number_of_digit);
	(DigitCode[value]&0x02) ? LCD_SEG_LITTLE_DIGIT_SET_B(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_B(number_of_digit);
	(DigitCode[value]&0x04) ? LCD_SEG_LITTLE_DIGIT_SET_C(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_C(number_of_digit);
	(DigitCode[value]&0x08) ? LCD_SEG_LITTLE_DIGIT_SET_D(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_D(number_of_digit);
	(DigitCode[value]&0x10) ? LCD_SEG_LITTLE_DIGIT_SET_E(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_E(number_of_digit);
	(DigitCode[value]&0x20) ? LCD_SEG_LITTLE_DIGIT_SET_F(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_F(number_of_digit);
	(DigitCode[value]&0x40) ? LCD_SEG_LITTLE_DIGIT_SET_G(number_of_digit) : LCD_SEG_LITTLE_DIGIT_CLEAR_G(number_of_digit);
}
//============================================================
//
//============================================================
void LCD_HT_Init(void)
{
	/*nrf_drv_spi_config_t config =
    {
        .ss_pin       	= HT1622_CS_PIN,
		.sck_pin  		= HT1622_WR_PIN,
        .mosi_pin 		= HT1622_DATA_PIN,
        .miso_pin 		= NRF_DRV_SPI_PIN_NOT_USED,
        .irq_priority 	= APP_IRQ_PRIORITY_HIGH,
        .orc          	= 0xCC,
        .frequency    	= NRF_SPI_FREQ_250K,
        .mode         	= NRF_DRV_SPI_MODE_3,
        .bit_order    	= NRF_SPI_BIT_ORDER_MSB_FIRST,
    };
	
	nrf_drv_spi_init(&m_spi_master_0, &config, spi_master_0_event_handler);

	LCD_HT_WriteCommand(SYS_EN);
	LCD_HT_WriteCommand(LCD_ON);
	LCD_HT_WriteCommand(NORMAL_MODE);
	LCD_HT_WriteCommand(SET_1_3_BIAS);*/
}

void LCD_Clear (void)
{
	LCD_ALL_SEG_CLEAR();
}


void Print_Heater_OFF (void)
{
	LCD_SEG_POINT();
	LCD_Set_Digit_Value(1,21); //H
	LCD_SEG_DIGIT_SET_G(2); //-
	LCD_Set_Digit_Value(3,0); //0
	LCD_Set_Digit_Value(4,0x0F); //F
	LCD_Set_Digit_Value(5,0x0F); //F
}

void Update_PowerState (void)
{
	LCD_SEG_DIGIT_SET_G(1);
	LCD_SEG_DIGIT_SET_G(2);
	LCD_SEG_DIGIT_SET_G(4);
	LCD_SEG_DIGIT_SET_G(5);
}

void Display_Standby (void)
{
	LCD_SEG_DIGIT_SET_G(1);
	LCD_SEG_DIGIT_SET_G(2);
	LCD_SEG_DIGIT_SET_G(4);
	LCD_SEG_DIGIT_SET_G(5);
}

void Display_Heater_dis (void)
{
	LCD_SEG_DIGIT_SET_G(2);
	LCD_SEG_DIGIT_SET_G(3);
}

void Update_FanSpeed(uint8_t speed)
{
	LCD_SEG_FAN();
	switch (speed)
	{
		case 1:
			LCD_SEG_SPEED1();
	  		break;

		case 2:
			LCD_SEG_SPEED1();
			LCD_SEG_SPEED2();
			break;

		case 3:
			LCD_SEG_SPEED1();
			LCD_SEG_SPEED2();
			LCD_SEG_SPEED3();
	 		break;

		case 4:
			LCD_SEG_SPEED1();
			LCD_SEG_SPEED2();
			LCD_SEG_SPEED3();
			LCD_SEG_SPEED4();
	  		break;

		case 5:
			LCD_SEG_SPEED1();
			LCD_SEG_SPEED2();
			LCD_SEG_SPEED3();
			LCD_SEG_SPEED4();
			LCD_SEG_SPEED5();
	  		break;

	  	case 6:
			LCD_SEG_SPEED1();
			LCD_SEG_SPEED2();
			LCD_SEG_SPEED3();
			LCD_SEG_SPEED4();
			LCD_SEG_SPEED5();
			LCD_SEG_SPEED6();
	  		break;
	}
}

void Update_DumpPos(uint8_t pos)
{
	LCD_SEG_CIRCULATION_CLEAR();
	LCD_SEG_OUTSIDE_CLEAR();
	switch (pos)
	{
		case 0:
			LCD_SEG_CIRCULATION();
 			break;

		case 1:
			LCD_SEG_CIRCULATION();
			LCD_SEG_OUTSIDE();
		  	break;

		case 2:
			LCD_SEG_OUTSIDE();;
  			break;
	}
}

void Display_Heater_State (uint8_t heater_state)
{
	if (heater_state == NOTHING)
	{
		LCD_SEG_HEAT_CLEAR();
		LCD_SEG_HEAT_OFF_CLEAR();
		Write_ON_OFF_Little_Digit(NOTHING);
	}
	else if (heater_state == ON)
	{
		LCD_SEG_HEAT();
		LCD_SEG_HEAT_OFF_CLEAR();
		Write_ON_OFF_Little_Digit(ON);
	}
	else if (heater_state == OFF)
	{
		LCD_SEG_HEAT();
		LCD_SEG_HEAT_OFF();
		Write_ON_OFF_Little_Digit(OFF);
		LCD_SEG_POINT();
	}
}

void Update_Heater_State (uint8_t heater_state)
{
	if (heater_state == ON)
	{
		LCD_SEG_HEAT();
		LCD_SEG_HEAT_OFF_CLEAR();
	}
	else if (heater_state == OFF)
	{
		LCD_SEG_HEAT();
		LCD_SEG_HEAT_OFF();
	}
}

void Diplay_Preset_State (uint8_t preset_state)
{
	if (preset_state == NOTHING)
	{
		LCD_SEG_ALARM_CLOCK_CLEAR();
		LCD_SEG_AUTO_CLEAR();
	}
	else if (preset_state == ON)
	{
		LCD_SEG_ALARM_CLOCK();
		LCD_SEG_AUTO();
	}
	else if (preset_state == OFF)
	{
		LCD_SEG_ALARM_CLOCK_CLEAR();
		LCD_SEG_AUTO_CLEAR();
	}
}

void Diplay_Timer_State (uint8_t timer_state)
{
	if (timer_state == NOTHING)
	{
		LCD_SEG_ALARM_CLOCK_CLEAR();
	}
	else if (timer_state == ON)
	{
		LCD_SEG_ALARM_CLOCK();
	}
	else if (timer_state == OFF)
	{
		LCD_SEG_ALARM_CLOCK_CLEAR();
	}
}

void Diplay_Filter_State (uint8_t filter_state)
{
	if (filter_state == NOTHING)
	{
		LCD_SEG_FILTER_CLEAR();
	}
	else if (filter_state == ON)
	{
		LCD_SEG_FILTER();
	}
	else if (filter_state == OFF)
	{
		LCD_SEG_FILTER_CLEAR();
	}
}

void Clear_Speed_Info (void)
{
	LCD_SEG_FAN_CLEAR();
	LCD_SEG_SPEED1_CLEAR();
	LCD_SEG_SPEED2_CLEAR();
	LCD_SEG_SPEED3_CLEAR();
	LCD_SEG_SPEED4_CLEAR();
	LCD_SEG_SPEED5_CLEAR();
	LCD_SEG_SPEED6_CLEAR();
}

void Write_ON_OFF_Little_Digit(uint8_t state)
{
	if (state == NOTHING)
	{
		LCD_Set_Little_Digit_Value(1,20);
		LCD_Set_Little_Digit_Value(2,20);
		LCD_Set_Little_Digit_Value(3,20);
	}
	else if (state == ON)
	{
		LCD_Set_Little_Digit_Value(1,0x00);
		LCD_Set_Little_Digit_Value(2,19);
		LCD_Set_Little_Digit_Value(3,20);
	}
	else if (state == OFF)
	{
		LCD_Set_Little_Digit_Value(1,0x00);
		LCD_Set_Little_Digit_Value(2,0x0F);
		LCD_Set_Little_Digit_Value(3,0x0F);
	}
}

void Update_Setting_Filter_Time (uint16_t filter_time)
{
	LCD_Set_Little_Digit_Value(1,filter_time/100);
	LCD_Set_Little_Digit_Value(2,(filter_time%100)/10);
	LCD_Set_Little_Digit_Value(3,(filter_time%100)%10);
}

void Update_MaFlag (uint8_t MAflag)
{
	if(MAflag)
	{
		LCD_SEG_MA();
	}
	else
	{
		LCD_SEG_MA_CLEAR();
	}
}

void Update_AutoFlag (uint8_t AutoFlag)
{
	if(AutoFlag)
	{
		LCD_SEG_AUTO();
	}
	else
	{
		LCD_SEG_AUTO_CLEAR();
	}
}

void Update_Time (uint8_t Hours,uint8_t Minutes)
{
	LCD_SEG_BREZEER_ICON_CLEAR();
	LCD_SEG_RIGHT_ARROW_CLEAR();
	LCD_SEG_LEFT_ARROW_CLEAR();

	LCD_Set_Digit_Value(1,Hours/10);
	LCD_Set_Digit_Value(2,Hours%10);

	LCD_SEG_DIGIT_SET_G(3);

	LCD_Set_Digit_Value(4,Minutes/10);
	LCD_Set_Digit_Value(5,Minutes%10);
}
void Update_Temp (int8_t Temp)
{
	LCD_SEG_POINT();
	LCD_Set_Digit_Value(4,18);

	if (Temp<0)
	{
		LCD_SEG_DIGIT_SET_G(1);
		LCD_Set_Digit_Value(2,-Temp/10);
		LCD_Set_Digit_Value(3,-Temp%10);
	}
	else
	{
		LCD_Set_Digit_Value(2,Temp/10);
		LCD_Set_Digit_Value(3,Temp%10);
	}
}


void Update_Temp_for_Setting (int8_t Temp)
{
	LCD_SEG_POINT();
	if (Temp<0)
	{
		LCD_Set_Digit_Value(2,-Temp/10);
		LCD_Set_Digit_Value(3,-Temp%10);
	}
	else
	{
		LCD_Set_Digit_Value(2,Temp/10);
		LCD_Set_Digit_Value(3,Temp%10);
	}
}
void Update_Temp_Set (int8_t Temp)
{
	LCD_SEG_BREZEER_ICON();
	LCD_SEG_RIGHT_ARROW();

	LCD_SEG_LEFT_ARROW_CLEAR();

	Update_Temp(Temp);
}
void Update_Temp_In (int8_t Temp)
{
	LCD_SEG_BREZEER_ICON();
	LCD_SEG_LEFT_ARROW();

	LCD_SEG_RIGHT_ARROW_CLEAR();

	Update_Temp(Temp);
}
void Update_Temp_Out (int8_t Temp)
{
	LCD_SEG_BREZEER_ICON();
	LCD_SEG_RIGHT_ARROW();

	LCD_SEG_LEFT_ARROW_CLEAR();

	Update_Temp(Temp);
}

void Clear_Air_Direction (void)
{
	LCD_SEG_RIGHT_ARROW_CLEAR();
	LCD_SEG_BREZEER_ICON_CLEAR();
	LCD_SEG_LEFT_ARROW_CLEAR();
}

void Clear_ALL_Digit (void)
{
	LCD_SEG_POINT_CLEAR();
	LCD_Set_Digit_Value(1,20);
	LCD_Set_Digit_Value(2,20);
	LCD_Set_Digit_Value(3,20);
	LCD_Set_Digit_Value(4,20);
	LCD_Set_Digit_Value(5,20);
}

void Clear_ALL_Digit_without_Point (void)
{
	LCD_Set_Digit_Value(1,20);
	LCD_Set_Digit_Value(2,20);
	LCD_Set_Digit_Value(3,20);
	LCD_Set_Digit_Value(4,20);
	LCD_Set_Digit_Value(5,20);
}


void LCD_Test_All_Seg_Dynamic (uint16_t delay)
{
	nrf_delay_ms(delay);
	LCD_SEG_ALARM_CLOCK();
	LCD_HT_WriteData();
	
	nrf_delay_ms(delay);
	LCD_SEG_FILTER();
	LCD_HT_WriteData();
	for (uint8_t a = 0; a <=15;a++)
	{
		nrf_delay_ms(delay);
		LCD_Set_Little_Digit_Value(1,a);
		LCD_Set_Little_Digit_Value(2,a);
		LCD_Set_Little_Digit_Value(3,a);
		LCD_HT_WriteData();
	}
	nrf_delay_ms(delay);
	LCD_Set_Little_Digit_Value(1,8);
	LCD_Set_Little_Digit_Value(2,8);
	LCD_Set_Little_Digit_Value(3,8);
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_HEAT();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_HEAT_OFF();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_LEFT_ARROW();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_BREZEER_ICON();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_RIGHT_ARROW();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_POINT();
	LCD_HT_WriteData();

	for (uint8_t a = 0; a <=15;a++)
	{
		nrf_delay_ms(delay);
		LCD_Set_Digit_Value(1,a);
		LCD_Set_Digit_Value(2,a);
		LCD_Set_Digit_Value(3,a);
		LCD_Set_Digit_Value(4,a);
		LCD_Set_Digit_Value(5,a);
		LCD_HT_WriteData();
	}
	
	nrf_delay_ms(delay);
	LCD_Set_Digit_Value(1,8);
	LCD_Set_Digit_Value(2,8);
	LCD_Set_Digit_Value(3,8);
	LCD_Set_Digit_Value(4,8);
	LCD_Set_Digit_Value(5,8);
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_FAN();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_SPEED1();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_SPEED2();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_SPEED3();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_SPEED4();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_SPEED5();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_SPEED6();
	LCD_HT_WriteData();


	nrf_delay_ms(delay);
	LCD_SEG_CIRCULATION();
	LCD_HT_WriteData();


	nrf_delay_ms(delay);
	LCD_SEG_OUTSIDE();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_MA();
	LCD_HT_WriteData();

	nrf_delay_ms(delay);
	LCD_SEG_AUTO();
	LCD_HT_WriteData();
}

void Printf_Nocon (void)
{
	LCD_ALL_SEG_CLEAR();

	LCD_Set_Digit_Value(1,16);
	LCD_Set_Digit_Value(2,0);
	LCD_Set_Digit_Value(3,0x0C);
	LCD_Set_Digit_Value(4,0);
	LCD_Set_Digit_Value(5,16);
}

void Printf_Npair (void)
{
	LCD_ALL_SEG_CLEAR();

	LCD_Set_Digit_Value(1,16);
	LCD_Set_Digit_Value(2,25);
	LCD_Set_Digit_Value(3,0x0A);
	LCD_Set_Digit_Value(4,26);
	LCD_Set_Digit_Value(5,22);
}

void Printf_Conn (void)
{
	LCD_ALL_SEG_CLEAR();

	LCD_Set_Digit_Value(1,0x0C);
	LCD_Set_Digit_Value(2,0);
	LCD_Set_Digit_Value(3,16);
	LCD_Set_Digit_Value(4,16);
}

void Printf_Pair (void)
{
	LCD_ALL_SEG_CLEAR();

	LCD_Set_Digit_Value(1,25);
	LCD_Set_Digit_Value(2,0x0A);
	LCD_Set_Digit_Value(3,26);
	LCD_Set_Digit_Value(4,22);
}

void Printf_Nores (void)
{
	LCD_ALL_SEG_CLEAR();

	LCD_Set_Digit_Value(1,16);
	LCD_Set_Digit_Value(2,0);
	LCD_Set_Digit_Value(3,22);
	LCD_Set_Digit_Value(4,0x0E);
	LCD_Set_Digit_Value(5,23);
}

void Printf_ERR_Code (uint8_t err_code)
{
	LCD_ALL_SEG_CLEAR();

	LCD_Set_Digit_Value(1,0x0E);
	LCD_Set_Digit_Value(2,0x0C);
	LCD_SEG_DIGIT_SET_G(3);

	LCD_Set_Digit_Value(4,err_code/10);
	LCD_Set_Digit_Value(5,err_code%10);

}

void Printf_Clean (void)
{
	LCD_ALL_SEG_CLEAR();
	
	LCD_Set_Digit_Value(1,0x0C);
	LCD_Set_Digit_Value(2,24);
	LCD_Set_Digit_Value(3,0x0E);
	LCD_Set_Digit_Value(4,0x0A);
	LCD_Set_Digit_Value(5,16);
}

void Write_FirmW_Version (uint8_t version)
{
	LCD_ALL_SEG_CLEAR();
	
	LCD_Set_Digit_Value(1,0x0F);
	LCD_Set_Digit_Value(2,26);
	LCD_Set_Digit_Value(3,22);
	
	LCD_Set_Digit_Value(4,version/10);
	LCD_Set_Digit_Value(5,version%10);
}