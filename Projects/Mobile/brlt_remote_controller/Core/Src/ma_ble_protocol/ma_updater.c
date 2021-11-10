#include "ma_updater.h"

#include "crc_calculate.h"
#include "string.h"
//include here a array of firmware
#include "Brlt.bin.h"

#define CHUNK_SIZE							512 //if 1024 may be falls
#define CRC_SIZE							2

const uint8_t* firmware = BrLt_bin;
uint32_t firmware_size = sizeof BrLt_bin;

uint32_t ma_updater_firmw_size(void)
{
	return firmware_size;
}

uint16_t ma_updater_firmw_version(void)
{
	return *((uint16_t*)(firmware + 0x208));
}

uint32_t ma_updater_set_state(ma_updater_state_t state)
{
	static uint8_t reboot_data[7] = {0};
	static uint8_t start_data[132] = {0};
	static uint8_t chunck_data[CHUNK_SIZE + 4] = {0};
	static ma_updater_state_t prev_st = MA_UPD_IDLE;
	static uint32_t offset = 0;

	ma_ble_frame_t frame = {0};
	uint32_t request_id = 0x01;
	uint16_t file_crc = 0;	

	*(uint32_t*)start_data = (uint32_t)(firmware_size + 128 + CRC_SIZE);//this is file len
	memset(start_data + 4, 0xDE, sizeof start_data - 4);//file may consist app + crc + softdev
	
	if(state != prev_st + 1 && state != MA_UPD_CHUNK)
		frame.type = 0;
	
	prev_st = state;
	frame.request_id = request_id;
	switch(state)
	{
		case MA_UPD_IDLE:
			offset = 0;
			break;
			
		case MA_UPD_PREPARE:
			offset = 0;
			frame.type = SLAVE_UPDATE_REBOOT_FOR_UPDATE;
			frame.data = reboot_data;
			frame.data_size = sizeof(reboot_data);
			ma_ble_put_frame_in_queue(frame);
			break;
			
		case MA_UPD_START:	
			frame.type = SLAVE_UPDATE_START;
			frame.data = start_data;
			frame.data_size = sizeof(start_data);
			ma_ble_put_frame_in_queue(frame);
			break;
			
		case MA_UPD_CHUNK:
			
			if(offset < firmware_size)
			{	
				*(uint32_t*)chunck_data = offset;
				memcpy(chunck_data + 4, firmware + offset, CHUNK_SIZE);
				
				frame.type = SLAVE_UPDATE_FIRM_CHUNK;
				frame.data = chunck_data;
				
				if((offset + CHUNK_SIZE) > firmware_size)
				{
					frame.data_size = firmware_size - offset + 4;
					offset = firmware_size;			
				}
				else
				{	
					frame.data_size = CHUNK_SIZE + 4;
					offset += CHUNK_SIZE;
				}			
			}
			else if(offset != MA_UDP_END_OF_FIRMWARE)
			{		
				*(uint32_t*)chunck_data = offset;
				file_crc = CRC_CCITT(0xFFFF,firmware,firmware_size,0);
				*(chunck_data + 4) = ((uint8_t*)&file_crc)[1];
				*(chunck_data + 5) = ((uint8_t*)&file_crc)[0];
				
				frame.type = SLAVE_UPDATE_FIRM_CHUNK;
				frame.data = chunck_data;
				frame.data_size = CRC_SIZE + 4;
				offset = MA_UDP_END_OF_FIRMWARE;
			}
			else
			{	
				frame.type = SLAVE_UPDATE_UPDATE;
				frame.data = start_data;
				frame.data_size = sizeof(start_data);
				prev_st = MA_UPD_IDLE;
			}
			ma_ble_put_frame_in_queue(frame);
			break;		
		
		default:
			break;		
	}
	
	return offset;
}
