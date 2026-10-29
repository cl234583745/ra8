/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "hal_data.h"

#define _SEMI_SAMPLE_APPLICATION_
#include "samplesemi.h"
#undef _SEMI_SAMPLE_APPLICATION_

#include "sdoserv.h"
#include "objdef.h"
#include "ecatslv.h"
#include "samplefoe.h"
#include "bootmode.h"
#include "r_fw_up_buf.h"
#include <stdio.h>

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
extern const UINT16 cBitMask[16];
extern TOBJECT OBJMEM * ObjDicList;

#if BACKUP_PARAMETER_SUPPORTED
static UINT16 write_store_parameter[BACKUP_MEMORY_SIZE_BYTE/2];
#endif
/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/

#if BACKUP_PARAMETER_SUPPORTED
/*******************************************************************************
* Function Name: EE_ResetFlashData
* Description  : This function shall clear the backup parameter memory. It is called from “InitDefaultEntries()”
* Arguments    : none
* Return Value : none
*******************************************************************************/
void EE_ResetFlashData(void)
{

	UINT8 * erase_dest = (UINT8 *)BACKUP_MEMORY_START_ADDRESS;
	if (Check_StoreParameterAddress((UINT32)erase_dest) == false)
	{
		/*---------- Exit ----------*/
	    UINT8 message[] = "address is wrong in EE_ResetFlashData().\r\n";
	    printf("%s", message);
		return;
	}

	/*---------- Erase non-volatile memory for Store Parameter ----------*/
	R_MRAM_Erase(&g_mram0_ctrl, (UINT32)erase_dest, BACKUP_MEMORY_SIZE_BLOCK);

	/*---------- Verify erasing ----------*/
	for (UINT32 i = 0; i < BACKUP_MEMORY_SIZE_BYTE/4; i++)
	{
	    UINT32 *read_addr = (UINT32 *)(BACKUP_MEMORY_START_ADDRESS + i*4);
		if (0xFFFFFFFF != *read_addr)
		{
			UINT8 message[] = "R_MRAM_Erase() is failure in EE_ResetFlashData().\r\n";
			printf("%s", message);
			return;
		}
	}
	/** Erasing non-volatile memory for Store Parameter is complete. **/

#ifdef UART_DEBUG
	UINT8 message[] = "EE_ResetFlashData() is complete.\r\n";
	printf("%s", message);
#endif

} /* End of function EE_ResetFlashData() */

/*******************************************************************************
* Function Name: EE_GetChecksum
* Description  : This function shall return a 32Bit Crc for the backup parameter memory. The return value is stored in 0x10F0.1
* Arguments    : none
* Return Value : UINT32 : Checksum over the stored backup data
*******************************************************************************/
UINT32 EE_GetChecksum(void)
{
	UINT32 u32Checksum = 0;

	/** In this sample program, EE_GetChecksum() is not supported. **/

	return u32Checksum;
} /* End of function EE_GetChecksum() */

/*******************************************************************************
* Function Name: EE_IsDefaultDataInitialized
* Description  : The function is called on slave power up (from “COE_ObjInit()”) and shall check if the backup parameter memory was already initialized.
* Arguments    : none
* Return Value : UINT8 : 0 if the backup parameter memory was not initialized yet. not 0 if the memory was initialized.
*******************************************************************************/
UINT8 EE_IsDefaultDataInitialized(void)
{

	UINT8 result = 0;
	UINT16 *ReadData = (UINT16 *)BACKUP_MEMORY_START_ADDRESS;

	/* Check if default data is initialized. */
	if (Default_Data_Is_Initialized == *ReadData)
	{
		/* The backup parameter memory was already initialized. */
		result = 1;
	}
	else
	{
		result = 0;
	}

#ifdef UART_DEBUG
	if (result == 1)
	{
		UINT8 message[] = "the backup parameter memory was already initialized.\r\n";
		printf("%s", message);
	}
	else
	{
		UINT8 message[] = "the backup parameter memory was not initialized.\r\n";
		printf("%s", message);
	}
#endif

	return result;
} /* End of function EE_IsDefaultDataInitialized() */

