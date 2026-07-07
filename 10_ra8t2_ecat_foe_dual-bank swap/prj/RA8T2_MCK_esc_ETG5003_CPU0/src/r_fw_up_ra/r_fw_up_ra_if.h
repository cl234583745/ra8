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
 * File Name    : r_fw_up_ra_if.h
 * Version      : 1.00
 * Description  : Firmware update API interface file
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 09.04.2026 1.00     First Release
 ***********************************************************************************************************************/

#ifndef R_FW_UP_RA_IF_H_
#define R_FW_UP_RA_IF_H_

/***********************************************************************************************************************
 Includes <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include "hal_data.h"
#include "r_fw_up_ra_feature.h"
/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
/* Flash specification */
#define FW_UP_PAGE_SIZE         (256)           /* Page size of serial flash memory */

/* Flash driver WRITE API restriction */
#define FW_UP_WRITE_ATONCE_SIZE BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES // Byte

/* Flash Mapping */
#define FW_UP_PRV_CMRAM_START_ADDR  (0x02000000)	                                /* Code MRAM start address */
#define FW_UP_SUA0_SIZE             (8192)                                          /* Startup area0 size = 8KB */
#define FW_UP_SUA1_SIZE             (8192)                                          /* Startup area1 size = 8KB */
#define FW_UP_PRV_SEMI_PRM_SIZE     (4096)											/* Paramter backup for SEMI = 4KB */
#define FW_UP_PRV_RESERVED_SIZE     (FW_UP_SUA0_SIZE + FW_UP_SUA1_SIZE + FW_UP_PRV_SEMI_PRM_SIZE)

#define FW_UP_APPLI_SIZE        ((BSP_ROM_SIZE_BYTES - FW_UP_PRV_RESERVED_SIZE)/2)  /* User apllicaiotn size 502KB (1024-20)/2 */
#define FW_UP_SUA0_ADDR         FW_UP_PRV_CMRAM_START_ADDR                          /* Startup area 0 (+0KB)  */
#define FW_UP_SUA1_ADDR         (FW_UP_PRV_CMRAM_START_ADDR + FW_UP_SUA0_SIZE)      /* Startup area 1 (+8KB)  */
#define FW_UP_BANK0_ADDR        (FW_UP_PRV_CMRAM_START_ADDR + FW_UP_PRV_RESERVED_SIZE)  /* BANK0 user applicaiotn (+20KB) */
#define FW_UP_BANK1_ADDR        (FW_UP_BANK0_ADDR + FW_UP_APPLI_SIZE)               /* BANK1 user applicaiotn (+522KB) */


#define FW_UP_APPLI_ID_OFFSET   (0x00000080)    /* Identify table is placed after vector tables */



/* The value of blank of code flash */
#define FW_UP_BLANK_VALUE       (0xFFFFFFFFu)

/* Enable EEPROM revision no. check */
#ifndef FW_UP_ENABLE_REVNO_CHECK
#define FW_UP_ENABLE_REVNO_CHECK    (0)                 // default disable
#endif

#ifndef FWUPMEMCPY
#if (FW_UP_FLASH_TYPE == 1)
#define FWUPMEMCPY                                    fw_up_memcpy
#else
#define FWUPMEMCPY                                    memcpy
#endif
#endif

#ifndef FWUPMEMCMP
#if (FW_UP_FLASH_TYPE == 1)
#define FWUPMEMCMP                                    fw_up_memcmp
#else
#define FWUPMEMCMP                                    memcmp
#endif
#endif



/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/
/* Firmware update API error codes */
typedef enum e_fw_up_return_t
{
    FW_UP_SUCCESS, /* Processing completed successfully */
    FW_UP_ERR_OPENED, /* Firmware update API initialized. R_FW_UP_Open has been run. */
    FW_UP_ERR_NOT_OPEN, /* Firmware update API uninitialized. R_FW_UP_Open has not been run. */
    FW_UP_ERR_NULL_PTR, /* Value of the pointer is NULL or FIT_NO_PTR */
    FW_UP_ERR_INVALID_RECORD, /* Record format is invalid */
    FW_UP_ERR_BUF_FULL, /* Buffer is filled */
    FW_UP_ERR_BUF_EMPTY, /* Buffer is empty */
    FW_UP_ERR_INITIALIZE, /* Flash API open error */
    FW_UP_ERR_ERASE, /* Erase error */
    FW_UP_ERR_WRITE, /* Write error */
    FW_UP_ERR_INTERNAL, /* Module internal error is detected. */

} fw_up_return_t;

/* Structure of the write data */
typedef struct st_fw_up_fl_data_t
{
    uint32_t src_addr; /* Address of write buffer containing the data to write to flash */
    uint32_t dst_addr; /* Address of ROM to write */
    uint32_t len; /* Number of bytes contained in the write buffer specified with arc_addr */
    uint16_t count; /* Number of bytes put in the buffer */
} fw_up_fl_data_t;

/***********************************************************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ***********************************************************************************************************************/
fw_up_return_t fw_up_open(void);
fw_up_return_t fw_up_close(void);
fw_up_return_t analyze_and_write_data(const uint8_t *p_recv_data, uint32_t data_size);
bool fw_up_check_addr_value(uint32_t *addr);

int32_t fw_up_memcmp(int8_t *p_dst, int8_t *p_src, uint32_t len);
void fw_up_memcpy(uint8_t *p_dst, uint8_t *p_src, uint32_t len);

#endif /* R_FW_UP_RZ_IF_H_ */
