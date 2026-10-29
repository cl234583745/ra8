/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
\addtogroup SampleFoe FoE Sample functions
@{
*/

/**
\file samplefoe.c
\author EthercatSSC@beckhoff.com
\brief Implementation

*/


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "applInterface.h"
#include "bootmode.h"
#define _FOE_SAMPLE_APPLICATION_
#include "samplefoe.h"
#undef _FOE_SAMPLE_APPLICATION_
#include "r_fw_up_ra_if.h"
/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/
/*ECATCHANGE_START(V5.12) FOE1*/
#define    MAX_FILE_NAME_SIZE    32

/** \brief  MAX_FILE_SIZE: Maximum file size */
#define MAX_FILE_SIZE                             0x180
/*ECATCHANGE_END(V5.12) FOE1*/

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
/*ECATCHANGE_START(V5.12) FOE1*/
UINT32           nFileWriteOffset;
CHAR             aFileName[MAX_FILE_NAME_SIZE];
UINT8 MBXMEM      aFileData[MAX_FILE_SIZE];


UINT32 u32FileSize;
UINT32 ReadDataAddr;	// Read data address for uploading
#if defined(BOARD_RA8T2_MCK) | defined(BOARD_RA8T2_EK)
    #if defined(MRAM_BANK0)
    const UINT16 HUGE aFileNameHeader[8] = {0x4345,0x5441,0x5746,0x525F,0x3841,0x3254,0x425F,0x3130}; // "ECATFW_RA8T2_B01"
    #elif defined(MRAM_BANK1)
    const UINT16 HUGE aFileNameHeader[8] = {0x4345,0x5441,0x5746,0x525F,0x3841,0x3254,0x425F,0x3030}; // "ECATFW_RA8T2_B00"
    #endif
#endif
const UINT32      aFilePassword = 0x00000000;

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/
/*ECATCHANGE_START(V5.12) FOE1*/
UINT16 FoE_Read(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password, UINT16 maxBlockSize, UINT16 *pData)
{
    UINT32 sizeError = 0;

 	UINT8 *pReadData;

	/* initialize file size */
	u32FileSize = FW_UP_APPLI_SIZE;
    
	/* file name size check */
    if ((nameSize + 1) > MAX_FILE_NAME_SIZE)
    {
        return ECAT_FOE_ERRCODE_DISKFULL;
    }
	/* file name header matching  check */
    if ((pName[0] == aFileNameHeader[0])
        && (pName[1] == aFileNameHeader[1])
        && (pName[2] == aFileNameHeader[2])
        && (pName[3] == aFileNameHeader[3])
        && (pName[4] == aFileNameHeader[4])
        && (pName[5] == aFileNameHeader[5])
        )
    {
		/* password check */
		if(aFilePassword != password)
		{
			return ECAT_FOE_ERRCODE_NORIGHTS;
		}
	}
	else
	{
        return ECAT_FOE_ERRCODE_DISKFULL;
	}
    sizeError = maxBlockSize;

    if (u32FileSize < sizeError)
    {
        sizeError = u32FileSize;
    }

	/* initilize read data address */
#if defined(MRAM_BANK0)
	ReadDataAddr = FW_UP_BANK0_ADDR;
#elif defined(MRAM_BANK1)
    ReadDataAddr = FW_UP_BANK1_ADDR;
#endif
	pReadData = ( UINT8 *)ReadDataAddr;
	
    /*copy the first foe data block*/
    MEMCPY(pData, pReadData, sizeError);

    return (UINT16)sizeError;
}

UINT16 FoE_ReadData(UINT32 offset, UINT16 maxBlockSize, UINT16 *pData)
{
    UINT32 sizeError = 0;
	UINT8 *pReadData;

    if (u32FileSize < offset)
    {
        return 0;
    }

    /*get file length to send*/
    sizeError = u32FileSize - offset;


    if (sizeError > maxBlockSize)
    {
        /*transmit max block size if the file data to be send is greater than the max data block*/
        sizeError = maxBlockSize;
    }
	/* calculate read data address */
	pReadData = (UINT8 *)(ReadDataAddr + offset);
    /*copy the foe data block 2 .. n*/
	MEMCPY(pData, pReadData, sizeError);
    return (UINT16)sizeError;
}


UINT16 FoE_WriteData(UINT16 MBXMEM * pData, UINT16 Size, BOOL bDataFollowing)
{
    if (bBootMode)
    {
        return BL_Data(pData, Size);
    }
    else
        if ((nFileWriteOffset + Size) > MAX_FILE_SIZE)
        {
            return ECAT_FOE_ERRCODE_DISKFULL;
        }

    if (Size)
    {

        MBXMEMCPY(&aFileData[nFileWriteOffset], pData, Size);

    }

    if (bDataFollowing)
    {
        /* FoE-Data services will follow */
        nFileWriteOffset += Size;
        
    }
    else
    {
        /* last part of the file is written */
        u32FileSize = nFileWriteOffset + Size;
        nFileWriteOffset = 0;
    }

    return 0;
}

UINT16 FoE_Write(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password)
{
    if ((nameSize >= SIZEOF(aFileNameHeader))
        && (pName[0] == aFileNameHeader[0])
        && (pName[1] == aFileNameHeader[1])
        && (pName[2] == aFileNameHeader[2])
        && (pName[3] == aFileNameHeader[3])
        && (pName[4] == aFileNameHeader[4])
        && (pName[5] == aFileNameHeader[5])
        && (pName[6] == aFileNameHeader[6])
        && (pName[7] == aFileNameHeader[7])
        )
    {
        if (bBootMode)
        {
			if(aFilePassword != password)
			{
				return ECAT_FOE_ERRCODE_NORIGHTS;
			}
            BL_StartDownload(password);
            return 0;
        }
        else
        {
            return ECAT_FOE_ERRCODE_BOOTSTRAPONLY;
        }
    }
    else
        if (bBootMode)
        {
            return ECAT_FOE_ERRCODE_NOTINBOOTSTRAP;
        }
        else
    if (nameSize < MAX_FILE_NAME_SIZE)
    {
        /* for test every file name can be written */
        MBXSTRCPY(aFileName, pName, nameSize);
        MBXSTRCPY(aFileName + nameSize, "\0", 1); //string termination


        nFileWriteOffset = 0;
        u32FileSize = 0;
        return 0;
    }
    else
    {
        return ECAT_FOE_ERRCODE_DISKFULL;
    }

}

/*ECATCHANGE_END(V5.12) FOE1*/

/** @} */