/*******************************************************************************
* Function Name: EE_StoreDefaultData
* Description  : The function is called after the default parameter values are written to the memory. Called from “InitDefaultEntries()”
* Arguments    : none
* Return Value : none
*******************************************************************************/
void EE_StoreDefaultData(void)
{
	/** First word of the beginning of the backup memory checks if default backup parameters has been initialized. **/

	uint8_t * write_dest = (uint8_t *)BACKUP_MEMORY_START_ADDRESS;	//Write to Code MRAM.
	if (Check_StoreParameterAddress((UINT32)write_dest) == false)
	{
		/* Exit */
	    UINT8 message[] = "address is wrong in EE_StoreDefaultData().\r\n";
	    printf("%s", message);
		return;
	}

	UINT8 write_data[2];
	write_data[0] = Default_Data_Is_Initialized & 0x00FF;
	write_data[1] = (Default_Data_Is_Initialized & 0xFF00) >> 8;
	/* Write to first word of the beginning of non-volatile memory for Store Parameter */
	fsp_err_t err = R_MRAM_Write(&g_mram0_ctrl, &write_data[0], write_dest, 2);
	if (err != FSP_SUCCESS)
	{
		UINT8 message[] = "Writing to MRAM is failed in EE_StoreDefaultData().\r\n";
		printf("%s", message);
		return;
	}

	/* check if the written data is correct. */
	if (Default_Data_Is_Initialized != *(UINT16 *)write_dest)
	{
		UINT8 message[] = "Writing to first word of the beginning of non-volatile memory for Store Parameter is failure.\r\n";
		printf("%s", message);
	}
	else
	{
#ifdef UART_DEBUG
		/** Writing to first word of the beginning of non-volatile memory for Store Parameter is complete. **/
		UINT8 message[] = "Writing to first word of the beginning of non-volatile memory for Store Parameter is complete.\r\n";
		printf("%s", message);
#endif
	}

} /* End of function EE_StoreDefaultData() */

/*******************************************************************************
* Function Name: EE_LoadDefaultData
* Description  : The function is called before the default parameter values are read from the memory. Called from “LoadDefaultEntries()”
* Arguments    : none
* Return Value : none
*******************************************************************************/
void EE_LoadDefaultData(void)
{

	/** In this sample program, EE_LoadDefaultData() is not supported. **/

} /* End of function EE_LoadDefaultData() */

/*******************************************************************************
* Function Name: EE_ReadWordsFromNonVolatileMemory
* Description  : The function shall copy the stored parameter values to referenced pointer (referencing the object entry). Called from “LoadBackupEntries ()”
* Arguments    : UINT16 * pDest   : destination memory to store the data
				 UINT16 srcOffset : offset within the non-volatile memory
				 UINT16 n         : number of words to be read
* Return Value : none
*******************************************************************************/
void EE_ReadWordsFromNonVolatileMemory(UINT16 HUGE *pDest, UINT16 srcOffset, UINT16 n)
{
    volatile UINT8 dummy_read = 0;
    UINT16 * src = NULL;
    UINT32 i = 0;
    BOOL verify = true;

	/* Copy the store parameters from non-volatile memory. */
    for (i = 0; i < n; i++)
    {
        src = (UINT16 *)(BACKUP_MEMORY_START_ADDRESS + (UINT32)srcOffset*2 + i*2);
        if (Check_StoreParameterAddress((UINT32)src) == false)
        {
            /* Exit */
            UINT8 message[] = "address is wrong in EE_ReadWordsFromNonVolatileMemory().\r\n";
            printf("%s", message);
            return;
        }
        pDest[i] = *src;

    }

    /* Verify */
    for (i = 0; i < n; i++)
    {
        src = (UINT16 *)(BACKUP_MEMORY_START_ADDRESS + (UINT32)srcOffset*2 + i*2);
        if (pDest[i] != *src)
        {
            verify = false;
        }
    }

    if (verify == false)
    {
        UINT8 message[] = "EE_ReadWordsFromNonVolatileMemory() is failure.\r\n";
        printf("%s", message);
    }

    FSP_PARAMETER_NOT_USED(dummy_read);

} /* End of function EE_ReadWordsFromNonVolatileMemory() */

