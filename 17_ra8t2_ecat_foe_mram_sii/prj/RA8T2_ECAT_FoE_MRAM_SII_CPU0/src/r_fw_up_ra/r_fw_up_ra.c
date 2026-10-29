/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2026 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_fw_up_ra.c
 * Version      : 1.00
 * Description  : Firmware update 
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 10.04.2026 1.00     First Release
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 Includes <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/* Access to peripherals and board defines. */
#include "hal_data.h"

/* Defines for Firmware update support */
#include "r_fw_up_ra_if.h"
#include "r_fw_up_buf.h"
#include "r_fw_up_ra_private.h"

/*******************************************************************************
Imported global variables and functions (from other files)
*******************************************************************************/
extern void handle_error(fsp_err_t err);
/***********************************************************************************************************************
 Global variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Private variables and functions
 ***********************************************************************************************************************/
static bool is_opened = false;


static fw_up_return_t write_firmware(fw_up_fl_data_t *p_fl_data);
static fw_up_return_t fw_up_put_data(fw_up_fl_data_t *p_fl_data);
static fw_up_return_t fw_up_get_data(fw_up_fl_data_t *p_fl_data);

/***********************************************************************************************************************
 * Function Name: fw_up_open
 * Description  : Initialize firmware update function.
 * Arguments    : none
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_OPENED -
 *                   fw_up_open has already been run.
 ***********************************************************************************************************************/
fw_up_return_t fw_up_open(void)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    /* Check that the fw_up_open() has been executed. */
    if (true == is_opened)
    {
        return FW_UP_ERR_OPENED;
    }

    /* Initialize variable for store address of allocated memory */
    fw_up_memory_init();

    /* Initialize ring buffer */
    fw_up_buf_init();

    /* Set initialize complete flag */
    is_opened = true;

    return ret;
} /* End of function fw_up_open() */

/***********************************************************************************************************************
 * Function Name: fw_up_close
 * Description  : Initialize firmware update function.
 * Arguments    : none
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                   fw_up_open has not been run.
 ***********************************************************************************************************************/
fw_up_return_t fw_up_close(void)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    /* Check that the fw_up_open() has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Initialize ring buffer */
    fw_up_buf_init();

    /* Clear initialize complete flag */
    is_opened = false;

    return ret;
} /* End of function fw_up_close() */

/***********************************************************************************************************************
 * Function Name: analyze_and_write_data
 * Description  : Analyze mot s format and program data.
 * Arguments    : *p_recv_data -
 *                   mot file data.
 *                data_size
 *                   mot file data size.m
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_WRITE -
 *                   Flash API(erase) error.
 ***********************************************************************************************************************/
fw_up_return_t analyze_and_write_data(const uint8_t *p_recv_data, uint32_t data_size)
{
    fw_up_return_t  ret1;
    fw_up_return_t  ret2;
    fw_up_return_t  ret3;
    fw_up_fl_data_t fl_data;
    fw_up_fl_data_t bin_data;
    int32_t status;

    /* Put received data in buffer */
    /* Casting is valid because the pointer value is 32 bits and the reference destination of the buffer is never changed */
    fl_data.src_addr = (uint32_t)p_recv_data;
    fl_data.len = data_size;
    fl_data.count = 0;

    do
    {
        /* Put received data in buffer */
        ret1 = fw_up_put_data(&fl_data);
        if (FW_UP_ERR_INVALID_RECORD == ret1)
        {
            return ret1;
        }
        ret3 = FW_UP_SUCCESS;
        do
        {
            /* Get Pointer to firmware data and data size, write address. */
            ret2 = fw_up_get_data(&bin_data);
            if (FW_UP_SUCCESS == ret2)
            {
                /* Check write address  */
                if (fw_up_check_addr_value((uint32_t *)&(bin_data.dst_addr)) == true)
                {
					/* Write firmware to the bank */
					ret3 = write_firmware(&bin_data);
					if (FW_UP_SUCCESS == ret3)
					{
						/* Casting is valid because address values are converted to pointers */
						status = FWUPMEMCMP((int8_t*)(bin_data.dst_addr), (int8_t*)bin_data.src_addr, bin_data.len);
						
						if (0 != status)
						{
							ret3 = FW_UP_ERR_WRITE;
						}
					}
					else
					{
						ret3 = FW_UP_ERR_WRITE;
					}
                }
            }
        }
        while ((FW_UP_SUCCESS == ret3) && (FW_UP_SUCCESS == ret2));
    }
    while (FW_UP_SUCCESS != ret1);

    return ret3;
} /* End of function analyze_and_write_data() */

