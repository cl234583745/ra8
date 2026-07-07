/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
 * \addtogroup ESM EtherCAT State Machine
 * @{
 */

/**
\file bootmode.h
\author EthercatSSC@beckhoff.com
\brief Function prototypes for the Boot state

\version 5.11

<br>Changes to version V5.01:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"


#ifndef _BOOTMODE_H_
#define _BOOTMODE_H_

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

#endif //_BOOTMODE_H_

#if defined(_BOOTMODE_) && (_BOOTMODE_ == 1)
    #define PROTO
#else
    #define PROTO extern
#endif
 /*-----------------------------------------------------------------------------------------
------
------    Global variables
------
-----------------------------------------------------------------------------------------*/
PROTO const UINT32 tbl_identify[4];
#define ID_VENDOR	(0)
#define ID_PRODUCT	(1)
#define ID_REVISION	(2)
#define ID_SERIAL	(3)
/*-----------------------------------------------------------------------------------------
------
------    Global functions
------
-----------------------------------------------------------------------------------------*/
PROTO    void BL_Start( UINT8 State);
PROTO   void BL_Stop(void);
PROTO    void BL_StartDownload(UINT32 password);
PROTO   UINT16 BL_Data(UINT16 *pData,UINT16 Size);
PROTO void BL_SetRebootFlag(BOOL Flag);
PROTO BOOL BL_CheckRebootFlag(void);
PROTO void BL_Reboot(void);

#undef PROTO
/** @}*/