/*******************************************************************************
* Function Name: EE_WriteWordsToNonVolatileMemory
* Description  : The function shall copy the data to the non-volatile memory. Called from “InitDefaultEntries()” ,StoreBackupEntries(), COE_WriteBackupEntry().
* Arguments    : UINT16 dstOffset : offset(word size) within the non-volatile memory
				 UINT16 * pSrc    : source memory to read the data
				 UINT16 n         : number of words to be read
* Return Value : UINT32 : new checksum over the whole stored data
*******************************************************************************/
UINT32 EE_WriteWordsToNonVolatileMemory(UINT16 destOffset, UINT16 HUGE * pSrc, UINT16 n)
{
	/** This sample program is not supported by checksum **/

	UINT32 * read_dst = NULL;
	UINT32 * write_buffer_addr = (UINT32 *)&write_store_parameter[0];
	volatile UINT8 dummy_read = 0;

	/*　Before erasing, copy the current memory. */
    for (UINT32 i = 0; i < BACKUP_MEMORY_SIZE_BYTE/4; i++)
    {
        read_dst = (UINT32 *)(BACKUP_MEMORY_START_ADDRESS + i*4);
        if (Check_StoreParameterAddress((UINT32)read_dst) == false)
        {
            /* Exit */
            UINT8 message[] = "address is wrong in EE_WriteWordsToNonVolatileMemory().\r\n";
            printf("%s", message);
            return 0;
        }
        write_buffer_addr[i] = *read_dst;
    }

	/* Erase non-volatile memory for Store Parameter */
	R_MRAM_Erase(&g_mram0_ctrl, BACKUP_MEMORY_START_ADDRESS, BACKUP_MEMORY_SIZE_BLOCK);

	/* Verify erasing */
	for (UINT32 i = 0; i < BACKUP_MEMORY_SIZE_BYTE/4; i++)
	{
		if (0xFFFFFFFF != *(UINT32 *)(BACKUP_MEMORY_START_ADDRESS + i*4))
		{
			UINT8 message[] = "R_MRAM_Erase() is failure in EE_WriteWordsToNonVolatileMemory().\r\n";
			printf("%s", message);

			return 0;
		}
	}
	/** Erasing non-volatile memory for Store Parameter is complete. **/

	/* Change the write_store_parameter array */
	for (UINT32 i = 0; i < n; i++)
	{
		write_store_parameter[destOffset + i] = pSrc[i];
	}

	/* Write to non-volatile memory for Store Parameter */
	UINT32 write_byte = FW_UP_WRITE_ATONCE_SIZE;
	for (UINT32 i = 0; i < BACKUP_MEMORY_SIZE_BYTE/write_byte; i++)
	{
		/* Set write_dst */
		UINT16 * write_dst = (UINT16 *)(BACKUP_MEMORY_START_ADDRESS + i*write_byte);
		if (Check_StoreParameterAddress((UINT32)write_dst) == false)
		{
			UINT8 message[] = "write_dst is incorrect in EE_WriteWordsToNonVolatileMemory().\r\n";
			printf("%s", message);
			return 0;
		}

		/* Write */
		fsp_err_t err = R_MRAM_Write(&g_mram0_ctrl, (UINT32 *)(&write_store_parameter[i*write_byte/2]), (UINT32 *)write_dst, write_byte);
		if (err != FSP_SUCCESS)
		{
			UINT8 message[] = "Writing to MRAM is failed in EE_WriteWordsToNonVolatileMemory().\r\n";
			printf("%s", message);
			return 0;
		}

	}

	/* Verify if the written data is correct. */
	for (UINT32 i = 0; i < BACKUP_MEMORY_SIZE_BYTE/2; i++)
	{
	    UINT32 read_addr = BACKUP_MEMORY_START_ADDRESS + i*2;
		if (write_store_parameter[i] != *(UINT16 *)read_addr)
		{
			UINT8 message[] = "the written data is incorrect in EE_WriteWordsToNonVolatileMemory().\r\n";
			printf("%s", message);

			return 0;
		}
	}
	/** EE_WriteWordsToNonVolatileMemory() is complete. **/

	Print_WriteWordsToNonVolatileMemory(destOffset);

    FSP_PARAMETER_NOT_USED(dummy_read);

	/** This sample program is not supported by checksum **/
	return 0;
} /* End of function EE_WriteWordsToNonVolatileMemory() */

