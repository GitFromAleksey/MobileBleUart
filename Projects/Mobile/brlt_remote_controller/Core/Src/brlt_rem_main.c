/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "app_error.h"
#include "app_uart.h"
#include "app_timer.h"
#include "app_util.h"
#include "bsp.h"
#include "boards.h"
#include "nrf_delay.h"

#include "brlt_rem_rc.h"
#include "ma_ble.h"
#include "ma_updater.h"
	 
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define FOR_TESTER

#define UART_TX_BUF_SIZE        	256                                     /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE        	256                                     /**< UART RX buffer size. */

#define GET_DEV_STATUS_TIMEOUT		APP_TIMER_TICKS(1000)

APP_TIMER_DEF(m_get_status_timer);
static BRLT_RC_RF_Response_TypeDef curr_params;
uint16_t 	brlt_firmw_ver;
uint8_t		brlt_curr_params_valid = 0;

void cyclic_change_speed(BRLT_RC_RF_Response_TypeDef* params)
{
	BRLT_RC_RF_Response_TypeDef temp = *params;
	
	if(!params->UserSet.SysFlags.State)
	{	
		temp.UserSet.SysFlags.State = true;
		temp.UserSet.FanSpeed = 1;
	}
	else if (params->UserSet.FanSpeed  >= params->limits.max_fan_speed)
	{
		temp.UserSet.SysFlags.State = false;
	}
	else
	{
		temp.UserSet.FanSpeed = ++params->UserSet.FanSpeed; 
	}
	if (brlt_curr_params_valid)
		brlt_rem_send_set_params_cmd(&temp);	
}

void cyclic_change_temp(BRLT_RC_RF_Response_TypeDef* params)
{	
	BRLT_RC_RF_Response_TypeDef temp = *params;
	
	if(!params->UserSet.SysFlags.HeaterState)
	{	
		temp.UserSet.SysFlags.HeaterState = true;
		temp.UserSet.SetTemperature = 0;
	}
	else if (params->UserSet.SetTemperature  >= 40)
	{
		temp.UserSet.SysFlags.HeaterState = false;
		temp.UserSet.SetTemperature = 0;
	}
	else
	{
		temp.UserSet.SetTemperature = params->UserSet.SetTemperature + 10; 
	}
	if (brlt_curr_params_valid)
		brlt_rem_send_set_params_cmd(&temp);
}


/**@brief Function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing ASSERT call.
 * @param[in] p_file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
void uart_event_handle(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        /**@snippet [Handling data from UART] */
        case APP_UART_DATA_READY:
			
            break;

        /**@snippet [Handling data from UART] */
        case APP_UART_COMMUNICATION_ERROR:
            NRF_LOG_ERROR("Communication error occurred while handling UART.\r\n");
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            NRF_LOG_ERROR("Error occurred in FIFO module used by UART.\r\n");
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}



/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);
	
    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

static void get_status_timer_handler(void * p_context)
{	
	brlt_rem_rc_send_get_params_request();
	brlt_rem_rc_send_get_dev_status_request();
	

	NRF_LOG_INFO("Current params: speed= %d, temp= %d,"
			" work state= %d, heater state= %d, heater present = %d," 
			" firm = %x\r\n",
			curr_params.UserSet.FanSpeed,
			curr_params.UserSet.SetTemperature,
			curr_params.UserSet.SysFlags.State,
			curr_params.UserSet.SysFlags.HeaterState,
			curr_params.UserSet.SysFlags.HeaterPresent,
			brlt_firmw_ver);
	NRF_LOG_INFO("Current max speed: %d \r\n",curr_params.limits.max_fan_speed);
		
		
	
#ifdef FOR_TESTER
	NRF_LOG_INFO("Current sensors values: T_in= %d, T_out= %d \r\n",
		curr_params.SensorState.T_in,
		curr_params.SensorState.T_out);
	
	NRF_LOG_INFO("Counters. \tfan: %d, \telec: %d, \tfilter: %d \r\n",
			curr_params.SysCounters.FanTime,
			curr_params.SysCounters.WorkTime,
			curr_params.SysCounters.FilterTime);
	
#define ERR_NUMB_TOLERANCE 	3
#define ERR_FIELD_SIZE		(ERR_NUMB_TOLERANCE + sizeof(char))	
#define ERR_QUANTITY 		sizeof(BRLT_ErrorCnt_TypeDef)
		
	static char error_cnt_buffer[ERR_FIELD_SIZE * ERR_QUANTITY];
	
	for (uint8_t i = 0; i != ERR_QUANTITY; ++i)
	  sprintf(error_cnt_buffer + ERR_FIELD_SIZE*i,
			  "%*d ", ERR_NUMB_TOLERANCE, curr_params.errors.cnt.er[i]);
			  
	NRF_LOG_INFO("Errors: reg 0x%x Counters: %s\r\n", curr_params.errors.reg,
				 (uint32_t)error_cnt_buffer);
#endif	
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in] event  Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{	
	if (ma_ble_is_connect())
	{
		switch (event)
		{			
			case BSP_EVENT_KEY_3:
				cyclic_change_temp(&curr_params);
				break;
			case BSP_EVENT_KEY_2:
				cyclic_change_speed(&curr_params);		
            	break;				
			case BSP_EVENT_KEY_1:
				ma_updater_set_state(MA_UPD_PREPARE);
				NRF_LOG_INFO("Start update firmw with ver %x\r\n", 
							 ma_updater_firmw_version());
				//brlt_rem_send_test_cmd();
				break;
			case BSP_EVENT_KEY_0:
				NVIC_SystemReset();	
			break;
					
			default:
				break;
		}
	}
	else 
	{	  
		if (event == BSP_EVENT_KEY_0)
			ma_ble_start_search();
	}

}

