/*******************************************************************************
*
*
*
*
*******************************************************************************/
/*Includes*/
#include "remote_protocol.h"
#include "device_interface.h"
/*Private defines*/
// clang-format off

// clang-format on
/*Private types*/

/*Private functions prototypes*/
static uint32_t init_protocol_instance(void * p_context);
static uint32_t deinit_protocol_instance(void * p_context);
static uint32_t input_data_handler(void * p_context, uint32_t length);
static uint32_t output_data_handler(void * p_context, uint32_t length);
static uint32_t input_raw_data_handler(void * p_context, uint32_t length);
static uint32_t output_raw_data_handler(void * p_context, uint32_t length);
/*Exported constants*/
const protocol_m_list_t	remote_protocol_vptr = {
	.init 				= &init_protocol_instance,
	.deinit 			= &deinit_protocol_instance,
	.input_data 		= &input_data_handler,
	.output_data 		= &output_data_handler,
	.input_raw_data 	= &input_raw_data_handler,
	.output_raw_data 	= &output_raw_data_handler,
};
/*Private variables*/
/*static uint32_t adv_data_parser (uint32_t pair_mode,
				 ble_gap_evt_adv_report_t * p_adv_data);*/
/*Private macro*/
// clang-format off

// clang-format on
/*Private constants*/

/*Private functions*/
/*static uint32_t adv_data_parser (uint32_t pair_mode,
				 ble_gap_evt_adv_report_t * p_adv_data)
{
	
	return NRF_SUCCESS;
}*/

/*class methods section*/

static uint32_t init_protocol_instance(void * p_context)
{
	return NRF_SUCCESS;
}

static uint32_t deinit_protocol_instance(void * p_context)
{
	return NRF_SUCCESS;
}
 
static uint32_t input_data_handler(void * p_context, uint32_t length)
{
	return NRF_SUCCESS;	
}

static uint32_t output_data_handler(void * p_context, uint32_t length)
{
	return NRF_SUCCESS;
}

static uint32_t input_raw_data_handler(void * p_context, uint32_t length)
{
	return NRF_SUCCESS;
}

static uint32_t output_raw_data_handler(void * p_context, uint32_t length)
{
	return NRF_SUCCESS;
}
/*Public functions*/
ble_adv_data_parser_t remote_get_adv_parser (void)
{
	return NULL;//&adv_data_parser;
}
/******************************* END_OF_FILE **********************************/