/***********************************************************************************************************************
 * Function Name: fw_up_addr_check_addr_value
 * Description  : The address is checked valid.
 * Arguments    : none
 * Return Value : true -
 *                   Valid.
 *                false -
 *                   Invalid.
 ***********************************************************************************************************************/
bool fw_up_check_addr_value(uint32_t *p_addr_value)
{
    bool ret = false;

	if(*p_addr_value != FW_UP_BLANK_VALUE)
	{
		if((*p_addr_value >= FW_UP_SUA0_ADDR && (FW_UP_SUA1_ADDR) > *p_addr_value))
		{
			*p_addr_value += FW_UP_SUA0_SIZE;
			ret = true;
		}

#if defined(MRAM_BANK0)
		if ((*p_addr_value >= FW_UP_BANK1_ADDR && (FW_UP_BANK1_ADDR + FW_UP_APPLI_SIZE) > *p_addr_value))
		{
			ret = true;
		}
#elif defined(MRAM_BANK1)
        if ((*p_addr_value >= FW_UP_BANK0_ADDR && (FW_UP_BANK0_ADDR + FW_UP_APPLI_SIZE) > *p_addr_value))
        {
            ret = true;
        }
#endif
	}

    return ret;

} /* End of function fw_up_check_addr_value */

/***********************************************************************************************************************
 * Function Name: write_firmware
 * Description  : Write firmware data.
 * Arguments    : p_fl_data -
 *                   Pointer to binary data of initial address, write address, data size.
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                   R_FW_UP_Open has not been run.
 *                FW_UP_ERR_NULL_PTR -
 *                   The argument p_fl_data is null pointer.
 *                FW_UP_ERR_INTERNAL -
 *                   Flash API error.
 ***********************************************************************************************************************/
static fw_up_return_t write_firmware(fw_up_fl_data_t *p_fl_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;
	fsp_err_t err = FSP_SUCCESS;

	uint32_t i, j, write_size, remain_size;

    /* Check that the fw_up_open has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Make sure that the argument is correct. */
    /* To cast in order to compare the address. There is no problem because the information is not lost even if the
     *  cast. */
    if (NULL == p_fl_data)
    {
        return FW_UP_ERR_NULL_PTR;
    }
    /* Write firmware data */
	j = (p_fl_data->len / FW_UP_WRITE_ATONCE_SIZE);

	remain_size = p_fl_data->len;
	for ( i = 0; i < j ; i++)
	{
		if(remain_size >= FW_UP_WRITE_ATONCE_SIZE)
		{
			write_size = FW_UP_WRITE_ATONCE_SIZE;
			remain_size -= FW_UP_WRITE_ATONCE_SIZE;
		}
		else
		{
			write_size = remain_size;
		}
		err = R_MRAM_Write(&g_mram0_ctrl,
						   (uint32_t *)(p_fl_data->src_addr + (i * FW_UP_WRITE_ATONCE_SIZE)),
						   (uint32_t *)(p_fl_data->dst_addr + (i * FW_UP_WRITE_ATONCE_SIZE)),
						   write_size);
		if (FSP_SUCCESS != err)
		{
			break;
		}
	}

    if (FSP_SUCCESS != err)
    {
        ret = FW_UP_ERR_INTERNAL;
    }

    return ret;

} /* End of function write_firmware() */


/***********************************************************************************************************************
 * Function Name: fw_up_put_data
 * Description  : Put in firmware data to ring buffer
 * Arguments    : *p_fl_data -
 *                    Pointer to initial address and data size of firmware data.
 * Return Value : FW_UP_SUCCESS -
 *                    Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                    fw_up_open has not been run.
 *                FW_UP_ERR_NULL_PTR -
 *                    The argument p_fl_data is null pointer.
 *                FW_UP_ERR_RING_BUF_FULL -
 *                    Ring buffer is not empty.
 *                FW_UP_ERR_INVALID_RECORD -
 *                    Motorola S record format data is invalid.
 *                FW_UP_ERR_INTERNAL -
 *                    Memory allocation is failed.
 ***********************************************************************************************************************/
