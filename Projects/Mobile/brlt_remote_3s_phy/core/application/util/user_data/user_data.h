/**
  ******************************************************************************
  * @file    user_data.h
  * @author  Savenko S.A., Sorokin N.N.
  * @version V1.1.0
  * @date    08-December-2017
  * @brief   Flash driver for user data manage.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_DATA_H
#define __USER_DATA_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct  _tag_ud_format_t
{
	uint32_t	dev_id;
} ud_format_t;

typedef ret_code_t (*ud_init_t)(void *);
typedef ret_code_t (*ud_io_proc_t)(void *, ud_format_t *);
typedef ret_code_t (*ud_data_proc_t)(void *);

typedef struct _tag_ud_m_list_t
{
	ud_init_t			ud_init;
	ud_init_t			ud_deinit;
	ud_io_proc_t		ud_create;
	ud_data_proc_t		ud_delete;
	ud_io_proc_t		ud_read;
	ud_io_proc_t		ud_write;
} ud_m_list_t;

/* Exported constants --------------------------------------------------------*/
extern const ud_m_list_t	ud_m_list;
/* External variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**@brief Init of module. Must be called after SD init!
 *
 *
 *
 * @returns FDS error code, if != 0, operation not perfomed.
 * 
 */


/**@brief Delete all user data from flash
 * @returns FDS error code, if != 0, operation not perfomed.
 * 
 */


/**@brief Save of user data.
 * @param[in] params Params struct of BRLT.
 *
 * @returns FDS error code, if != 0, operation not perfomed.
 * 
 */


/**@brief Load of user data. Always loading 
 * the last saved data.
 * @param[in] params Params struct of BRLT.
 *
 * @returns FDS error code, if != 0, operation not perfomed.
 * 
 */

#endif  /*__BRLT_FLASH_H*/

/***************************************************************END OF FILE****/