/**@brief Function for initializing the UART. */
static void uart_init(void)
{
    ret_code_t err_code;

    const app_uart_comm_params_t comm_params =
      {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
        .baud_rate    = UART_BAUDRATE_BAUDRATE_Baud115200
      };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);

    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing buttons and leds. */
static void buttons_leds_init(void)
{
    ret_code_t err_code;
	
    err_code = bsp_init(BSP_INIT_LED|BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the timer. */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

	err_code = app_timer_create(&m_get_status_timer,
                                APP_TIMER_MODE_REPEATED,
                                get_status_timer_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the nrf log module. */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
}

/** @brief Function for the Power manager. */
static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

static void ma_event_handler (ma_ble_event_t type)
{
	uint32_t err_code;

	app_timer_stop(m_get_status_timer);
	
	switch(type)
	{
		case MA_BLE_CONNECTION:
			brlt_curr_params_valid = 0;
			brlt_rem_rc_send_get_params_request();
			brlt_rem_rc_send_get_dev_status_request();
			app_timer_start(m_get_status_timer, GET_DEV_STATUS_TIMEOUT, NULL);
			err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
			break;
		case MA_BLE_DISCONNECTION:	
			err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
			NVIC_SystemReset();
			break;			
		case MA_BLE_START_SEARCH:
			err_code = bsp_indication_set(BSP_INDICATE_SCANNING);
            APP_ERROR_CHECK(err_code);	
			break;
		case MA_BLE_FIND_DEVICE:
			err_code = bsp_indication_set(BSP_INDICATE_IDLE);
			APP_ERROR_CHECK(err_code);	
			break;
	
		default: 
			break;				
	}
}

static void ma_frame_callback (ma_ble_frame_t frame)
{
#warning "change this flag! ready ro send only if request id is correct"
	uint8_t percentage_of_firm = 0;
	static bool blink_state = true;
	
	//BRLT_RC_RF_Response_TypeDef temp_params;
	switch(frame.type)
	{	
		case SLAVE_UPDATE_REQ:
			ma_updater_set_state(MA_UPD_START);
			NRF_LOG_INFO("SLAVE_UPDATE_REQ\r\n");
			break;
		case SLAVE_UPDATE_FIRM_START_OK:	
			ma_updater_set_state(MA_UPD_CHUNK);
			NRF_LOG_INFO("MA_FIRM_UPD_START\r\n");
			break;
		case SLAVE_UPDATE_FIRM_CHUNK_OK:			
			percentage_of_firm = (1000 * ma_updater_set_state(MA_UPD_CHUNK) / 
				ma_updater_firmw_size())/10;
			
			NRF_LOG_INFO("MA_FIRM_UPD_CHUNK, firmware %% = %d \r\n",
						 percentage_of_firm);
			
			if(blink_state=!blink_state)
				bsp_board_led_on(BSP_BOARD_LED_1);
			else
				bsp_board_led_off(BSP_BOARD_LED_1);
				
			break;
		case BLE_FRAME_TYPE_DEV_STATUS_RESP:
			brlt_firmw_ver = brlt_rem_rc_get_fv_ver(frame);
			break;
		case SLAVE_UPDATE_FIRM_BURN_AND_VERIFY_OK:
			bsp_board_led_off(BSP_BOARD_LED_1);
			NRF_LOG_INFO("MA_FIRM_UPD_FINISH\r\n");
			break;
		case BLE_FRAME_TYPE_GET_MODE_RESP:
			curr_params = brlt_rem_get_params_from_frame(frame);
			brlt_curr_params_valid = 1;
			
			//if (temp_params.UserSet.SysFlags.State)
				//curr_params = temp_params;
			//else
				//curr_params.UserSet.SysFlags.State = false;
			
			break;			
		default:
			break;	
	}
	
}

int main(void)
{
	bool erase_paired = false;
    timer_init();
    uart_init();
    log_init();
    buttons_leds_init();
	
	erase_paired = bsp_button_is_pressed(2);
	  
	ma_ble_init(BRLT, erase_paired,
				ma_event_handler, ma_frame_callback);
	
	NRF_LOG_INFO("BRLT remote controller, press Button 1 to start search\r\n");
	NRF_LOG_INFO("Use Button 3 for speed control, Button 4 for temp control,"
				 " Button 2 for firmware ver %x update start\r\n", 
				 ma_updater_firmw_version());
	
	NRF_LOG_INFO("Press reset button with clamped Button 3 for bond erase");
	
    for (;;)
    {	
		ma_ble_frames_flush();
		
		if (!NRF_LOG_PROCESS())
        { 
			power_manage();
        }

    }
}
