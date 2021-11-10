/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "brc_lcd_ht16xx_driver.h"

/*Private defines*/
// clang-format off

// clang-format on
/*Private types*/
typedef struct _tag_driver_state_t
{
	uint8_t dummy;
} driver_state_t;

typedef struct _tag_driver_main_t
{
	lcd_driver_t  			interface;
	driver_state_t			driver_state;
} driver_main_t;

/*Private functions prototypes*/
static uint32_t init_lcd_driver_instance(void * p_context, void * p_raw_buff);
static uint32_t deinit_lcd_driver_instance(void * p_context);
static uint32_t write_lcd_driver_instance(void * p_context, 
										  void * p_data, uint32_t length);

static uint32_t lcd_ht_write_data(uint8_t * p_data, uint32_t length);
static uint32_t lcd_ht_write_command(uint8_t com_type);

/*Exported constants*/
const lcd_driver_m_list_t	lcd_driver_vptr = {
	.init 				= &init_lcd_driver_instance,
	.deinit 			= &deinit_lcd_driver_instance,
	.write				= &write_lcd_driver_instance,
};

static driver_main_t driver_main = {.interface.vptr = 
									&lcd_driver_vptr,};

/*Private variables*/
static uint8_t * p_lcd_data;
static uint8_t spi_tx_buffer [20];
static volatile uint8_t spi_command_tc = 0;

/*Private constants*/
static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/ 

/*Private functions*/

static void spi_master_0_event_handler(nrf_drv_spi_evt_t const * p_event)
{
   spi_command_tc = 0;
}

static uint32_t lcd_ht_write_data(uint8_t * p_data, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	char msb = 0, lsb;
	uint8_t staple_buffer = 0;
	
	spi_command_tc = 1;
	spi_tx_buffer[0] = 0xA0;

	for (int i = 0; i < ((length/2)+1); i++)
	{
		staple_buffer = ((*(p_lcd_data + (2*i)))&0xF0) |
			(((*(p_lcd_data + ((2*i)+1)))&0xF0) >> 4);
		if ( (staple_buffer & 0x01) != 0) lsb = 0x80;
		else lsb = 0x00;
		spi_tx_buffer[i+1] =  staple_buffer>>1;
		spi_tx_buffer[i+1] |= msb;
		msb = lsb;
	}
	
	ret_val = nrf_drv_spi_transfer(&m_spi_master_0,spi_tx_buffer,(LCD_DATA_SIZE/2)+2,0,0);
	
	while (spi_command_tc == 1){};
	
	return ret_val;
}

static uint32_t lcd_ht_write_command(uint8_t com_type)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	spi_command_tc = 1;
	spi_tx_buffer[0] = (com_type>>3)|0x80;
	spi_tx_buffer[1] = (com_type<<5);
	ret_val = nrf_drv_spi_transfer(&m_spi_master_0,spi_tx_buffer,2,0,0);
	while (spi_command_tc == 1){};
	
	return ret_val;
}

/*class methods section*/
static uint32_t init_lcd_driver_instance(void * p_context, void * p_raw_buff)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	p_lcd_data = (uint8_t *) p_raw_buff;
	
	nrf_drv_spi_config_t config =
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
	
	ret_val = nrf_drv_spi_init(&m_spi_master_0, &config, 
							   spi_master_0_event_handler);

	ret_val = lcd_ht_write_command(SYS_EN);
	ret_val = lcd_ht_write_command(LCD_ON);
	ret_val = lcd_ht_write_command(NORMAL_MODE);
	ret_val = lcd_ht_write_command(SET_1_3_BIAS);
	
	return ret_val;
}

static uint32_t deinit_lcd_driver_instance(void * p_context)
{
	ret_code_t ret_val = NRF_SUCCESS;
	
	ret_val = lcd_ht_write_command(LCD_OFF);
	ret_val = lcd_ht_write_command(SYS_DIS);
	nrf_drv_spi_uninit(&m_spi_master_0);
	
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
	
	return ret_val;
}

static uint32_t write_lcd_driver_instance(void * p_context, void * p_data, uint32_t length)
{
	ret_code_t ret_val = NRF_SUCCESS;
	static uint8_t * p_local_data;
	
	if (p_data != NULL)
	{
		p_local_data = (uint8_t *) p_data;
		ret_val = lcd_ht_write_data(p_local_data, length);
	}
	else
	{
		ret_val = lcd_ht_write_data(p_lcd_data, LCD_DATA_SIZE);
	}

	return ret_val;
}
/*Public functions*/
lcd_driver_t * brc_lcd_ht16xx_driver_get_interface(void)
{
	return &driver_main.interface;
}

/******************************* END_OF_FILE **********************************/