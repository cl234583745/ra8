/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
\addtogroup ESM EtherCAT State Machine
@{
*/

/**
\file bootmode.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 5.12

<br>Changes to version V4.20:<br>
V5.12 BOOT2: call BL_Start() from Init to Boot<br>
<br>Changes to version - :<br>
V4.20: File created
*/

/*--------------------------------------------------------------------------------------
------
------    Includes
------
--------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "ecatfoe.h"
#include "ecat_def.h"
#include "ecatslv.h"
#include "mailbox.h"
#include "ecatappl.h"
#include "foeappl.h"
#include "sampleappl.h"
#include "renesashw.h"

#define    _BOOTMODE_ 1
#include "bootmode.h"
#undef _BOOTMODE_

#ifdef __ICCARM__
#include "intrinsics.h"												// intrinsic functions header
#endif // __iccarm__

/* Access to Firmware updata API */
#include "hal_data.h"
#include "r_fw_up_ra_if.h"

extern void handle_error(fsp_err_t err);

/*--------------------------------------------------------------------------------------
------
------    local Types and Defines
------
--------------------------------------------------------------------------------------*/
#define BL_WRITE_BUFFER_SIZE		FW_UP_PAGE_SIZE		// Byte
#define BL_DATA_STATUS_IDLE			(0)					// Idle
#define BL_DATA_STATUS_WRITE		(1)					// Data write to Flash

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
static UINT8  DataStatus = BL_DATA_STATUS_IDLE;
static BOOL   bReBoot;

/*-----------------------------------------------------------------------------------------
------
------    Module internal function declarations
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    Module internal variable definitions
------
-----------------------------------------------------------------------------------------*/
BSP_DONT_REMOVE const uint32_t g_identify[4] BSP_PLACE_IN_SECTION(".identify") = {(VENDOR_ID), (PRODUCT_CODE), (REVISION_NUMBER), (SERIAL_NUMBER)};

/******************************************************************************
* Function Name: BL_Start
* Description  : Boot Loader start function
* Arguments    : State  -- Current State
* Return Value : None
******************************************************************************/
void BL_Start( UINT8 State)
{
	FSP_PARAMETER_NOT_USED(State);

} /* BL_Start() */

/******************************************************************************
* Function Name: BL_Stop
* Description  : Called in the state transition from BOOT to Init
* Arguments    : None
* Return Value : None
******************************************************************************/
void BL_Stop(void)
{
}

/******************************************************************************
* Function Name: BL_StartDownload
* Description  : File download start function
* Arguments    : password -- download password
* Return Value : None
******************************************************************************/
void BL_StartDownload(UINT32 password)
{
	fw_up_return_t status;

	status = fw_up_open();					// Initialize firmware update function
	handle_error((fsp_err_t)status);

	DataStatus = BL_DATA_STATUS_WRITE;

	FSP_PARAMETER_NOT_USED(password);
} /* BL_StartDownload() */

/******************************************************************************
* Function Name: BL_Data
* Description  : File data receive function
* Arguments    : pData -- Data pointer
*              : Size  -- Data Length
* Return Value : FoE error code
******************************************************************************/
EEPBUFFER     Buffer;

UINT16 BL_Data(UINT16 *pData,UINT16 Size)
{
	UINT16 ErrorCode = 0;
	UINT8  LastData;
	UINT32 i;

	fw_up_return_t status;

    volatile UINT32 *pIdentify = (UINT32 *)(FW_UP_SUA1_ADDR + FW_UP_APPLI_ID_OFFSET);    // Identify section address

	switch(DataStatus)
	{
	case BL_DATA_STATUS_WRITE:
		LastData = (Size != (u16ReceiveMbxSize - MBX_HEADER_SIZE - FOE_HEADER_SIZE));
		status = analyze_and_write_data((const uint8_t *)pData, (uint32_t)Size);	// data copy to write buffer and write to flash
		handle_error((fsp_err_t)status);
		if(LastData == TRUE)											// last receive data ?
		{
			//--------------------------------------------------
			// SII update, update firmware Revision Number
			//--------------------------------------------------
			for ( i = 0; i < 4 ; i++)											// get new firmware identify 
			{
				Buffer.dword[i] = *pIdentify++;
			}
			ESC_EepromAccess(SII_EEP_IDENTIFY_OFFSET + SII_EEP_REVESIONNO, 2, &Buffer.word[SII_EEP_REVESIONNO], ESC_WR);
			fw_up_close();
			BL_SetRebootFlag(TRUE);												// yes. reboot is available.
		}
		break;
	case BL_DATA_STATUS_IDLE:
	default:
		break;
	}

	return(ErrorCode);
} /* BL_Data() */

/******************************************************************************
* Function Name: BL_SetRebootFlag
* Description  : Reboot flag set function
* Arguments    : Flag -- TRUE/FALSE
* Return Value : None
******************************************************************************/
void BL_SetRebootFlag(BOOL Flag)
{
	bReBoot = Flag;
}

/******************************************************************************
* Function Name: BL_CheckRebootFlag
* Description  : Check reboot flag function
* Arguments    : None
* Return Value : Flag
******************************************************************************/
BOOL BL_CheckRebootFlag(void)
{
	return(bReBoot);
}


/******************************************************************************
* Function Name: BL_Reboot
* Description  : Reboot boot loader function
* Arguments    : None
* Return Value : None
******************************************************************************/
void BL_Reboot(void)
{
	if(R_MRMS->MSUASMON_b.BTFLG == 1)
	{
		/* Current STARTUP_AREA_BLOCK0 */
		R_MRAM_StartUpAreaSelect(&g_mram0_ctrl, FLASH_STARTUP_AREA_BLOCK1, false);
	}
	else
	{
		/* Current STARTUP_AREA_BLOCK1 */
		R_MRAM_StartUpAreaSelect(&g_mram0_ctrl, FLASH_STARTUP_AREA_BLOCK0, false);
	}
	NVIC_SystemReset();	// System Software Reset
	while(1)
	{
		/* Do nothing */
	};
	
} /* BL_Reboot() */

