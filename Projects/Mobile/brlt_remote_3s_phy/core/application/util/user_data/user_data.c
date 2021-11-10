/**
  ******************************************************************************
  * @file    brlt_user_data.c
  * @author  Savenko S.A.
  * @version V1.0.0
  * @date    02-October-2017
  * @brief   Breezer lite flash driver for user data manage.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main_common.h"
#include <string.h>

#include "fds.h"
#include "fstorage.h"
	  

/* Private constants ---------------------------------------------------------*/
/*
Record keys should be in the range 0x0001 - 0xBFFF. 
The value 0x0000 is reserved by the system. The values from 0xC000 to 0xFFFF are
reserved for use by the Peer Manager module and can only be used in applications 
that do not include Peer Manager.

File IDs should be in the range 0x0000 - 0xBFFF. The value 0xFFFF is used by the
system. The values from 0xC000 to 0xFFFE are reserved for use by the
Peer Manager module and can only be used in applications that do not
include Peer Manager.
*/

#define USER_DATA_FIRST_RESERVED_FILE_ID	0x1100
#define USER_DATA_LAST_RESERVED_FILE_ID		0x1200
#define USER_DATA_FILE_ID					0x1100

#define USER_DATA_FIRST_RESERVED_RECORD_KEY_ID	0x2200
#define USER_DATA_LAST_RESERVED_RECORD_KEY_ID	0x2300
#define USER_DATA_BASE_REC_KEY				0x2200

#define SIZE_IN_WORDS(x)  ((sizeof(x) % sizeof(uint32_t) != 0) + \
							sizeof(x) / sizeof(uint32_t))

#define USER_DATA_CURRENT_REC_KEY	(USER_DATA_BASE_REC_KEY)
#define MAX_USER_DATA_SIZE			sizeof(ud_format_t)

typedef uint16_t rec_key_t;							
/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static ret_code_t user_data_manager_init(void * context);
static ret_code_t user_data_manager_deinit(void * context);
static ret_code_t user_data_create(void * context, ud_format_t * p_data);
static ret_code_t user_data_delete(void * context);
static ret_code_t user_data_save(void * context, ud_format_t * p_data);
static ret_code_t user_data_load(void * context, ud_format_t * p_data);
/* Exported constants --------------------------------------------------------*/
const ud_m_list_t ud_m_list = {
	.ud_init 	= user_data_manager_init,
	.ud_deinit 	= user_data_manager_deinit,
	.ud_create 	= user_data_create,
	.ud_delete 	= user_data_delete,
	.ud_read 	= user_data_load,
	.ud_write 	= user_data_save,
};

/* Private functions ---------------------------------------------------------*/

/* Assert user data migration configuration data
on the compile time */
static void assert_current_config(void)
{ 

};


static uint32_t run_garbage_collector (void)
{
	ret_code_t ret = FDS_SUCCESS;
	
	ret = fds_gc();
	if (ret != FDS_SUCCESS)
	{

	}
	return ret;
}

static ret_code_t delete_ud_records (void)
{
	ret_code_t ret = FDS_SUCCESS;

	fds_record_desc_t   record_desc;
	fds_find_token_t    ftok ={0};
	
	ret = fds_record_find(USER_DATA_FILE_ID, USER_DATA_BASE_REC_KEY, 
						  &record_desc, &ftok);	
	if(ret != FDS_SUCCESS)
		return ret;
	
	ret = fds_record_delete(&record_desc);
	if (ret != FDS_SUCCESS)
	{

	}
	return ret;
}

static ret_code_t delete_ud_file (void)
{
	ret_code_t ret = FDS_SUCCESS;
	
	ret = fds_file_delete(USER_DATA_FILE_ID);
	if (ret != FDS_SUCCESS)
	{

	}
	return ret;
}

static bool file_id_within_user_data_range(uint16_t file_id)
{
    return ((USER_DATA_FIRST_RESERVED_FILE_ID <= file_id)
         && (file_id <= USER_DATA_LAST_RESERVED_FILE_ID));
}

static bool record_key_within_user_data_range(uint16_t record_key)
{
    return ((USER_DATA_FIRST_RESERVED_RECORD_KEY_ID <= record_key)
         && (record_key <= USER_DATA_LAST_RESERVED_RECORD_KEY_ID));
}

/**@brief Event handler.
 * 
 * @param[in] p_fds_evt Event of fds system.
 *
 * 
 */
static void fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
	switch (p_fds_evt->id)
	{
		case FDS_EVT_INIT:
		{
            if (p_fds_evt->result != FDS_SUCCESS)
			{
                // Initialization failed.
            }
		}	break;
		case FDS_EVT_WRITE:
		case FDS_EVT_UPDATE:
		{
			if (file_id_within_user_data_range (p_fds_evt->write.file_id) ||
			   record_key_within_user_data_range (p_fds_evt->write.record_key))
			{
				if (p_fds_evt->result == FDS_SUCCESS){}
			}
		}	break;
		case FDS_EVT_DEL_FILE:  
		{
			if (file_id_within_user_data_range (p_fds_evt->del.file_id))
			{
				if (p_fds_evt->result == FDS_SUCCESS){}
			}
		}	break;
		case FDS_EVT_DEL_RECORD:
		{ 
			if (file_id_within_user_data_range (p_fds_evt->del.file_id) ||
			   record_key_within_user_data_range (p_fds_evt->del.record_key))
			{
				if (p_fds_evt->result == FDS_SUCCESS){}
			}
		}	break;
		case FDS_EVT_GC:
		{
			if (p_fds_evt->result == FDS_SUCCESS){}

		}	break;
		
		default:
			break;
	}
}