/*******************************************************************************
* Function Name: Check_StoreParameterAddress
* Description  : This function checks if address is in non-volatile memory for Store Parameter.
* Arguments    : UINT32 address : memory address
* Return Value : none
*******************************************************************************/
BOOL Check_StoreParameterAddress(const UINT32 address)
{
    if ((BACKUP_MEMORY_START_ADDRESS <= address) &&
            (address <= (BACKUP_MEMORY_START_ADDRESS + BACKUP_MEMORY_SIZE_BYTE)))
    {
        return true;
    }
    else if ((BACKUP_MEMORY_START_ADDRESS <= address) &&
            (address <= (BACKUP_MEMORY_START_ADDRESS + BACKUP_MEMORY_SIZE_BYTE)))
    {
        return true;
    }

	return false;
}

/*******************************************************************************
* Function Name: Print_WriteWordsToNonVolatileMemory
* Description  : This function prints writing store parameter to non-volatile memory
* Arguments    : UINT16 dstOffset : offset(word size) within the non-volatile memory
				 UINT16 * pSrc    : source memory to read the data
				 UINT16 n         : number of words to be read
* Return Value : none
*******************************************************************************/
void Print_WriteWordsToNonVolatileMemory(UINT16 destOffset)
{
	if (destOffset < NonVolatileWordOffset_0xF3A1)
	{
		UINT8 message[] = "Index 0x10F0 backup is success.\r\n";
		printf("%s", message);
	}
	else if (destOffset < NonVolatileWordOffset_0xF3A2)
	{
		UINT8 message[] = "Index 0xF3A1 backup is success.\r\n";
		printf("%s", message);
	}
	else if (destOffset < NonVolatileWordOffset_0xF3A3)
	{
		UINT8 message[] = "Index 0xF3A2 backup is success.\r\n";
		printf("%s", message);
	}
	else if (destOffset < NonVolatileWordOffset_0xF3A4)
	{
		UINT8 message[] = "Index 0xF3A3 backup is success.\r\n";
		printf("%s", message);
	}
	else if (destOffset < NonVolatileWordOffset_0xF3A5)
	{
		UINT8 message[] = "Index 0xF3A4 backup is success.\r\n";
		printf("%s", message);
	}
	else if (destOffset < NonVolatileWordOffset_0xF3A6)
	{
		UINT8 message[] = "Index 0xF3A5 backup is success.\r\n";
		printf("%s", message);
	}
	else if (destOffset < NonVolatileWordOffset_0xF3A7)
	{
		UINT8 message[] = "Index 0xF3A6 backup is success.\r\n";
		printf("%s", message);
	}
	else if (destOffset < NonVolatileWordOffset_0xF3A8)
	{
		UINT8 message[] = "Index 0xF3A7 backup is success.\r\n";
		printf("%s", message);
	}
	else
	{
		UINT8 message[] = "Index 0xF3A8 backup is success.\r\n";
		printf("%s", message);
	}

} /* End of function Print_WriteWordsToNonVolatileMemory() */

