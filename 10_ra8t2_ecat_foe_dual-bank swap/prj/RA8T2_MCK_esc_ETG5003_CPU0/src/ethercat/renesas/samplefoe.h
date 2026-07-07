/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
 * \addtogroup SampleFoe FoE Sample functions
 * @{
 */

/**
\file samplefoe.h
\author EthercatSSC@beckhoff.com
\brief FoE Sample functions specific objects

 */

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#ifndef _SAMPLE_FOE_H_
#define _SAMPLE_FOE_H_

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
/*---------------------------------------------
-    SII EEPROM Access (word access)
-----------------------------------------------*/
#define		SII_EEP_IDENTIFY_OFFSET		0x08
#define		SII_EEP_VENDORID			0x00
#define		SII_EEP_PRODUCTCODE			0x02
#define		SII_EEP_REVESIONNO			0x04
#define		SII_EEP_SERIALNO			0x06
typedef union
{
	UINT32	dword[4];
	UINT16	word[8];
	UINT8	byte[16];
}EEPBUFFER;

extern UINT16 FoE_Read(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password, UINT16 maxBlockSize, UINT16 *pData);
extern UINT16 FoE_ReadData(UINT32 offset, UINT16 maxBlockSize, UINT16 *pData);
extern UINT16 FoE_Write(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password);
extern UINT16 FoE_WriteData(UINT16 MBXMEM * pData, UINT16 Size, BOOL bDataFollowing);


/*-----------------------------------------------------------------------------------------
------
------    Objects
------
-----------------------------------------------------------------------------------------*/
#ifdef _FOE_SAMPLE_APPLICATION_
    #define PROTO
#else
    #define PROTO extern
#endif

#ifdef _OBJD_
/**
 * \brief Object 0x5000 (Firmware Writable Bank) entry description
 *
 * This object is a variable so only one entry is defined
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM EntryDesc0x5000 = {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ};


/** \brief Object 0x5000 (Firmware Writable Bank) name*/
OBJCONST UCHAR OBJMEM aName0x5000[] = "Firmware Writable Bank";
#endif //#ifdef _OBJD_


/** \brief Object 0x5000 (Firmware Writable Bank) variable*/
PROTO UINT8 FirmwareWritableBank
#ifdef _FOE_SAMPLE_APPLICATION_
 #if defined(MRAM_BANK0)
    = 0x1
 #elif defined(MRAM_BANK1)
    = 0x0
 #endif
#endif // #ifdef _FOE_SAMPLE_APPLICATION_
;

#ifdef _OBJD_
/**
 * \brief Object dictionary related to the IO Application
 */

TOBJECT    OBJMEM FoEApplicationObjDic[] = {
    /* Object 0x5000 */
    {NULL,NULL, 0x5000, {DEFTYPE_UNSIGNED8, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x5000, aName0x5000, &FirmwareWritableBank, NULL, NULL, 0x0000 },

    {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL, NULL, 0x000}
};
#endif    //#ifdef _OBJD_

#undef PROTO

#endif  // #ifndef _SAMPLE_FOE_H_
/** @}*/