static fw_up_return_t fw_up_put_data(fw_up_fl_data_t *p_fl_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;
    uint16_t cnt;
    uint8_t *pdata_tmp;

    /* Check that the fw_up_open() has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Make sure that the argument is correct. */
    /* To cast in order to compare the address. There is no problem because the information is not lost even if the
     *  cast. */
    if (NULL == p_fl_data)
    {
        return FW_UP_ERR_NULL_PTR;
    }

    /* Set initial address of firmware data to be put in ring buffer */
    pdata_tmp = ((uint8_t *)p_fl_data->src_addr) + p_fl_data->count;

    for (cnt = p_fl_data->count; cnt < p_fl_data->len; cnt++)
    {
        /* Put firmware data in ring buffer */
        ret = fw_up_put_mot_s(*pdata_tmp);

        if (FW_UP_SUCCESS != ret)
        {
            /* Set values of data size to be put in ring buffer */
            p_fl_data->count = cnt;

            return ret;
        }

        pdata_tmp++;
    }

    return ret;

} /* End of function fw_up_put_data() */


/***********************************************************************************************************************
 * Function Name: fw_up_get_data
 * Description  : Stores firmware update data to ring buffer
 * Arguments    : *p_fl_data -
 *                   Pointer to binary data of initial address.
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                   fw_up_open has not been run.
 *                FW_UP_ERR_NULL_PTR -
 *                   The argument p_fl_data is null pointer.
 *                FW_UP_ERR_RING_BUF_EMPTY -
 *                   Ring buffer is empty.
 ***********************************************************************************************************************/
static fw_up_return_t fw_up_get_data(fw_up_fl_data_t *p_fl_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    /* Check that the R_FW_UP_Open has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Make sure that the argument is correct. */
    /* To cast in order to compare the address. There is no problem because the information is not lost even if the
     *  cast. */
    if (NULL == p_fl_data)
    {
        return FW_UP_ERR_NULL_PTR;
    }

    /* Get initial address and write address, data size of firmware data */
    ret = fw_up_get_binary(p_fl_data);

    return ret;
} /* End of function fw_up_get_data() */

/***********************************************************************************************************************
 * Function Name: fw_up_memcmp
 * Description  : memory compare
 * Arguments    : *p_dst - 
 *                   Pointer to data of destination address.
 *                *p_src - 
 *                   Pointer to data of source address.
 *                len - 
 *                   length of data copmpare
 * Return Value : 0 - memaory data match , 1 or -1 - memory data not match
 ***********************************************************************************************************************/
int32_t fw_up_memcmp(int8_t *p_dst, int8_t *p_src, uint32_t len)
{
	int32_t	ret = 0;
	uint32_t i;
    volatile uint8_t dummy_read;
    uint32_t dst, src;

	for (i = 0; i < len; i++)
	{
        dst = (uint32_t)(*p_dst);
        src = (uint32_t)(*p_src);
		if (dst < src)
		{
			
			ret = -1;
			break;
		}
		else  if (dst > src)
		{
			ret = 1;
			break;
		}
		else
		{
			/* Do nothing! */
		}
        /* If least significant 2 bits of p_dst address are 0x02, put dummy read sequence to deactivate unintentional Continuous Read Mode */
		if (((uint32_t)p_dst & 0x03) == 0x02)
		{
            dummy_read = *(uint8_t *)(FW_UP_BANK1_ADDR);
		}
		p_src++;
		p_dst++;
	}
	(void) dummy_read;
	return ret;
} /* End of fw_up_memcmp */

/***********************************************************************************************************************
 * Function Name: fw_up_memcpy
 * Description  : memory copy
 * Arguments    : *p_dst - 
 *                   Pointer to data of destination address.
 *                *p_src - 
 *                   Pointer to data of source address.
 *                len - 
 *                   length of data copy
 * Return Value : none
 ***********************************************************************************************************************/
void fw_up_memcpy(uint8_t *p_dst, uint8_t *p_src, uint32_t len)
{
	uint32_t i;
    volatile uint8_t dummy_read;

	for (i = 0; i < len; i++)
	{
		*p_dst = *p_src;

        /* If least significant 2 bits of p_dst address are 0x02, put dummy read sequence to deactivate unintentional Continuous Read Mode */
		if (((uint32_t)p_dst & 0x03) == 0x02)
		{
            dummy_read = *(uint8_t *)(FW_UP_BANK1_ADDR);
		}
		p_src++;
		p_dst++;
	}
	(void) dummy_read;

} /* End of fw_up_memcpy */