#endif // #if BACKUP_PARAMETER_SUPPORTED

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param   Index             index of the requested object.
 \param   Subindex          subindex of the requested object.
 \param   Size              Size of the object, returned by the function OBJ_GetObjectLength which was called before
 \param   pData             Pointer to the buffer where the read data shall be copied to
 \param   bCompleteAccess   Indicates if a complete read of all subindices of the object shall be done or not

 \return  result of the read operation (0 (success) or an abort code (ABORTIDX_.... defined in sdosrv.h))

 \brief   This function reads the requested object 0xFBFx
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 ReadFunction0xFBFx( UINT16 Index, UINT8 Subindex, UINT32 Size, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{

	TOBJECT OBJMEM * LocalObjDic = ObjDicList;
	UINT16 i;
	OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry;
	OBJCONST TOBJECT OBJMEM * pObjEntry = NULL;
	for (i = 0; i < 0xFFFF; i++){
		if ( LocalObjDic->Index == Index ){
			pObjEntry = LocalObjDic;
			break;
		}
		LocalObjDic = LocalObjDic->pNext;
	}
	if ( pObjEntry == NULL ){
		return ABORTIDX_OBJECT_NOT_EXISTING;
	}

	UINT16 index = Index;
	UINT8 subindex = Subindex;

	/* lastSubindex is used for complete access to make loop over the requested entries
	to be read, we initialize this variable with the requested subindex that only
	one loop will be done for a single access */
	UINT16 lastSubindex = subindex;

	/* get the information of ObjCode and MaxSubindex in local variables to support different types of microcontroller */
	UINT8 objCode = (pObjEntry->ObjDesc.ObjFlags & OBJFLAGS_OBJCODEMASK) >> OBJFLAGS_OBJCODESHIFT;
	UINT16 maxSubindex = (pObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;

	UINT8 bRead = 0x0;
	UINT8 result = 0;

	if ( objCode != OBJCODE_VAR && index >= 0x1000 )
	{
		/* if the object is an array or record we have to get the maxSubindex from the
		actual value of subindex 0, which is stored as UINT16 at the beginning of the
		object's variable */
		maxSubindex = (*((UINT16 *) (pObjEntry->pVarPtr))) & 0x00FF;
	}

	if ( bCompleteAccess )
	{
		if ( objCode == OBJCODE_VAR || index < 0x1000 )
		{
			/* complete access is not supported with simple objects or ENUM descriptions */
			return ABORTIDX_UNSUPPORTED_ACCESS;
		}

		/* we read until the maximum subindex */
		lastSubindex = maxSubindex;
	}

	/* a variable object is read */
	for (i = subindex; i <= lastSubindex; i++)
	{
		/* if only a single entry is requested, this loop will only be done once */
		UINT16 MBXMEM *pVarPtr = (UINT16 MBXMEM *) pObjEntry->pVarPtr;
		UINT16 bitOffset = 0;

		if (i == 0)
		{
			/* subindex 0 is requested, the entry's data is at the beginning of the object's variable */
		}
		else if ( index >= 0x1000 )
		{
			/* subindex 1-n of an variable object is requested, we get the offset of the variable here */
			bitOffset = OBJ_GetEntryOffset((UINT8)i, pObjEntry);
		}

		/* we increment the variable pointer to the corresponding word address */
		pVarPtr += (bitOffset >> 4);

		/* get the corresponding entry description */
		pEntry = OBJ_GetEntryDesc(pObjEntry, (UINT8)i);
		if (0 != (((UINT8) ((pEntry->ObjAccess & ACCESS_READ)<<1)) & (nAlStatus & STATE_MASK)) )
		{
			if ( i == subindex                                     /* requested entry */
				|| (bCompleteAccess && i >= subindex) )       /* complete access and entry should be read */
			{
				UINT16 bitMask;

				/* we have to copy the entry */
				if ( i == 0 && objCode != OBJCODE_VAR )
				{
					/* we read subindex 0 of an array or record */
					{
						pData[0] = SWAPWORD((UINT16)maxSubindex);
					}

					/* we increment the destination pointer by 2 because the subindex 0 will be
					transmitted as UINT16 for a complete access */
					pData++;
				}
				else
				{
					UINT16 dataType = pEntry->DataType;

					if (pEntry->DataType >= 0x700)
					{
						/* the ENUM data types are defined from index 0x700 in this example
						convert in standard data type for the read access */
						if ( pEntry->BitLength <= 8 )
						{
							dataType = DEFTYPE_BIT1-1+pEntry->BitLength;
						}
						else if ( pEntry->BitLength == 16 )
						{
							dataType = DEFTYPE_UNSIGNED16;
						}
						else if ( pEntry->BitLength == 32 )
						{
							dataType = DEFTYPE_UNSIGNED32;
						}
					}

					switch (dataType)
					{
					case DEFTYPE_NULL:
						if(bCompleteAccess)
						{
							/*Handle alignment entry*/
							if (((pEntry->BitLength & 0xF) > 0)
								&& (((bitOffset + pEntry->BitLength) & 0x0F) == 0 ))
							{
								/* we have reached the UINT16 border */
								pData++;
							}

							/*increment WORD offset*/
							pData += ((pEntry->BitLength & 0xF0) >> 4);
						}
						else
						{
							return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED;
						}
						break;
					case    DEFTYPE_UNSIGNED8:
						{
							/* depending on the bitOffset we have to copy the Hi or the Lo-Byte */
							UINT16 TmpValue = 0x0000;

							bitMask = cBitMask[pEntry->BitLength] << (bitOffset & 0x0F);

							/*Swap object data (if required); all masks and offsets are defined for little endian format*/
							TmpValue = SWAPWORD(pVarPtr[0]);

							/*Clear pData if the first bits within the WORD memory will be copied*/
							if ((bitOffset & 0x0F) == 0)
							{
								pData[0] = 0;
							}

							pData[0] = SWAPWORD(pData[0]);

							if (bCompleteAccess)
							{
								/*shifting is not required for Complete access because the bits are set to the correct offset by the master*/
								pData[0] |= TmpValue & bitMask;
							}
							else
							{
								/*Shift Bits to the beginning of the mailbox memory*/
								pData[0] |= ((TmpValue & bitMask) >> (bitOffset & 0x0F));
							}

							pData[0] = SWAPWORD(pData[0]);
							if ( ((bitOffset + pEntry->BitLength) & 0x0F) == 0 )
							{
								/* we have reached the UINT16 border */
								pData++;
							}

						}
						break;
					case    DEFTYPE_OCTETSTRING:
						if(bitOffset & 0xF)
						{
							/* fill in the 1 Byte that are not filled in. */
							UINT16 TmpValue = pVarPtr[0];
							OBJTOMBXMEMCPY(pData, &TmpValue, 2);
							/* we have reached the UINT16 border */
							pData++;
							pVarPtr++;

							UINT16 RemainingReadByte = BIT2BYTE(pEntry->BitLength) - 1;
							/*Read two bytes at a time*/
							while( RemainingReadByte > 2 )
							{
								OBJTOMBXMEMCPY(pData, pVarPtr, 2);
								/* we have reached the UINT16 border */
								pData++;
								pVarPtr++;
								RemainingReadByte -= 2;
							}

							/*copy only 1 byte*/
							if (RemainingReadByte == 1)
							{
								UINT8 *tmp = NULL;
								tmp = (UINT8 *) pVarPtr;
								TmpValue = tmp[0] & 0x00FF;
								OBJTOMBXMEMCPY(pData, &TmpValue, 2);
							}

							break;
						}

						OBJTOMBXMEMCPY(pData, pVarPtr, BIT2BYTE(pEntry->BitLength));

						pData += BIT2WORD((pEntry->BitLength & ~0xF));

						if((pEntry->BitLength & 0xF) != 0)
						{
							/*current entry has an odd word length => clear last byte of next word*/
							*pData &= 0xFF;
						}

						break;
					default:
						/* other data types are not supported from this example */
						return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED;
					} //switch (deftype)

					bRead = 1;
				}
			}
		}
		else
		{
			/*No access to current object entry => shift pData if required*/
			UINT8 cnt = 0;

			/*If this entry is the first in the 16Bit block clear the memory*/
			if ((bitOffset & 0x0F) == 0)
			{
				*pData = 0;
			}

			/*Handle Bit/Byte Offset*/
			if (((pEntry->BitLength & 0xF) > 0)
				&& (((bitOffset + pEntry->BitLength) & 0x0F) == 0 ))
			{
				/* we have reached the UINT16 border */
				pData++;

				/*Clear new buffer*/
				*pData = 0;
			}

			/*increment WORD offset*/
			for(cnt = 0; cnt <((pEntry->BitLength & 0xF0) >> 4); cnt++)
			{

				/*current 16Bit are skipped => clear current buffer */
				pData++;

				/*Clear new buffer*/
				*pData = 0;
			}

			/* we don't have read access */
			if ( (pEntry->ObjAccess & ACCESS_READ) == 0 )
			{
				/* it is a write only entry */
				result = ABORTIDX_WRITE_ONLY_ENTRY;
			}
			else
			{
				/* we don't have read access in this state */
				result = ABORTIDX_IN_THIS_STATE_DATA_CANNOT_BE_READ_OR_STORED;
			}
		}
	}

	if(bRead == 0)
	{
		return result;
	}

	FSP_PARAMETER_NOT_USED(Size);

	return 0;
}

/*******************************************************************************
* Function Name: Device_Reset
* Description  : This function executes the Device Reset described in ETG.5003.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void Device_Reset(void)
{

    NVIC_SystemReset();    // System Software Reset
    while(1)
    {
        /* Do nothing */
    };

}