static ret_code_t user_data_manager_init(void * context)
{	
	assert_current_config();  
  
	ret_code_t ret = fds_register(fds_evt_handler);
	if (ret != FDS_SUCCESS)
		return ret;	
	
	ret = fds_init();
	
	return ret;		
}

static ret_code_t user_data_manager_deinit(void * context)
{	
	assert_current_config();  
	
	ret_code_t ret = delete_ud_file();
	
	return ret;		
}

static ret_code_t user_data_create(void * context, ud_format_t * p_data)
{
	static ud_format_t user_data;
	
	ret_code_t ret_val = FDS_SUCCESS;
	
	fds_record_t        record;
	fds_record_desc_t   record_desc;
	fds_record_chunk_t  record_chunk;
	
	/* Set up data */
	memcpy(&user_data, p_data, MAX_USER_DATA_SIZE);
		
	record_chunk.p_data         = (uint32_t *)&user_data;
	record_chunk.length_words   = SIZE_IN_WORDS(user_data);
	
	/* Set up record. */
	record.file_id	= USER_DATA_FILE_ID;
	record.key		= USER_DATA_CURRENT_REC_KEY;
	record.data.p_chunks	= &record_chunk;
	record.data.num_chunks	= 1;
	
	ret_val = fds_record_write(&record_desc, &record);
	switch (ret_val)
	{
		case FDS_ERR_NOT_INITIALIZED:
		case FDS_ERR_INVALID_ARG:
		{
			return ret_val;
		}	break;
		case FDS_ERR_UNALIGNED_ADDR:
		{
			return ret_val;
		}	break;
		case FDS_ERR_RECORD_TOO_LARGE:
		{
			return ret_val;
		}	break;
		case FDS_ERR_NO_SPACE_IN_FLASH:
		{
			run_garbage_collector();
			return ret_val;
		}	break;
		case FDS_SUCCESS:
		{
			return ret_val;
		}	break;
		case FDS_ERR_NO_SPACE_IN_QUEUES:
		{
			return ret_val;
		}	break;
		default:
			break;
	}
	return ret_val;
}

static ret_code_t user_data_delete(void * context)
{
	ret_code_t ret_val = FDS_SUCCESS;
	
	ret_val = delete_ud_records();

	return ret_val;	
}

static ret_code_t user_data_save(void * context, ud_format_t * p_data)
{
	static ud_format_t user_data;
	
	uint32_t ret_val = FDS_SUCCESS;
	
	fds_record_t        record;
	fds_record_chunk_t  record_chunk;
	fds_record_desc_t   record_desc;
	/*Important, make sure you zero init the ftok token*/
	fds_find_token_t    ftok ={0};
	
	ret_val = fds_record_find(USER_DATA_FILE_ID, USER_DATA_CURRENT_REC_KEY,
						  &record_desc, &ftok);	
	if(ret_val != FDS_SUCCESS)
		return ret_val;
	
	/* Set up data */
	memcpy(&user_data, p_data, MAX_USER_DATA_SIZE);
		
	record_chunk.p_data         = (uint32_t *)&user_data;
	record_chunk.length_words   = SIZE_IN_WORDS(user_data);
	
	/* Set up record. */
	record.file_id	= USER_DATA_FILE_ID;
	record.key		= USER_DATA_CURRENT_REC_KEY;
	record.data.p_chunks	= &record_chunk;
	record.data.num_chunks	= 1;
	
	ret_val = fds_record_update(&record_desc, &record);
	switch (ret_val)
	{
		case FDS_ERR_NOT_INITIALIZED:
		case FDS_ERR_INVALID_ARG:
		{
			return ret_val;
		}	break;
		case FDS_ERR_UNALIGNED_ADDR:
		{
			return ret_val;
		}	break;
		case FDS_ERR_RECORD_TOO_LARGE:
		{
			return ret_val;
		}	break;
		case FDS_ERR_NO_SPACE_IN_FLASH:
		{
			run_garbage_collector();
			return ret_val;
		}	break;
		case FDS_SUCCESS:
		{
			return ret_val;
		}	break;
		case FDS_ERR_NO_SPACE_IN_QUEUES:
		{
			return ret_val;
		}	break;
		default:
			break;
	}
	return ret_val;
};

static ret_code_t user_data_load(void * context, ud_format_t * p_data)
{
	ret_code_t ret;
	fds_flash_record_t  flash_record;
	fds_record_desc_t   record_desc;
	/*Important, make sure you zero init the ftok token*/
	fds_find_token_t    ftok ={0};
	
	ret = fds_record_find_in_file(USER_DATA_FILE_ID, 
						  &record_desc, &ftok);	
	if(ret != FDS_SUCCESS)
		return ret;

	ret = fds_record_open(&record_desc, &flash_record);
	if ( ret != FDS_SUCCESS)
		return ret;
	/* Convert data to actual format */
	memcpy(p_data, flash_record.p_data, MAX_USER_DATA_SIZE);

	/* Close the record when done. */
	ret = fds_record_close(&record_desc);

	return ret;
};