/**
 \param     index                    index of the requested object.
 \param     subindex                subindex of the requested object.
 \param    dataSize                received data size of the SDO Download
 \param    pData                    Pointer to the buffer where the written data can be copied from
 \param    bCompleteAccess    Indicates if a complete write of all subindices of the
                                     object shall be done or not

 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This function writes the object 0xFBF0
*/
UINT8 Write0xFBF0( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{
	if ( bCompleteAccess )
	{
		/* Complete Access is not supported for object 0xFBF0 */
		return ABORTIDX_UNSUPPORTED_ACCESS;
	}

	if (( subindex == 0 ) || (subindex == 2) || (subindex == 3))
	{
		/* Subindex 0, 2, 3 is not writable */
		return ABORTIDX_READ_ONLY_ENTRY;
	}

	if ((0x6574 == pData[0]) && (0x6573 == pData[1]) && (0x0072 == pData[2]))
	{
		UINT8 message[] = "Execute Device_Reset().\r\n";
		printf("%s", message);
		Device_Reset();
	}

	FSP_PARAMETER_NOT_USED(dataSize);
	FSP_PARAMETER_NOT_USED(index);

	return 0;
}

/**
 \param     index                    index of the requested object.
 \param     subindex                subindex of the requested object.
 \param    dataSize                received data size of the SDO Download
 \param    pData                    Pointer to the buffer where the written data can be copied from
 \param    bCompleteAccess    Indicates if a complete write of all subindices of the
                                     object shall be done or not

 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This function writes the object 0xFBF2
*/
UINT8 Write0xFBF2( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{
	if ( bCompleteAccess )
	{
		/* Complete Access is not supported for object 0xFBF0 */
		return ABORTIDX_UNSUPPORTED_ACCESS;
	}

	if (( subindex == 0 ) || (subindex == 2) || (subindex == 3))
	{
		/* Subindex 0, 2, 3 is not writable */
		return ABORTIDX_READ_ONLY_ENTRY;
	}

#if STORE_BACKUP_PARAMETER_IMMEDIATELY
	FSP_PARAMETER_NOT_USED(pData);
#else
	if ((0x6173 == pData[0]) && (0x6576 == pData[1]))
	{
#if BACKUP_PARAMETER_SUPPORTED

		StoreBackupEntries();

#endif // #if BACKUP_PARAMETER_SUPPORTED
	}
	else
	{
		return ABORTIDX_PARAM_IS_INCOMPATIBLE;
	}
#endif

	FSP_PARAMETER_NOT_USED(dataSize);
	FSP_PARAMETER_NOT_USED(index);

	return 0;
}
